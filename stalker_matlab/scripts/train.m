% Train neurostalker based on groundtruth extracted from swf trees and input from imagesstacks
% This version disregard the memory cost of loading training cases
clc; clear all; close all;
% PARA
TESTSIZE = 1;
TESTSTARTLOC = [[76.663, 324, 9.749]];
TESTSTARTDIR = [[-0.9278, 0.1916]];
NTESTSTEP = 40;
STEPSIZE = 0.4;

NOISETYPE = 'original'; % The noise added to vision blocks: original, gauss, salt_pepper
NTREE = 200;
MTRY = 140;

CACHETRAIN = true;

PREFIX = 'OP_';

CACHETRAINDATA = true;
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
    tic; rf_th = TreeBagger(NTREE, train_x, train_y(:, 1), 'Method', 'regression', 'NVarToSample', MTRY, 'NPrint', true,'Options',options); toc;
    tic; rf_phi = TreeBagger(NTREE, train_x, train_y(:, 2), 'Method', 'regression', 'NVarToSample', MTRY, 'NPrint', true,'Options',options); toc;

    if CACHETRAINMODEL 
        save(fullfile(curdir, 'modelcache.mat'), 'rf_th', 'rf_phi');
    end
end

% % Test RF
% Read in the test subject

img3dctr = 1; % counter for img3d cell array
for i = nsbj - TESTSIZE + 1 : nsbj 
    fsbj = load(fullfile(datadir, strcat(PREFIX, num2str(nsbj), '.mat')), 'sbj', 'img3d'); 
    ltestrobot{img3dctr} = fsbj.sbj;
    img3d{img3dctr} = fsbj.img3d;
    img3dctr = img3dctr + 1;
end

for r = 1 : numel(ltestrobot)
    disp(img3d{r});
 	testimg = img3d{r}.(NOISETYPE);
 
 	% Start Location
 	curnode.x = TESTSTARTLOC(r, 1);
 	curnode.y = TESTSTARTLOC(r, 2);
 	curnode.z = TESTSTARTLOC(r, 3);
 
 	% Start Direction
 	curnode.prev_th = TESTSTARTDIR(r, 1);
 	curnode.prev_phi = TESTSTARTDIR(r, 2);

    % Iterate each step of the prediction
    for s = 1 : NTESTSTEP 
        disp(['ltestrobot', ltestrobot])
        disp({'ltestrobot{r}', ltestrobot{r}})
        disp(fieldnames(ltestrobot{r}))
        vbox = extractbox(testimg, ltestrobot{r}.vboxsize, curnode.x, curnode.y, curnode.z, ltestrobot{r}.zerosize);
        th = rf_th.predict(vbox(:)');
        phi = rf_phi.predict(vbox(:)');

        % Move one step
        [dx, dy, dz] = sph2cart(th, phi, STEPSIZE);
        nextnode.x = curnode.x + dx;
        nextnode.y = curnode.y + dy;
        nextnode.z = curnode.z + dz*0.1;

        fprintf('Move to direction (%f, %f) - (%f, %f, %f)\n', th, phi, dx, dy, dz);
        line([curnode.x, nextnode.x], [curnode.y, nextnode.y], [curnode.z, nextnode.z], 'Color','k', 'LineWidth',5);
        curnode = nextnode;
    end
 end
