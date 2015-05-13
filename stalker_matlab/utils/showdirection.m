function showdirection(vision_box, threshold, th, phi, x_loc, y_loc, z_loc, zerosize)
% This script can be used as visulisation check of ground truth and training result 
A = vision_box > threshold;  % synthetic data
[x y z] = ind2sub(size(A), find(A));
mx = mean(x) + x_loc - zerosize; my = mean(y) + y_loc - zerosize;
mz = mean(z) + z_loc - zerosize;
% plot3((x + x_loc), (y + y_loc), (z + z_loc), 'b.');
[dx, dy, dz] = sph2cart_sq(th, phi ,1);
scale_ruler = 6;
line([mx - scale_ruler * dx, mx + scale_ruler * dx], [my - scale_ruler * dy,...
    my + scale_ruler * dy], [mz - scale_ruler * dz, mz + scale_ruler * dz], 'Color','k', 'LineWidth',5);
% line([mx - scale_ruler * dxg, mx + scale_ruler * dxg], [my - scale_ruler * dyg,...
%     my + scale_ruler * dyg], [mz - scale_ruler * dzg, mz + scale_ruler * dzg], 'Color','r', 'LineWidth',5);
end