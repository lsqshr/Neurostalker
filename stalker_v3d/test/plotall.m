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

% Clean the seeds
seeds(seeds>255 | seeds<0) = -4;
sumseeds = sum(seeds, 2);
seedidx = sumseeds > 0;

plot3(seeds(seedidx,1),seeds(seedidx,2),seeds(seedidx>0,3), 'rh');
plot3(seeds(1,1),seeds(1,2),seeds(1,3), 'r+');
plot3(seeds(2,1),seeds(2,2),seeds(2,3), 'r+');
plot3(seeds(3,1),seeds(3,2),seeds(3,3), 'r+');
plot3(seeds(4,1),seeds(4,2),seeds(4,3), 'r+');

hold off

% Show spheres
nseed = size(seeds, 1);
for i = 1 : nseed 
	figure(i + 1)
	view([2^0.5, 2^0.5, 2^0.5])
	titlestr = sprintf('sphere %d', i - 1);
	title(titlestr)
	sph = csvread(fullfile(filepath, 'testdata', sprintf('sampledsphere%d.csv', i-1)));
	peak = csvread(fullfile(filepath, 'testdata', sprintf('sphpeak%d.csv', i-1)));
	peaks = peaks;
	startpts = zeros(size(peak, 1), 1);
	hold on
	plot3(sph(:,1),sph(:,2),sph(:,3), 'co');
	quiver3(startpts, startpts, startpts, peak(:,1), peak(:,2), peak(:,3), 4);
end
