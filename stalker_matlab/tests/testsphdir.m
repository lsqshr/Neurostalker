[th, phi] = picksphpoints(10000);
[x, y, z] = sph2cart(th, phi, 1);
scatter3(x, y, z);