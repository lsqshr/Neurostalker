function showdirection(vision_box, threshold, th, phi, x_loc, y_loc, z_loc, zero_size, boxsize)
% This script can be used as visulisation check of ground truth and training result
% mx my mz represents the center mass of visionbox 
A = vision_box > threshold;  % synthetic data
[x y z] = ind2sub(size(A), find(A));
mx = mean(x) + x_loc + zero_size - boxsize / 2;
my = mean(y) + y_loc + zero_size - boxsize / 2;
mz = mean(z) + z_loc + zero_size - boxsize / 2;
[dx, dy, dz] = sph2cart_sq(th, phi ,1);
scale_ruler = 6;
for i = 1 : numel(dx) 
line([mx - scale_ruler * dx(i), mx + scale_ruler * dx(i)], [my - scale_ruler * dy(i),...
    my + scale_ruler * dy(i)], [mz - scale_ruler * dz(i), mz + scale_ruler * dz(i)], 'Color','k', 'LineWidth',5);
end
end