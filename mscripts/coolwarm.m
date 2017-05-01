function[map] = coolwarm(N);
s = linspace(0.0,1.0,N);
rgb1 = [ 0.230, 0.299, 0.754 ];
rgb2 = [ 0.706, 0.016, 0.150 ];
map = diverging_map(s,rgb1,rgb2);
