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

%     Read in the robots 
%     Only deal with low memory use
    for i = 1 : nsbj-TESTSIZE
        fsbj = load(fullfile(datadir, strcat(PREFIX, num2str(nsbj), '.mat')), 'sbj');   
        ltrainrobot{i} = fsbj.sbj;
    end

    %ltrainrobot = lsbj{1 : numel(lsbj)-TESTSIZE};
    %ltestrobot = lsbj{numel(lsbj)-TESTSIZE+1 : end};
    
    ltestrobot_counter = 1;
    for i = (nsbj - TESTSIZE + 1) : nsbj
        fsbj = load(fullfile(datadir, strcat(PREFIX, num2str(nsbj), '.mat')), 'sbj');   
        ltestrobot{ltestrobot_counter} = fsbj.sbj;
        ltestrobot_counter = ltestrobot_counter + 1;
    end
    
    
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

 