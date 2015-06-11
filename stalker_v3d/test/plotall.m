close all
filepath = fileparts(mfilename('fullpath'));

% Plot Seeds
hold on
figure(1)
title('Checking Seeds')
seeds = csvread(fullfile(filepath, 'testdata', 'seeds.csv'));
seeds(isnan(seeds)) = 0 ;
bimg = csvread(fullfile(filepath, 'testdata', 'binaryimg.csv'));
plot3(bimg(:,1), bimg(:,2), bimg(:,3), 'b.');
plot3(seeds(:,1), seeds(:,2), seeds(:,3), 'r+');

nseed = size(seeds, 1);
for i = 1: nseed
	sph = csvread(fullfile(filepath, 'testdata', sprintf('sampledsphere%d.csv', i-1)));
	peak = csvread(fullfile(filepath, 'testdata', sprintf('sphpeak%d.csv', i-1)));
	seedx = repmat(seeds(i, 1), size(peak, 1), 1);
	seedy = repmat(seeds(i, 2), size(peak, 1), 1);
	seedz = repmat(seeds(i, 3), size(peak, 1), 1);
	quiver3(seedx, seedy, seedz, peak(:, 1), peak(:, 2), peak(:, 3), 4);
end

hold off

% Show spheres
for i = 1 : nseed 
	figure(i + 1)
	view([2^0.5, 2^0.5, 2^0.5])
	titlestr = sprintf('sphere %d', i - 1);
	title(titlestr)
	sph = csvread(fullfile(filepath, 'testdata', sprintf('sampledsphere%d.csv', i-1)));
	peak = csvread(fullfile(filepath, 'testdata', sprintf('sphpeak%d.csv', i-1)));
	startpts = zeros(size(peak, 1), 1);
	hold on
	plot3(sph(:,1),sph(:,2),sph(:,3), 'co');
	quiver3(startpts, startpts, startpts, peak(:,1), peak(:,2), peak(:,3), 4);
end
