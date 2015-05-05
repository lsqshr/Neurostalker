function robot=extract_gt(three_dim, salt_pepper_three_dim, ag_three_dim, current_folder, Option, zero_size)
% Extract the voxel vision blocks of 1 imagestack and the ground truth
% directions
% Parameters: 
% threedim - ...
% salt_pepper_three_dim - ..

%addpath(genpath(fullfile('..', 'lib', '@tree'))); % We need to add 

swc = importdata([current_folder '.swc']);

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
[~, I] = find(lparind == -1 );
assert numel(I) == 1;

t = tree(robot_ground_truth(1)); % tree structure in ../lib/@tree
[t n(1)] = t.addnode(1, robot_ground_truth(2));
for i=2:1:numel(parind)-1
    par_n=parind(i+1);
    par_n=par_n-1;
    [t n(i)] = t.addnode(n(par_n), robot_ground_truth(i+1));
end
 if Option==1
 figure
 hold on
 for i=1:(numel(parind)-1)
 test=t.get(n(i));
 plot3(test.x_loc, test.y_loc, test.z_loc, 'r+');
 %pause(0.1)
 end
 end

df_order = tree(t, 'clear'); % Generate an empty synchronized tree
iterator = t.depthfirstiterator; % Doesn't matter whether you call this on |t| or |df_order|
% index = 1;
% for i = iterator
%     df_order = df_order.set(i, index);
%     index = index + 1;
% end
%disp(df_order.tostring)
 % disp(t.tostring)
iterator=iterator-1;

% for i=50
% node_ind_next=t.getchildren(n(i))
% end
%for i=2:numel(parind)-1

%initialize the root parameter
box_size_define=13;

% figure
% showbox(ag_three_dim,0.3)

current=t.get(1);
ori_saved_box=savebox(three_dim, box_size_define, current.x_loc,current.y_loc,current.z_loc,zero_size);
robot(1).ori_vision_box=ori_saved_box;
ori_saved_box=savebox(salt_pepper_three_dim, box_size_define, current.x_loc,current.y_loc,current.z_loc,zero_size);
robot(1).sp_vision_box=ori_saved_box;
ori_saved_box=savebox(ag_three_dim, box_size_define, current.x_loc,current.y_loc,current.z_loc,zero_size);
robot(1).ag_vision_box=ori_saved_box;
robot(1).fissure=0;

 current=t.get(n(1));
 ori_saved_box=savebox(three_dim, box_size_define, current.x_loc,current.y_loc,current.z_loc,zero_size);
 robot(2).ori_vision_box=ori_saved_box;
 ori_saved_box=savebox(salt_pepper_three_dim, box_size_define, current.x_loc,current.y_loc,current.z_loc,zero_size);
 robot(2).sp_vision_box=ori_saved_box;
 ori_saved_box=savebox(ag_three_dim, box_size_define, current.x_loc,current.y_loc,current.z_loc,zero_size);
 robot(2).ag_vision_box=ori_saved_box;
 robot(2).fissure=0;

%initilize the unit vector 
current=t.get(1);
next=t.get(n(1));
next_x=next.x_loc-current.x_loc;
next_y=next.y_loc-current.y_loc;
next_z=next.z_loc-current.z_loc;
next_magnitude=sqrt(next_x*next_x+next_y*next_y+next_z*next_z);
next_x_direction=next_x/next_magnitude;
next_y_direction=next_y/next_magnitude;
next_z_direction=next_z/next_magnitude;
robot(1).next_x_dir=next_x_direction;
robot(1).next_y_dir=next_y_direction;
robot(1).next_z_dir=next_z_direction;
robot(1).next_mag=next_magnitude;
robot(1).radius=current.radius;

robot(1).prev_x_dir=next_x_direction;
robot(1).prev_y_dir=next_y_direction;
robot(1).prev_z_dir=next_z_direction;
robot(1).prev_mag=next_magnitude;

%initialize the children parameter
 for   i=2:numel(parind)-1
  %i
node_ind=iterator(i);
%node_ind_next=iterator(i+1);
current=t.get(n(node_ind));
robot(i).radius=current.radius;
ori_saved_box=savebox(three_dim, box_size_define, current.x_loc,current.y_loc,current.z_loc,zero_size);
%first parameter done
robot(i).ori_vision_box=ori_saved_box;

ori_saved_box=savebox(salt_pepper_three_dim, box_size_define, current.x_loc,current.y_loc,current.z_loc,zero_size);
%second parameter done
robot(i).sp_vision_box=ori_saved_box;

ori_saved_box=savebox(ag_three_dim, box_size_define, current.x_loc,current.y_loc,current.z_loc,zero_size);
%third parameter done
robot(i).ag_vision_box=ori_saved_box;

%Use this robot location minus previous robot location
parent=t.get(t.getparent(n(node_ind)));
prev_x=current.x_loc-parent.x_loc;
prev_y=current.y_loc-parent.y_loc;
prev_z=current.z_loc-parent.z_loc;
prev_magnitude=sqrt(prev_x*prev_x+prev_y*prev_y+prev_z*prev_z);
prev_x_direction=prev_x/prev_magnitude;
prev_y_direction=prev_y/prev_magnitude;
prev_z_direction=prev_z/prev_magnitude;
robot(i).prev_x_dir=abs(prev_x_direction);
robot(i).prev_y_dir=abs(prev_y_direction);
robot(i).prev_z_dir=abs(prev_z_direction);
%initialize the direction of x y z
robot(i).see_x_dir=(prev_x_direction);
robot(i).see_y_dir=(prev_y_direction);
robot(i).see_z_dir=(prev_z_direction);

robot(i).prev_mag=prev_magnitude;
%this is just for bebug
%currenti=current.nodeidx;
node_ind_next=t.getchildren(n(node_ind))-1;
size_children=numel(node_ind_next);

%% Just define fissure
%% 
if size_children==1
   robot(i).fissure=0;
else
   robot(i).fissure=1;
end
if size_children==1
next=t.get(n(node_ind_next));

if Option==1
line([current.x_loc, next.x_loc], [current.y_loc,next.y_loc], [current.z_loc, next.z_loc]);
end

%start to define midrobot
robot(i).mid_robot.x_loc=(current.x_loc+next.x_loc)/2;
robot(i).mid_robot.y_loc=(current.y_loc+next.y_loc)/2;
robot(i).mid_robot.z_loc=(current.z_loc+next.z_loc)/2;

%next direction
next_x=next.x_loc-current.x_loc;
next_y=next.y_loc-current.y_loc;
next_z=next.z_loc-current.z_loc;
next_magnitude=sqrt(next_x*next_x+next_y*next_y+next_z*next_z);
next_x_direction=next_x/next_magnitude;
next_y_direction=next_y/next_magnitude;
next_z_direction=next_z/next_magnitude;
robot(i).next_x_dir=abs(next_x_direction);
robot(i).next_y_dir=abs(next_y_direction);
robot(i).next_z_dir=abs(next_z_direction);
robot(i).next_mag=next_magnitude;
robot(i).radius=current.radius;
%pause(0.3)

%mid robot
%first version of mid robot
% robot(i).mid_robot.next_x_dir=(robot(i).next_x_dir+robot(i).prev_x_dir)/2;
% robot(i).mid_robot.next_y_dir=(robot(i).next_y_dir+robot(i).prev_y_dir)/2;
% robot(i).mid_robot.next_z_dir=(robot(i).next_z_dir+robot(i).prev_z_dir)/2;
%second version of mid robot
% tmp=normr([abs(abs(next_x)+robot(i).prev_mag*robot(i).prev_x_dir)/2,abs(abs(next_y)+robot(i).prev_mag*robot(i).prev_y_dir)/2,abs(abs(next_z)+robot(i).prev_mag*robot(i).prev_x_dir)/2]); 
% robot(i).mid_robot.next_x_dir=tmp(1); 
% robot(i).mid_robot.next_y_dir=tmp(2); 
% robot(i).mid_robot.next_z_dir=tmp(3);
% robot(i).mid_robot.next_mg=norm([[abs(abs(next_x)+robot(i).prev_mag*robot(i).prev_x_dir)/2,abs(abs(next_y)+robot(i).prev_mag*robot(i).prev_y_dir)/2,abs(abs(next_z)+robot(i).prev_mag*robot(i).prev_x_dir)/2]],2);

elseif size_children==2   
val=node_ind_next(1);
next=t.get(n(val));
%this is just for debug
%next.nodeidx;
next_x=next.x_loc-current.x_loc;
next_y=next.y_loc-current.y_loc;
next_z=next.z_loc-current.z_loc;
next_magnitude=sqrt(next_x*next_x+next_y*next_y+next_z*next_z);
next_x_direction=next_x/next_magnitude;
next_y_direction=next_y/next_magnitude;
next_z_direction=next_z/next_magnitude;
robot(i).next_x_dir=abs(next_x_direction);
robot(i).next_y_dir=abs(next_y_direction);
robot(i).next_z_dir=abs(next_z_direction);
robot(i).next_mag=next_magnitude;

if Option==1
line([current.x_loc, next.x_loc], [current.y_loc,next.y_loc], [current.z_loc, next.z_loc]);
end
%pause(0.3)
val=node_ind_next(2);

next=t.get(n(val));

next_x=next.x_loc-current.x_loc;
next_y=next.y_loc-current.y_loc;
next_z=next.z_loc-current.z_loc;
next_magnitude=sqrt(next_x*next_x+next_y*next_y+next_z*next_z);
next_x_direction=next_x/next_magnitude;
next_y_direction=next_y/next_magnitude;
next_z_direction=next_z/next_magnitude;
robot(i).next_two_x_dir=next_x_direction;
robot(i).next_two_y_dir=next_y_direction;
robot(i).next_two_z_dir=next_z_direction;
robot(i).next_two_mag=next_magnitude;

%this is just for debug
%next.nodeidx;
if Option==1
line([current.x_loc, next.x_loc], [current.y_loc,next.y_loc], [current.z_loc, next.z_loc]);
end
%pause(0.3)
end
 end

 

 %% start to extract emuler angle 
 for   i=1:numel(parind)-1
    %i
 [robot(i).prev_alpha,robot(i).prev_beta,r_one]= cart2sph(robot(i).prev_x_dir,robot(i).prev_y_dir,robot(i).prev_z_dir);
 %r_one
 [robot(i).next_alpha,robot(i).next_beta,r_one]= cart2sph(robot(i).next_x_dir,robot(i).next_y_dir,robot(i).next_z_dir);
 %r_one

 end

 size(robot)
 
