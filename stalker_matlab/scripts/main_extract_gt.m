function main_extract_gt()
% this script can extract multiple robot(voxel box and spherical angles. prepare for the training) 
clear all; clc; close all; warning on;
curdir = fileparts(mfilename('fullpath'));
disp(sprintf('currentpath:%s', curdir));

% PARA
ZERO_SIZE = 20;
FRTHRESHOLD = 15;
SALTPEPPER_PERCENT = 0.0001;
GAUSS_PERCENT = 0.1;
GAUSS_VARIANCE = 1;
NCLUSTER = 20;
VBOXSIZE = 13;
%

%Add the script folder into path
addpath(genpath(pwd));

% Add dependencies 
addpath(genpath(fullfile(curdir, '..', 'lib')));
addpath(genpath(fullfile(curdir, '..', 'utils')));


% Try to enter the raw image ground truth path
gtpath = fullfile(curdir, '..', 'data', 'input', 'raw', 'groundtruth'); 
oppath = fullfile(curdir, '..', 'data', 'input', 'raw', 'rawimage', 'op_raw_imagestack'); 
     
% It will iteratively load the ground truth
if input('Do you want show original 3D images? \nOption 1: Show  Option 2: Do not show:') == 1
    SHOWIMG = 'DISPLAY'
else
    SHOWIMG = 'NODISPLAY'
end

if input('Do you want show original ground truth? \nOption 1: Show  Option 2: Do not show:') == 1
    SHOWGT = 'DISPLAY' 
else
    SHOWGT = 'NODISPLAY' 
end

prefix='OP_';

ltrainrobot=[];
ltestrobot=[];

lrobot = {};
nrobot = 0;
for i = 1 : length(dir([gtpath, [filesep, '*.swc']])) % iterate each subject
    sbjid = [prefix num2str(i)];
    disp(sprintf('Working On Sbj %s\n', sbjid));
    sbjimgpath = fullfile(oppath, sbjid);
    sbjgtpath = fullfile(gtpath, sbjid);
    nfile = length(dir([sbjimgpath, [filesep, '*.tif']]));
    img3d = raw_image_prep(nfile, sbjimgpath, SHOWIMG, FRTHRESHOLD, ZERO_SIZE,...
                           SALTPEPPER_PERCENT, GAUSS_PERCENT, GAUSS_VARIANCE, NCLUSTER);
    preppath = fullfile('preprocessed', 'preprocessed_images'); 
    % Extract directions and radius
    sbj.lrobot = extract_gt(img3d, sbjgtpath, SHOWGT, ZERO_SIZE, VBOXSIZE);
    sbj.imgpath = sbjimgpath;
    sbj.gtpath = sbjgtpath;
    %sbj.img3d = img3d;
    sbj.zerosize = ZERO_SIZE;
    sbj.vboxsize = VBOXSIZE;
    save(fullfile(curdir, '..', 'data', 'input', 'preprocessed', strcat(num2str(sbjid),'.mat')), 'sbj', 'img3d');
    clearvars sbj;
    clearvars img3d;
end

end


function robot = extract_gt(img3d, sbjpath, SHOWIMG, zero_size, vboxsize)
% Extract the voxel vision blocks of 1 imagestack and the ground truth
% directions
% Parameters: 
% img3d - a structure of input voxels (3D image stack) of 1 single subject 
%         fields -- original, salt_pepper, gauss
%
% sbjpath - the name before .swc
% SHOWIMG - 'DISPLAY'/'NODISPLAY'
% zero_size - The size of the zero-padding on each plane of the cube (3D image stack). 
%addpath(genpath(fullfile('..', 'lib', '@tree'))); % We need to add 

swc = importdata([sbjpath '.swc']);

% Parse Each Line of the swc file
for i = 1:size(swc.data, 1)
    robot_ground_truth(i).nodeidx = swc.data(i, 1);
    robot_ground_truth(i).type = swc.data(i, 2);
    robot_ground_truth(i).x_loc = swc.data(i, 3);
    robot_ground_truth(i).y_loc = swc.data(i, 4);
    robot_ground_truth(i).z_loc = swc.data(i, 5);
    robot_ground_truth(i).radius = swc.data(i, 6);
    robot_ground_truth(i).p_nodeidx = swc.data(i, 7);
end

% Find the root node whose parind is -1
lparind = swc.data(:, 7);
[~, I] = find(lparind == -1);

assert(numel(I) == 1, 'One and only one root node should be found in this .swc');

t = tree(robot_ground_truth(1)); % Tree structure in ../lib/@tree
[t n(1)] = t.addnode(1, robot_ground_truth(2));

for i=2:1:numel(lparind)-1
    par_n = lparind(i+1);
    par_n = par_n-1;
    [t n(i)] = t.addnode(n(par_n), robot_ground_truth(i+1));
end

if SHOWIMG==1
    figure
    hold on
    for i=1:(numel(lparind)-1)
    test=t.get(n(i));
    plot3(test.x_loc, test.y_loc, test.z_loc, 'r+');
    %pause(0.1)
    end
end

df_order = tree(t, 'clear'); % Generate an empty synchronized tree
iterator = t.depthfirstiterator; % Doesn't matter whether you call this on |t| or |df_order|
iterator=iterator-1;

% Initialize the root parameter
curnode = t.get(1); % t is the whole tree, t.get(1) is the root
fields = fieldnames(img3d);

for i = 1 : numel(fields)
    robot(1).visionbox.(fields{i}) = savebox(img3d.(fields{i}), vboxsize, curnode.x_loc,curnode.y_loc,curnode.z_loc, zero_size);
    robot(1).fissure = 0;
end

% Move to the second node
curnode = t.get(n(1)); % n(1) is the first children
for i = 1 : numel(fields)
    robot(2).visionbox.(fields{i}) = savebox(img3d.(fields{i}), vboxsize, curnode.x_loc,curnode.y_loc,curnode.z_loc, zero_size);
    robot(2).fissure=0;
end

% Initilize the Cartisian unit vector of node 1 and node 2
% Because they are special 
curnode = t.get(1);
nextnode = t.get(n(1));
next_x = nextnode.x_loc-curnode.x_loc;
next_y = nextnode.y_loc-curnode.y_loc;
next_z = nextnode.z_loc-curnode.z_loc;
next_magnitude = sqrt(next_x*next_x+next_y*next_y+next_z*next_z);
next_x_direction = next_x/next_magnitude;
next_y_direction = next_y/next_magnitude;
next_z_direction = next_z/next_magnitude;
robot(1).next_x_dir = next_x_direction;
robot(1).next_y_dir = next_y_direction;
robot(1).next_z_dir = next_z_direction;
robot(1).next_mag = next_magnitude;
robot(1).radius = curnode.radius;

robot(1).prev_x_dir = next_x_direction;
robot(1).prev_y_dir = next_y_direction;
robot(1).prev_z_dir = next_z_direction;
robot(1).prev_mag = next_magnitude;

% Initialize the children parameter
for i = 2:numel(lparind)-1
    node_ind = iterator(i);
    curnode = t.get(n(node_ind));
    
    % Radius
    robot(i).radius = curnode.radius;

    % Save visionboxes to robots from tree
    fields = fieldnames(img3d);
    for j = 1 : numel(fields)
        robot(i).visionbox.(fields{j}) = savebox(img3d.(fields{j}), vboxsize, curnode.x_loc,curnode.y_loc,curnode.z_loc,zero_size);
        robot(i).fissure=0;
    end

    % Use this robot location minus previous robot location
    parent = t.get(t.getparent(n(node_ind)));
    dx = curnode.x_loc - parent.x_loc;
    dy = curnode.y_loc - parent.y_loc;
    dz = curnode.z_loc - parent.z_loc;

    dmagnitude = norm([dx, dy, dz]);

    % TODO: Simply make abs is WRONG!!!!!
    robot(i).prev_x_dir = abs(dx / dmagnitude);
    robot(i).prev_y_dir = abs(dy / dmagnitude);
    robot(i).prev_z_dir = abs(dz / dmagnitude);

    % Initialize the direction of x y z
    % robot(i).see_x_dir = (prev_x_direction);
    % robot(i).see_y_dir = (prev_y_direction);
    % robot(i).see_z_dir = (prev_z_direction);

    robot(i).prev_mag = dmagnitude;
    node_ind_next = t.getchildren(n(node_ind))-1;
    nchildren = numel(node_ind_next); % The number of children nodes

    % Define fissure based on the poupulation of children nodes
    if nchildren == 1
        robot(i).fissure = 0;
    else
        robot(i).fissure = 1;
    end

    for c = 1 : nchildren
        next = t.get(n(node_ind_next(c)));
        dx = nextnode.x_loc - curnode.x_loc;
        dy = nextnode.y_loc - curnode.y_loc;
        dz = nextnode.z_loc - curnode.z_loc;
        dmagnitude = norm([dx, dy, dz]);

        % Each child is associated with a next direction
        robot(i).next_x_dir(c) = abs(dx/dmagnitude); 
        robot(i).next_y_dir(c) = abs(dy/dmagnitude);
        robot(i).next_z_dir(c) = abs(dz/dmagnitude);
        robot(i).next_mag(c) = dmagnitude;

        if strcmp(SHOWIMG, 'DISPLAY') 
            % Plot the skelonton of tree
            line([curnode.x_loc, nextnode.x_loc],...
                 [curnode.y_loc, nextnode.y_loc],...
                 [curnode.z_loc, nextnode.z_loc]);
        end
    end
end

%% start to extract spherical angle 
for i = 1:numel(lparind)-1
    [robot(i).prev_th, robot(i).prev_phi,r_one] = ...
                             cart2sph(robot(i).prev_x_dir,robot(i).prev_y_dir,robot(i).prev_z_dir);
    [robot(i).next_th, robot(i).next_phi,r_one] = ...
                             cart2sph(robot(i).next_x_dir,robot(i).next_y_dir,robot(i).next_z_dir);
end

end


function [img3d, centroid] = raw_image_prep(nfile, imgpath, SHOWIMG, FRTHRESHOLD, ZERO_SIZE, SALTPEPPER_PERCENT, GAUSS_PERCENT, GAUSS_VARIANCE, NCLUSTER)
% Show the original 3D images and the save raw image data 
% Save from .tif to .mat

assert( nfile~=0, 'At least one image is needed');
for i = 1 : nfile
    curslice = imread(fullfile(imgpath, [num2str(i) '.tif']));
    % The X Y coordinate in tif is reversed according to the swc files
    A(:,:,i) = transpose(curslice); % Assign each slice to a 3D matrix
end

%This line isn just to make sure we use the whole stack 
disp(sprintf('Size of Image %d*%d*%d\n', size(A)));
A=double(padarray(A,[ZERO_SIZE,ZERO_SIZE,ZERO_SIZE])); % Add zero padding

foregroundmap = A > FRTHRESHOLD; % Binarise 
[x y z] = ind2sub(size(foregroundmap), find(foregroundmap));
foregroundidx = [x y z];

if strcmp(SHOWIMG, 'DISPLAY')
    figure
    plot3(x, y, z, 'b.');
end

% Salt and Pepper Noise
spA = noise(A,'sp', SALTPEPPER_PERCENT);
sp_foregroundmap = spA > FRTHRESHOLD;  % synthetic data
[x y z] = ind2sub(size(sp_foregroundmap), find(sp_foregroundmap));

if strcmp(SHOWIMG, 'DISPLAY')
    figure
    plot3(x, y, z, 'b.');
end

% Gaussian Noise
gsA = noise(A, 'ag', GAUSS_VARIANCE, GAUSS_PERCENT);

gs_foregroundmap = gsA > FRTHRESHOLD;  % synthetic data
[x y z] = ind2sub(size(gs_foregroundmap), find(gs_foregroundmap));

if strcmp(SHOWIMG, 'DISPLAY')
    figure
    plot3(x, y, z, 'b.');
end

% K-means clustering
% (K: number of clusters, G: assigned groups, C: cluster centers)
[assigned_groups, centroid] = kmeans(foregroundidx, NCLUSTER, 'distance','sqEuclidean', 'start','sample');

% Plot K-Means
if strcmp(SHOWIMG, 'DISPLAY')
    % show points and clusters (color-coded)
    clr = lines(NCLUSTER);
    figure, hold on
    scatter3(foregroundidx(:,1), foregroundidx(:,2), foregroundidx(:,3), 36, clr(assigned_groups,:), 'Marker','.')
    scatter3(centroid(:,1), centroid(:,2), centroid(:,3), 100, clr, 'Marker','o', 'LineWidth',3)
    hold off
    view(3), axis vis3d, box on, rotate3d on
    xlabel('x'), ylabel('y'), zlabel('z')
end

% TODO: In the future, we might try the histogram normalization. 
% Simple Normalization : ZERO-MEAN
Avec = A(:);
img3d.original = (A-min(Avec))/(max(Avec)-min(Avec));

gsAvec = gsA(:);
img3d.gauss = (gsA-min(gsAvec)) / (max(gsAvec)-min(gsAvec));

spAvec = spA(:);
img3d.salt_pepper = (spA-min(spAvec)) / (max(spAvec)-min(spAvec));

end