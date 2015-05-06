% Train neurostalker based on groundtruth extracted from swf trees and input from imagesstacks
% This version disregard the memory cost of loading training cases
clc; clear all; close all;
% PARA
TESTSIZE = 1;
TESTSTARTLOC = [[76.663, 324, 9.749]];
TESTSTARTDIR = [[-0.9278, 0.1916]];
NTESTSTEP = 40;

NOISETYPE = 'original'; % The noise added to vision blocks: original, gauss, salt_pepper
NTREE = 200;
MTRY = 140;

PREFIX = 'OP_';
% ENDPARA

curdir = fileparts(mfilename('fullpath'));
datadir = fullfile(curdir, '..', 'data', 'input', 'preprocessed');

nsbj = numel(dir([datadir, [filesep, '*.mat']]));

disp(['cur dir:', curdir]);
disp(['data dir:', datadir]);
disp(dir([datadir, [filesep, '*.mat']]));
disp(sprintf('nsbj: %d\n', nsbj));

% Read in the robots 
% Only deal with low memory use
for i = 1 : nsbj
    fsbj = load(fullfile(datadir, strcat(PREFIX, num2str(nsbj), '.mat')), 'sbj');	
    lrobot{i} = fsbj.sbj;
end

ltrainrobot = lrobot{1 : numel(lrobot)-TESTSIZE}
ltestrobot = lrobot{numel(lrobot)-TESTSIZE+1 : end}

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

% Make the Train X and Y
vboxsize = lrobot{1}.vboxsize;
train_x = zeros(ntrain, vboxsize^3);
row = 1;
for i = 1 : numel(lrobot) - TESTSIZE
    for j = 1 : numel(lrobot{i})
    	disp(sprintf('Reading train matrix row %d\n', row));
    	vb = lrobot{i}{j}.visionbox.(NOISETYPE);

        if lrobot{i}{j}.fissure == 1 % Only use the vboxes not at branching locations
        	continue;
        end

    	train_x(row, :) = vb(:);
        train_y(row, 1) = lrobot{i}{j}.next_alpha;
        train_y(row, 2) = lrobot{i}{j}.next_beta;
        row = row + 1;
    end
end

train_x(row:end,:) = []; % Delete the redundent rows not used because of the branching
train_y(row:end,:) = [];

clearvars lrobot;

% Train RF
%tic; model_x_dir = regRF_train(train_x, train_y, NTREE, MTRY); toc;
disp('Start to train RF...');
tic; rf = TreeBagger(NTREE, train_x, train_y, 'Method', 'regression', 'NVarToSample', MTRY); toc;

% % Test RF
% for r = 1 : numel(ltestrobot)
% 	testimg = ltestrobot{r}.img3d.(NOISETYPE);
% 
% 	% Start Location
% 	curnode.x = TESTSTARTLOC(r, 1);
% 	curnode.y = TESTSTARTLOC(r, 2);
% 	curnode.z = TESTSTARTLOC(r, 3);
% 
% 	% Start Direction
% 	curnode.prev_th = TESTSTARTDIR(r, 1);
% 	curnode.prev_phi = TESTSTARTDIR(r, 2);
% 
%     for s = 1 : NTESTSTEP 
%         
%     end
% 
% 	line([curnode.x, next.x], [curnode.y,next.y], [curnode.z, next.z],'Color','k','LineWidth',5);
% 
% 	ori_saved_box = savebox(ag_three_dim, box_size_define, curnode.x,curnode.y,curnode.z,zero_size);
% end
% 
% hold on
% 
% for step=1:40
% 
% 	pre_x_dir = cur_x_dir;
% 	pre_y_dir = cur_y_dir;
% 	pre_z_dir = cur_z_dir;
% 
% 	cur_x_dir = regRF_predict(tmp_voxel', model_x_dir);
% 	cur_y_dir = regRF_predict(tmp_voxel',model_y_dir);
% 	cur_z_dir = regRF_predict(tmp_voxel',model_z_dir);
% 	cur_mg = regRF_predict(tmp_voxel',model_mg);
% 	[cur_x_dir, cur_y_dir, cur_z_dir]=decide_sign(pre_x_dir, pre_y_dir, pre_z_dir, cur_x_dir, cur_y_dir, cur_z_dir);
% 	next.x=curnode.x+cur_x_dir*cur_mg;
% 	next.y=curnode.y+cur_y_dir*cur_mg;
% 	next.z=curnode.z+cur_z_dir*cur_mg*0.1;
% 	line([curnode.x, next.x], [curnode.y,next.y], [curnode.z, next.z],'Color','k','LineWidth',5);
% 	pause(0.5)
% 	curnode.x=next.x;
% 	curnode.y=next.y;
% 	curnode.z=next.z;
% end