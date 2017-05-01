#!/usr/bin/octave --persist

B = 38; %262 Phase-II
L = 3e8/326.5e6;
R = 6000; % Mpc
Rp = 11; %Mpc / MHz ;
BW = 18; %MHz. Modify accordingly

v = load('V2CORR.dat');
n = size(v);
N = n(1);

C = sqrt(N/B);
dnu = BW/C;
c = 1:C;
U = linspace(12.5, 475, B)/L;
k_ll = 2*pi*[1:C-1]/(Rp*BW);
k_pp = 2*pi*U/R;
Pk = zeros(B, C-1);
k = zeros(B, C-1);

n = 1;

for b = 1:B
for c1=1:C
for c2=1:C
V2(b,c1,c2) = v(n++);
endfor
endfor
endfor

for b = 1:B
for c1=1:C
for c2=c1+1:C
k(b, abs(c2-c1)) = sqrt(k_pp(b)^2 + k_ll(abs(c2-c1)))^2;
Pk(b, abs(c2-c1)) += V2(b,c1,c2);
endfor
endfor
endfor

surf(log10(k_ll), log10(k_pp), log10(abs(k.^3 .* Pk)));
shading interp;
