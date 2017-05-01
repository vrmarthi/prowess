function[map] = winterbeige(N);
s = linspace(0.0,1.0,N);
rgb1 = [ 0.217, 0.525, 0.910 ];
rgb2 = [ 0.677, 0.492, 0.093 ];
map = diverging_map(s,rgb1,rgb2);
