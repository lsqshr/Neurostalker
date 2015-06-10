filepath = fileparts(mfilename('fullpath'));

% Plot Seeds
hold on
figure(1)
seeds = csvread(fullfile(filepath, 'testdata', 'seeds.csv'));
seeds(isnan(seeds)) = 0 ;
bimg = csvread(fullfile(filepath, 'testdata', 'binaryimg.csv'));
plot3(bimg(:,1), bimg(:,2), bimg(:,3), 'b.');

% Clean the seeds
seeds(seeds>255 | seeds<0) = -1;
sumseeds = sum(seeds, 2);
seedidx = sumseeds > 0;

plot3(seeds(seedidx,1),seeds(seedidx,2),seeds(seedidx>0,3), 'rh');
hold off