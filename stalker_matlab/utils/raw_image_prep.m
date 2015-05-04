% this file is used to show the original 3D images and the save raw image data 
for i = 1:Number_file
string_i=num2str(i);
fullname=[string_i '.tif'];
current_pic=imread(fullname);
current_pic=transpose(current_pic);
A(:,:,i)=current_pic;
%imshow(current_pic);
end
%This  line isn just to make sure we use the whole stack 
% size(A) 
threshold =15;
three_dim = A;
zero_size = 20;
three_dim=double(padarray(three_dim,[zero_size,zero_size,zero_size]));

A = A > threshold;  % synthetic data
[x y z] = ind2sub(size(A), find(A));
figure
plot3(x, y, z, 'b.');

incidence_sp=0.0001;
salt_pepper_three_dim = noise(three_dim,'sp', incidence_sp);
A = salt_pepper_three_dim > threshold;  % synthetic data
[x y z] = ind2sub(size(A), find(A));
figure
plot3(x, y, z, 'b.');


incidence_ag=0.1;
variance=1;
ag_three_dim = noise(three_dim,'ag', variance, incidence_ag);

A = ag_three_dim > threshold;  % synthetic data
[x y z] = ind2sub(size(A), find(A));
figure
plot3(x, y, z, 'b.');

%# K-means clustering
%# (K: number of clusters, G: assigned groups, C: cluster centers)
three_dim_coordinate=[x,y,z];
cluster_num = 20;
[assigned_groups,centroid] = kmeans(three_dim_coordinate, cluster_num, 'distance','sqEuclidean', 'start','sample');

%# show points and clusters (color-coded)
clr = lines(cluster_num);
figure, hold on
scatter3(three_dim_coordinate(:,1), three_dim_coordinate(:,2), three_dim_coordinate(:,3), 36, clr(assigned_groups,:), 'Marker','.')
scatter3(centroid(:,1), centroid(:,2), centroid(:,3), 100, clr, 'Marker','o', 'LineWidth',3)
hold off
view(3), axis vis3d, box on, rotate3d on
xlabel('x'), ylabel('y'), zlabel('z')

%In the future, we might try the histogram normalization. 
three_dim_vec=three_dim(:);
% max(three_dim_vec)
% min(three_dim_vec)
three_dim=(three_dim-min(three_dim_vec))/(max(three_dim_vec)-min(three_dim_vec));

ag_three_dim_vec=ag_three_dim(:);
ag_three_dim=(ag_three_dim-min(ag_three_dim_vec))/(max(ag_three_dim_vec)-min(ag_three_dim_vec));

salt_pepper_three_dim_vec=salt_pepper_three_dim(:);
salt_pepper_three_dim=(salt_pepper_three_dim-min(salt_pepper_three_dim_vec))/(max(salt_pepper_three_dim_vec)-min(salt_pepper_three_dim_vec));

%Just for test the effect of normalization 
% A=three_dim;
% threshold=0.1;
% A = A > threshold;  % synthetic data
% [x y z] = ind2sub(size(A), find(A));
% figure
% plot3(x, y, z, 'b.');

% showbox(three_dim,4)