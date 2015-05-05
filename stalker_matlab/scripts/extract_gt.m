function robot = extract_gt(img3d, sbjpath Option, zero_size)
% Extract the voxel vision blocks of 1 imagestack and the ground truth
% directions
% Parameters: 
% img3d - a structure of input voxels (3D image stack) of 1 single subject 
%         fields -- original, salt_pepper, gauss
%
% sbjpath - the name before .swc
% Option - 'DISPLAY'/'NODISPLAY'
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
assert numel(I) == 1;

t = tree(robot_ground_truth(1)); % Tree structure in ../lib/@tree
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
iterator=iterator-1;

% Initialize the root parameter
box_size_define = 13;
curnode = t.get(1); % t is the whole tree, t.get(1) is the root
fields = fieldnames(img3d)
for i = 1 : numel(fields)
    robot(1).visionbox.(fields(i)) = savebox(img3d.(fields(i)), box_size_define, curnode.x_loc,curnode.y_loc,curnode.z_loc, zero_size);
    robot(1).fissure=0;
end

% Move to the second node
curnode = t.get(n(1)); % n(1) is the first children
for i = 1 : numel(fields)
    robot(2).visionbox.(fields(i)) = savebox(img3d.(fields(i)), box_size_define, curnode.x_loc,curnode.y_loc,curnode.z_loc, zero_size);
    robot(2).fissure=0;
end

% Initilize the Cartisian unit vector of node 1 and node 2
% Because they are special 
curnode=t.get(1);
next=t.get(n(1));
next_x=nextnode.x_loc-curnode.x_loc;
next_y=nextnode.y_loc-curnode.y_loc;
next_z=nextnode.z_loc-curnode.z_loc;
next_magnitude=sqrt(next_x*next_x+next_y*next_y+next_z*next_z);
next_x_direction=next_x/next_magnitude;
next_y_direction=next_y/next_magnitude;
next_z_direction=next_z/next_magnitude;
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
for i = 2:numel(parind)-1
    node_ind = iterator(i);
    curnode = t.get(n(node_ind));
    
    % Radius
    robot(i).radius = curnode.radius;

    % Save visionboxes to robots from tree
    fields = fieldnames(img3d)
    for j = 1 : numel(fields)
        robot(i).visionbox.(fields(j)) = savebox(img3d.(fields(j)), box_size_define, curnode.x_loc,curnode.y_loc,curnode.z_loc,zero_size);
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

        if strcmp(Option, 'DISPLAY') 
            % Plot the skelonton of tree
            line([curnode.x_loc, nextnode.x_loc],...
                 [curnode.y_loc, nextnode.y_loc],...
                 [curnode.z_loc, nextnode.z_loc]);
        end
    end
end

%% start to extract spherical angle 
for i = 1:numel(parind)-1
    [robot(i).prev_alpha, robot(i).prev_beta,r_one] = ...
                             cart2sph(robot(i).prev_x_dir,robot(i).prev_y_dir,robot(i).prev_z_dir);
    [robot(i).next_alpha, robot(i).next_beta,r_one] = ...
                             cart2sph(robot(i).next_x_dir,robot(i).next_y_dir,robot(i).next_z_dir);
end
