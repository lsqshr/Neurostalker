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
% s
for i = 1 : nseed
	sph = csvread(fullfile(filepath, 'testdata', sprintf('sampledsphere%d.csv', i-1)));
	momentlist = csvread(fullfile(filepath, 'testdata', sprintf('moment%d.csv', i-1)));
	[value index] = min(momentlist(:,3));
	seedx = repmat(seeds(i, 1), size(sph, 1), 1);
	seedy = repmat(seeds(i, 2), size(sph, 1), 1);
	seedz = repmat(seeds(i, 3), size(sph, 1), 1);
	seedx = seedx(index);
	seedy = seedy(index);
	seedz = seedz(index);
	index
	sph(index, 1)
	sph(index, 2)
	sph(index, 3)
	quiver3(seedx, seedy, seedz, sph(index, 1), sph(index, 2), sph(index, 3), 20);
end
hold off





%Show spheres
% for i = 1 : nseed 
% 	figure(i + 1)
% 	view([2^0.5, 2^0.5, 2^0.5])
% 	titlestr = sprintf('sphere %d', i - 1);
% 	title(titlestr)
% 	sph = csvread(fullfile(filepath, 'testdata', sprintf('sampledsphere%d.csv', i-1)));
% 	peak = csvread(fullfile(filepath, 'testdata', sprintf('sphpeak%d.csv', i-1)));
% 	startpts = zeros(size(peak, 1), 1);
% 	hold on
% 	plot3(sph(:,1),sph(:,2),sph(:,3), 'co');
% 	%quiver3(startpts, startpts, startpts, peak(:,1), peak(:,2), peak(:,3), 4);
% end

% Show GVF
gvf = csvread(fullfile(filepath, 'testdata', 'gvf-littlesoma.csv'));

figure(nseed + 2)
title('GVF')
%idx = [1:100:size(gvf, 1)];

%quiver3(gvf(idx, 1), gvf(idx, 2), gvf(idx, 3), gvf(idx, 4), gvf(idx, 5), gvf(idx, 6), 2)
hold on
idx = bimg(:, 1) * max(gvf(:, 2)) * max(gvf(:, 3)) + bimg(:,2) * max(gvf(:, 3)) + bimg(:, 3);
plot3(bimg(:,1), bimg(:,2), bimg(:,3), 'rh');
quiver3(gvf(bimg(:,end), 1), gvf(bimg(:,end), 2), gvf(bimg(:, end), 3), gvf(bimg(:,end), 4), gvf(bimg(:,end), 5), gvf(bimg(:,end), 6), 0.5)

