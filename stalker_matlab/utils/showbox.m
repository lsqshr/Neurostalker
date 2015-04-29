function showbox(vision_box,threshold)

A = vision_box > threshold;  % synthetic data
[x y z] = ind2sub(size(A), find(A));
figure
plot3(x, y, z, 'b.');
end