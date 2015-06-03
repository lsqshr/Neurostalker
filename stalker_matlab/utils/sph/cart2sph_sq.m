function [th, phi, r] = cart2sph_sq(x, y, z)
% Convert cartisian coordinates to spherical coordinates
% Using the mathmatician definition of the S^2
%                                2*pi > th > 0
%                                pi > phi > 0
% Ref: http://en.wikipedia.org/wiki/List_of_common_coordinate_transformations

th = atan(abs(y ./ x));
% Deal with the x == 0 cases
th(intersect(x == 0, y > 0)) = pi/2;
th(intersect(x == 0, y < 0)) = 3 * pi/2;
th(intersect(x == 0, y == 0)) = 0;

r = sqrt(x.^2 + y.^2 + z.^2);
phi = acos(z ./ r);

% QII
th(intersect(x < 0, y > 0)) = pi - th(intersect(x < 0, y > 0));
% QIII
th(intersect(x < 0, y < 0)) = th(intersect(x < 0, y < 0)) + pi;
% QIV
th(intersect(x > 0, y < 0)) = 2 * pi - th(intersect(x > 0, y < 0));

end