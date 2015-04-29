
clc
clear all
%close all
addpath(genpath('C:\Users\donghao\Desktop\Sydney\new_construction'));
robot_list(1)=load('robot_1.mat');
robot_list(2)=load('robot_2.mat');
robot_list(3)=load('robot_3.mat');
robot_list(4)=load('robot_4.mat');
robot_list(5)=load('robot_5.mat');
robot_list(6)=load('robot_6.mat');
robot_list(7)=load('robot_7.mat');
robot_list(8)=load('robot_8.mat');



%train all robots
tic
x_trn=[];
x_dir_vec_gt=[];
y_dir_vec_gt=[];
z_dir_vec_gt=[];
for j_1=1:7
    easy_case=[1 3 4 5 6 7 8];
    j=easy_case(j_1);
current_robot=robot_list(j);
robot=current_robot.robot;
[nothing total_robot]=size(robot);
for i=1:total_robot
    if robot(i).fissure==0
current_box_vec=robot(i).ag_vision_box;
current_box_vec=current_box_vec(:);
%current_box_vec=[current_box_vec;robot(i).prev_x_dir;robot(i).prev_y_dir;robot(i).prev_z_dir;];
x_trn=[x_trn; current_box_vec'];
x_dir_vec_gt=[x_dir_vec_gt; robot(i).next_x_dir];
y_dir_vec_gt=[y_dir_vec_gt; robot(i).next_y_dir];
z_dir_vec_gt=[z_dir_vec_gt; robot(i).next_z_dir];
    end
end
end
toc

tic
model_x_dir = regRF_train(x_trn,x_dir_vec_gt,50,70);
toc
tic
model_y_dir = regRF_train(x_trn,y_dir_vec_gt,50,70);
toc
tic
model_z_dir = regRF_train(x_trn,z_dir_vec_gt,50,70);
toc

x_trn=[];
y_trn=[];
robot_list(1)=load('robot_9.mat');
current_robot=robot_list(1);
robot=current_robot.robot;
load('OP_9_ag_three_dim.mat');
%figure
hold on

current.x_loc=76.663;
current.y_loc=324;
current.z_loc=9.749;
zero_size=20;
box_size_define=13;
for step=1:200    
ori_saved_box=savebox(ag_three_dim, box_size_define, current.x_loc,current.y_loc,current.z_loc,zero_size);
tmp_voxel=ori_saved_box(:);
cur_x_dir = regRF_predict(tmp_voxel',model_x_dir);
cur_y_dir = regRF_predict(tmp_voxel',model_y_dir);
cur_z_dir = regRF_predict(tmp_voxel',model_z_dir);
next.x_loc=current.x_loc+cur_x_dir;
next.y_loc=current.y_loc-cur_y_dir;
next.z_loc=current.z_loc+cur_z_dir;
line([current.y_loc, next.y_loc], [current.x_loc,next.x_loc], [current.z_loc, next.z_loc],'Color','b','LineWidth',5);
pause(0.1)
current.x_loc=next.x_loc;
current.y_loc=next.y_loc;
current.z_loc=next.z_loc;
end
