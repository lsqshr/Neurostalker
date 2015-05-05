% this script can extract single robot 
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
    for i = 1:9
    prefix='OP_';
    current_folder=[prefix num2str(i)]
    zero_size=20;
    load([current_folder '_three_dim.mat']);
    load([current_folder '_salt_pepper_three_dim.mat']);
    load([current_folder '_ag_three_dim.mat']);
    cd ..; cd ..;
    cd(fullfile('raw','groundtruth'));
    % This step save ground truth into the model easy for our training
    
    savetree(three_dim, salt_pepper_three_dim, ag_three_dim, current_folder, Option, zero_size);
    cd ..; cd ..;
    cd(fullfile('preprocessed'...
         , 'preprocessed_images')); 
      end
     
%\data\input\raw\groudtruth

% addpath(genpath('C:\Users\donghao\Desktop\Sydney\new_construction'));
% cd Image_Stacks
% Number_file=length(dir('OP_1'))-3;
% cd OP_1
% raw_image_prep
% cd ..
% cd ..
% save OP_1_three_dim three_dim
% save OP_1_salt_pepper_three_dim salt_pepper_three_dim
% save OP_1_ag_three_dim ag_three_dim
% clc
% clear all
% load('OP_1_salt_pepper_three_dim.mat');
%  zero_size=20;
%  % salt_pepper_three_dim=padarray(salt_pepper_three_dim,[zero_size,zero_size,zero_size]);
% load('OP_1_ag_three_dim.mat');
%  % ag_three_dim=padarray(ag_three_dim,[zero_size,zero_size,zero_size]);
% load('OP_1_three_dim.mat');
%  % three_dim=padarray(three_dim,[zero_size,zero_size,zero_size]);
% cd ground_truth
% importdata('C:\Users\donghao\Desktop\Sydney\new_construction\ground_truth\Copy_of_OP_1.swc');
% savetree
% cd ..
% 
% %  %% Ready to train 
% [nothing total_robot]=size(robot);
% save robot_1 robot