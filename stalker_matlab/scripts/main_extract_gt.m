% this script can extract multiple robot(voxel box and spherical angles. prepare for the training) 
clear all
clc
close all
warning off;

%Add the script folder into path
addpath(genpath(pwd));
% Try to find library and add lib into our path 
addpath(genpath(fullfile('..','lib')));
% Try to find utility function and add utility function into our path 
addpath(genpath(fullfile('..', 'utils'))); cd ..;
% Try to enter the raw image ground truth path
cd(fullfile('data', 'input', 'raw', 'rawimage'...
         , 'op_raw_imagestack')); 
     
% It will iteratively load the ground truth
Option = input('Do you want show original 3D images? \nOption 1: Show  Option 2: Do not show:');
for i =1:9
    prefix='OP_';
    current_folder=[prefix num2str(i)];
    Number_file=length(dir(current_folder))-3;
    cd(current_folder);
    raw_image_prep
    cd ..;cd ..;cd ..;cd ..;
    % Enter the preprocessed folder
    cd(fullfile('preprocessed'...
         , 'preprocessed_images')); 
    
    save( ([current_folder '_three_dim']), 'three_dim');
    save( ([current_folder '_salt_pepper_three_dim']), 'salt_pepper_three_dim');
    save( ([current_folder '_ag_pepper_three_dim']), 'ag_three_dim');

     
    % Back to the iamge stack folder 
     cd ..;cd ..;
     cd(fullfile('raw', 'rawimage'...
         , 'op_raw_imagestack')); 
     clc
     clearvars -except Option
end

cd ..;cd ..;cd ..;
    % Enter the preprocessed folder
    cd(fullfile('preprocessed'...
         , 'preprocessed_images')); 
Option = input('Do you want show original ground truth? \nOption 1: Show  Option 2: Do not show:');
zero_size = 20;
training_robot_list=[];
test_robot=[];
    for i = 1:9
    prefix='OP_';
    current_folder=[prefix num2str(i)]
    zero_size=20;
    load([current_folder '_three_dim.mat']);
    % size(three_dim) Make sure I load different raw voxels.
    load([current_folder '_salt_pepper_three_dim.mat']);
    load([current_folder '_ag_three_dim.mat']);
    cd ..; cd ..;
    cd(fullfile('raw','groundtruth'));
    % This step save ground truth into the model easy for our training
    
    robot=extract_gt(three_dim, salt_pepper_three_dim, ag_three_dim, current_folder, Option, zero_size);
    if (i<9)
    training_robot_list{i} = robot;
    else
    test_robot = robot;    
    end
    cd ..; cd ..;
    cd(fullfile('preprocessed'...
         , 'preprocessed_images')); 
      end
cd ..; cd(fullfile('groundtruth'...
         , 'normal')); 
     
     % Extract the groundtruth for training 
     save('training_robot_list','training_robot_list')
     save('test_robot','test_robot')
