% this script can extract multiple robot(voxel box and spherical angles. prepare for the training) 
clear all; clc; close all; warning off;

% PARA
ZERO_SIZE = 20;
TESTSIZE = 1;
FRTHRESHOLD = 15;
SALTPEPPER_PERCENT = 0.0001;
GAUSS_PERCENT = 0.1;
GAUSS_VARIANCE = 1;
NCLUSTER = 20;
%

%Add the script folder into path
addpath(genpath(pwd));

% Add dependencies 
addpath(genpath(fullfile('..','lib')));
addpath(genpath(fullfile('..', 'utils')));

% Try to enter the raw image ground truth path
gtpath = fullfile('..', 'data', 'input', 'raw', 'groundtruth'); 
oppath = fullfile('..', 'data', 'input', 'raw', 'rawimage'); 
     
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

lrobot = {}
for i = 1 : length(dir([gtpath, [filesep '*.swc']])) % iterate each subject
    disp(i)
    sbjid = [prefix num2str(i)];
    sbjpath = fullfile(oppath, sbjid);
    nfile = length(dir([sbjpath, '*.tif']));

    % TODO: changed interface, make sure it can run
    img3d = raw_image_prep(nfile, sbjpath, SHOWIMG, FRTHRESHOLD, ZERO_SIZE, SALTPEPPER_PERCENT, GAUSS_PERCENT, GAUSS_VARIANCE, NCLUSTER);

    % Enter the preprocessed folder
    preppath = fullfile('preprocessed', 'preprocessed_images'); 

    % Extract directions and radius
    lrobot{i} = extract_gt(img3d, sbjpath, SHOWGT, ZERO_SIZE);
end

ltrainrobot = lrobot(1 : (numel(lrobot) - TESTSIZE));
ltestrobot = lrobot((numel(lrobot)-TESTSIZE+1):end);
