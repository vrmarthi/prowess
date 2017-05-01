#!/usr/bin/octave --persist
nmaps = 100;
bins = 30;
N = 2048;
ki = kj = N/2 + 1;
ii = 1;

theta = 1.2783e-4;
theta = 3.2000e-4;
fov = (N*theta)^2;
l = 1e0;
p = poissrnd(l, N, N);

del_mod_k = zeros(N,N);

del_x = (p-l)/l;
del_k = (1/N^2)*fftshift(fft2(del_x));
DEL_K = zeros(1,2*N);
count = zeros(1,2*N);

for i = 1:N
for j = 1:N
k = round(sqrt((i-ki)^2 + (j-kj)^2));
if ( k == 0 ) k = 1;
endif
kk(ii++) = k;
count(k)++;
DEL_K(k) += (abs(del_k(i, j)).^2) ;
endfor
endfor

kmax = max(kk);

DEL_K = DEL_K(1:kmax)./count(1:kmax);
xaxis = 1/(2*theta)*[0:1/(kmax-1):1];
%loglog(xaxis,DEL_K, 'b.-'); hold on; 
P_p = 1697.0*ones(1,kmax);
P_c = 446.24 * [1:kmax].^-0.9;
%P_cl = 1e-4*[1:kmax].^-0.9;
modfn = sqrt(P_c+P_p)./sqrt(P_p);
%modfn = sqrt(P_cl .+ DEL_K)./sqrt(DEL_K);

for i = 1:N
for j = 1:N
k = round(sqrt((i-ki)^2 + (j-kj)^2));
if ( k == 0 ) k = 1;
endif
del_mod_k(i,j) = del_k(i,j)*modfn(k);
endfor
endfor

MOD_DEL_K = zeros(1,kmax);

for i = 1:N
for j = 1:N
k = round(sqrt((i-ki)^2 + (j-kj)^2));
if ( k == 0 ) k = 1;
endif
MOD_DEL_K(k) += (abs(del_mod_k(i,j)).^2)/count(k) ;
endfor
endfor

%loglog(xaxis,MOD_DEL_K, 'r.-');
%loglog(xaxis, P_cl, 'g.-');

mod_del_x = N^2 * ifft2(ifftshift(del_mod_k));

pmod = l*(1+mod_del_x);

diff = p - real(pmod);
DIFF = (1/N^2) * fftshift(fft2(diff));
DIFF_K = zeros(1,kmax);
for i = 1:N
for j = 1:N
k = round(sqrt((i-ki)^2 + (j-kj)^2));
if ( k == 0 ) k = 1;
endif
DIFF_K(k) += (abs(DIFF(i,j)).^2)/count(k) ;
endfor
endfor

%figure;
%loglog(xaxis, DIFF_K, '.-');

function Y = f(X)
Y = 10.^(-0.084476.*X.^3 - 0.374.*X.^2 - 2.17.*X + 3.35);
endfunction

S = logspace(-5, 1, 1000);
for i = 1:1000 
NS(i) = quad("f", log10(S(i)), 7);
endfor

beam = theta*theta;

nsrc = beam * NS;

%idx = zeros(1,5);
j = 1;
for i = 1:1000
if(abs(nsrc(i)-1) < 0.01) idx(j++) = i;
endif
endfor

Smin = S(idx(1)); % Jy
Smax = 50;     % Jy

S_bincentre = logspace(log10(Smin), log10(Smax), bins + 1);

nsrc = zeros(1,bins);
for i = 1:bins
%nsrc(i) = f( ( log10( 0.5 * ( S_bincentre(i) + S_bincentre(i+1) ) ) ) ) * (S_bincentre(i+1) - S_bincentre(i) );
nsrc(i) = quad("f", log10(S_bincentre(i)), log10(S_bincentre(i+1) ) );
S_low(i) = S_bincentre(i);
S_high(i) = S_bincentre(i+1);
deltaS(i) = S_high(i) - S_low(i);
endfor

srcnt = [S_low' S_high' deltaS' nsrc'];

%for i = 1:bins
%norm_nsrc(i) = sum(nsrc(1:i))/sum(nsrc);
%endfor


for i = 1:N
for j = 1:N
if(real(pmod(i,j)) < 0) pmod(i,j) = 0; 
endif
pmodvector((i-1)*N+j) = real(pmod(i,j));
pvector((i-1)*N+j) = p(i,j);
endfor
endfor

%[ss ii] = sort(pmodvector);

%figure;surf(real(pmod)); axis square; colorbar; view(90,90); shading flat; axis([1 N 1 N min(min(real(pmod))) max(max(real(pmod)))])

pmod = real(pmod);

for i =1:N
for j = 1:N
ppp((i-1)*N+j) = pmod(i,j);
endfor
endfor

ppp = ppp';

save -ascii pmod.dat ppp;
