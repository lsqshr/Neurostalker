% this script can extract single robot 
clear all
clc
close all
warning off;

% Try to find library and add lib into our path 
cd ..; cd lib; addpath(genpath(pwd)); cd ..;
% Try to find utility function and add utility function into our path 
cd utils; addpath(genpath(pwd)); cd ..;
% Try to enter the raw image ground truth path
cd(fullfile('data', 'input', 'raw', 'rawimage'...
         , 'op_raw_imagestack')); 
     
% It will iteratively load the ground truth

for i =1:9
    prefix='OP_';
current_folder=[prefix num2str(i)];
    Number_file=length(dir(current_folder))-3;
    cd(current_folder);
    raw_image_prep
    cd ..
    clc
    clear all
end

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