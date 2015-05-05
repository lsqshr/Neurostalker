function [img3d, centroid] = raw_image_prep(nfile, imgpath, SHOWIMG, FRTHRESHOLD, ZEROSIZE, SALTPEPPER_PERCENT, GAUSS_PERCENT, GAUSS_VARIANCE, NCLUSTER)
% Show the original 3D images and the save raw image data 
% Save from .tif to .mat

for i = 1 : nfile
	curslice = imread([num2str(i) '.tif']);
	% The X Y coordinate in tif is reversed according to the swc files
	A(:,:,i) = transpose(curslice); % Assign each slice to a 3D matrix
end

%This line isn just to make sure we use the whole stack 
disp(sprintf('Size of Image %d*%d*%d\n', size(A)));
A=double(padarray(A,[ZERO_SIZE,ZERO_SIZE,ZERO_SIZE])); % Add zero padding

foregroundmap = A > FRTHRESHOLD; % Binarise 
foregroundidx = [x y z] = ind2sub(size(foregroundmap), find(foregroundmap));

if strcmp(Option, 'DISPLAY')
	figure
	plot3(x, y, z, 'b.');
end

% Salt and Pepper Noise
sp_foregroundmap = noise(A,'sp', SALTPEPER_PERCENT);
sp_foregroundmap = spA > FRTHRESHOLD;  % synthetic data
[x y z] = ind2sub(size(sp_foregroundmap), find(sp_foregroundmap));

if strcmp(Option, 'DISPLAY')
	figure
	plot3(x, y, z, 'b.');
end

% Gaussian Noise
gsA = noise(A, 'ag', GAUSS_VARIANCE, GAUSS_PERCENT);

gs_foregroundmap = gsA > FRTHRESHOLD;  % synthetic data
[x y z] = ind2sub(size(gs_foregroundmap), find(gs_foregroundmap));

if strcmp(Option, 'DISPLAY')
	figure
	plot3(x, y, z, 'b.');
end

% K-means clustering
% (K: number of clusters, G: assigned groups, C: cluster centers)
[assigned_groups, centroid] = kmeans(foregroundidx, NCLUSTER, 'distance','sqEuclidean', 'start','sample');

% Plot K-Means
if strcmp(Option, 'DISPLAY')
	%# show points and clusters (color-coded)
	clr = lines(NCLUSTER);
	figure, hold on
	scatter3(foregroundidx(:,1), foregroundidx(:,2), foregroundidx(:,3), 36, clr(assigned_groups,:), 'Marker','.')
	scatter3(centroid(:,1), centroid(:,2), centroid(:,3), 100, clr, 'Marker','o', 'LineWidth',3)
	hold off
	view(3), axis vis3d, box on, rotate3d on
	xlabel('x'), ylabel('y'), zlabel('z')
end

% TODO: In the future, we might try the histogram normalization. 
% Simple Normalization : ZERO-MEAN
Avec = A(:);
img3d.original = (A-min(Avec))/(max(Avec)-min(Avec));

gsAvec = gsA(:);
img3d.gauss = (gsA-min(gsAvec)) / (max(gsAvec)-min(gsAvec));

spAvec = spA(:);
img3d.salt_pepper = (spA-min(spAvec)) / (max(spAvec)-min(spAvec));

end