function train()
% Train neurostalker based on groundtruth extracted from swf trees and input from imagesstacks
% This version disregard the memory cost of loading training cases
tic;
clc; clear all; close all;

% PARA
TESTSIZE        = 1;
TESTSTARTLOC    = [[89, 363, 30]];
TESTSTARTDIR    = [[-0.9278, 0.1916]];
NTESTSTEP       = 100;
STEPSIZE        = 1;
NOISETYPE       = 'original'; % The noise added to vision blocks: original, gauss, salt_pepper
NTREE           = 200;
MTRY            = 140;
CACHETRAIN      = true;
PREFIX          = 'OP_';
CACHETRAINDATA  = true;
CACHETRAINMODEL = true;
% ENDPARA

curdir = fileparts(mfilename('fullpath'));
datadir = fullfile(curdir, '..', 'data', 'input', 'preprocessed');
addpath(fullfile(curdir, '..', 'utils'));

nsbj = numel(dir([datadir, [filesep, '*.mat']]));

disp(['cur dir:', curdir]);
disp(['data dir:', datadir]);
disp(dir([datadir, [filesep, '*.mat']]));
disp(sprintf('nsbj: %d\n', nsbj));


% Make the Train X and Y
% Load the cached Train X and Y matrix if possible
if CACHETRAINDATA && exist(fullfile(curdir, 'traincache.mat'))
    fcache = load(fullfile(curdir, 'traincache.mat'));
    train_x = fcache.train_x;
    train_y = fcache.train_y;
    clearvars fcache; 
else
    % Read in the robots 
    % Only deal with low memory use
    for i = 1 : nsbj-TESTSIZE
        fsbj = load(fullfile(datadir, strcat(PREFIX, num2str(nsbj), '.mat')), 'sbj');   
        ltrainrobot{i} = fsbj.sbj;
    end

    %ltrainrobot = lsbj{1 : numel(lsbj)-TESTSIZE};
    %ltestrobot = lsbj{numel(lsbj)-TESTSIZE+1 : end};

    % Count the train vbox
    ntrain = 0;
    for r = 1 : numel(ltrainrobot)
        ntrain = ntrain + numel(ltrainrobot(r));
    end

    % Count the test vbox
    ntest = 0;
    for r = 1 : numel(ltestrobot)
        ntest = ntest + numel(ltestrobot(r));
    end

    vboxsize = lsbj{1}.vboxsize;
    train_x = zeros(ntrain, vboxsize^3);
    row = 1;
    for i = 1 : numel(lsbj) - TESTSIZE
        for j = 1 : numel(lsbj{i}.lrobot)
            disp(sprintf('Reading train matrix row %d\n', row));
            vb = lsbj{i}.lrobot(j).visionbox.(NOISETYPE);

            if lsbj{i}.lrobot(j).fissure == 1 % Only use the vboxes not at branching locations
                continue;
            end

        	train_x(row, :) = vb(:);
            train_y(row, 1) = lsbj{i}.lrobot(j).next_th;
            train_y(row, 2) = lsbj{i}.lrobot(j).next_phi;
            row = row + 1;
        end
    end

    train_x(row:end,:) = []; % Delete the redundent rows not used because of the branching
    train_y(row:end,:) = [];

    if CACHETRAINDATA % Cache the X and Y matrix
        save(fullfile(curdir, 'traincache.mat'), 'train_x', 'train_y');
    end
end

clearvars lsbj;

% Train RF
%tic; model_x_dir = regRF_train(train_x, train_y, NTREE, MTRY); toc;
if CACHETRAINMODEL && exist(fullfile(curdir, 'modelcache.mat'))
    fmodel = load(fullfile(curdir, 'modelcache.mat'), 'rf_th', 'rf_phi');
    rf_th = fmodel.rf_th;
    rf_phi = fmodel.rf_phi;
else
    disp('Start to train RF...');
    options = statset('UseParallel', 'Always');
    
    % When (2*pi > theta > pi) we invert the ground truth vector to its
    % inversed direction
    orith = train_y(:, 1);
    oriphi = train_y(:, 2);
    [gtx, gty, gtz] = sph2cart(orith, oriphi, ones(numel(orith, 1)));
    [invth, invphi] = cart2sph(-gtx, -gty, -gtz);
    gtth = orith;
    gtphi = oriphi;
    gtth(orith > pi) = invth(orith > pi);
    gtphi(orith > pi) = invphi(orith > pi);
    disp('gtth')
    disp(gtth);

    % Train Theta
    rf_th = TreeBagger(NTREE, train_x, gtth,...
                            'Method', 'regression', 'NVarToSample',...
                             MTRY, 'NPrint', true,'Options',options); 

    % Train Phi
    rf_phi = TreeBagger(NTREE, train_x, gtphi,... 
                             'Method', 'regression', 'NVarToSample',...
                              MTRY, 'NPrint', true,'Options',options);

    if CACHETRAINMODEL 
        save(fullfile(curdir, 'modelcache.mat'), 'rf_th', 'rf_phi');
    end
end



% % Test RF by walking 
% Read in the test subject
img3dctr = 1; % counter for img3d cell array
for i = nsbj - TESTSIZE + 1 : nsbj 
    fsbj = load(fullfile(datadir, strcat(PREFIX, num2str(nsbj), '.mat')), 'sbj', 'img3d'); 
    ltestrobot{img3dctr} = fsbj.sbj;
    img3d{img3dctr} = fsbj.img3d;
    img3dctr = img3dctr + 1;
end


for r = 1 : numel(ltestrobot)
    figure
    hold on
    showbox(img3d{r}.original, 0.1);
 	testimg = img3d{r}.(NOISETYPE);
 
 	% Start Location
 	curnode.x = TESTSTARTLOC(r, 1);
 	curnode.y = TESTSTARTLOC(r, 2);
 	curnode.z = TESTSTARTLOC(r, 3);
    fprintf('Starting at (%f, %f, %f)', curnode.x, curnode.y, curnode.z);
 
 	% Start Direction
 	curnode.prev_th = TESTSTARTDIR(r, 1);
 	curnode.prev_phi = TESTSTARTDIR(r, 2);
    fprintf('Starting from direction (%f, %f)\n', curnode.prev_th, curnode.prev_phi);

    % Iterate each step of the prediction
    for s = 1 : NTESTSTEP 
        % Extract the vbox of the stalker
        vbox = extractbox(testimg, ltestrobot{r}.vboxsize,...
                          curnode.x, curnode.y, curnode.z,...
                          ltestrobot{r}.zerosize);

        th = rf_th.predict(vbox(:)');
        phi = rf_phi.predict(vbox(:)');

        % Calculate the cosine of the forward and backward
        % Larger cosine means smaller angle
        % Choose the direction with the small angle based on the assumption that turning angles larger than pi/2
        % is not common in bio systems
        dcosforward = sphveccos(th, phi, curnode.prev_th, curnode.prev_phi);
        dcosbackward = sphveccos(th+pi, -phi, curnode.prev_th, curnode.prev_phi);

        if dcosbackward > dcosforward
            [x, y, z] = sph2cart(th, phi);
            [th, phi] = cart2sph(-x, -y, -z);
        end

        % Move one step
        [dx, dy, dz] = sph2cart(th, phi, STEPSIZE);
        nextnode.x = curnode.x + dx;
        nextnode.y = curnode.y + dy;
        nextnode.z = curnode.z + dz;
        nextnode.prev_th = th;
        nextnode.prev_phi = phi;

        fprintf('Step %d: Move to direction (%f, %f) - (%f, %f, %f)\n', s, th, phi, dx, dy, dz);
        line([curnode.x, nextnode.x], [curnode.y, nextnode.y], [curnode.z, nextnode.z], 'Color','k', 'LineWidth',5);
        curnode = nextnode;
        drawnow
    end
    hold off
end

end

