function lstrip = visualize_sae3d(W, neuidx, mm, s1, s2, s3, slicedir)
% Visualse the first layer of SAE to 3D little blocks
% PARA --- 
% W - the weight matrix with size INPUT*#HIDDEN-NEURON
% mm - [ min max ]
% s1, s2, s3 - size of each dimension of the block
% slicedir - the direction to slice the visualise cube [1,2,3] -- NOT IMPLEMENTED YET 
%
% RETURN ---
% It plots/returns a matrix with rows on flattened cube pattern 

% Validate inputs

if ~exist('mm','var')
    mm = [min(W(:)) max(W(:))];
end

% Remove the bias
W(end, :) = [];

if ~exist('s1','var') || ~exist('s2','var') || ~exist('s2','var')
	s = floor(nthroot(size(W, 1), 3));
	%assert(s == floor(s), 'Dim not matched: nthroot %f; floor(): %f', s, floor(s)) % Means the input is a cube
    s1 = s;
    s2 = s;
    s3 = s;
end

X = zeros(size(W));

norm = sqrt(sum(W .^ 2, 2));
X = W ./ repmat(norm, 1, size(W, 2));
fprintf('Trying to reshape (%d, %d) to (%d, %d)\n', size(X', 1), size(X', 2), s1, s2 * s3 * size(X, 2));
stripX = reshape(X', s1, s2 * s3 * size(X, 2)); % make it a long strip of squares with squaresize*#HNEURON

disp('size stripX')
disp(size(stripX))

for i = 1 : size(W, 2)
	row = stripX(:, (i-1) * s2 * s3 +1 : i * s2 * s3);
	row = row + abs(min(row(:)));
	lstrip{i} = row / max(row(:));
end

strip = r{neuidx};
viscube = reshape(strip, VBSIZE, VBSIZE, VBSIZE);
fprintf('IN INPUT - MAX: %f, MIN: %f\n', max(train_x(:)), min(train_y(:)));
fprintf('IN VIS - MAX: %f, MIN: %f\n', max(viscube(:)), min(viscube(:)));
normcube = viscube+abs(min(viscube(:)));
normcube = 1 - (normcube / max(normcube(:)));
VolumeRender(normcube);

X = zeros(size(sae.ae{1}.W{1}, 1) * VBSIZE, VBSIZE * VBSIZE);
for i = 1 : numel(r)
    %fprintf('Reading %dth row of vis matrix X...\n', i);
    X(1+(i-1)*VBSIZE:i*VBSIZE, :) = r{i};
end

figure(3)
imagesc(X(25:200, :));
colormap gray; 

end