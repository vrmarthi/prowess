function[map] = summerlilac(N);
s = linspace(0.0,1.0,N);
rgb1 = [ 0.085, 0.532, 0.201 ];
rgb2 = [ 0.436, 0.308, 0.631 ];
map = diverging_map(s,rgb1,rgb2);
