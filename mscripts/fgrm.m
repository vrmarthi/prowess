#!/usr/bin/octave --persist

%function ch = cubehelix(nlev,start,rots,hue,gamma)
%s = start;
%r = rots;
%h = hue;
%g = gamma;

s = 2.5;
nlev = 256;
r = 1.0;
h = 0.75;
g = 0.4;
l = 1;
for i = 1:nlev
L = l*i/nlev;
a = h*(L^g)*(1 - L^g)/2;
phi = 2*pi*(s/3 + r*L);
ch(i,1) = L^g +  a*( -0.14861*cos(phi) + 1.78277*sin(phi) );
ch(i,2) = L^g +  a*( -0.29277*cos(phi) - 0.90649*sin(phi) );
ch(i,3) = L^g +  a*( 1.97294*cos(phi) + 0.0*sin(phi) );
endfor

B = 38;
C = 312;

LL = zeros(B,C);

v = load('V2CORR.dat');

n = 1;
nn = 1;
for b = 1:B
for c1 = 1:C
for c2 = 1:C
V2(b,c1,c2) = v(n++);
endfor
endfor
endfor

for b = 1:B

for c1 = 1:C
for c2 = 1:C
v2(c1,c2) = V2(b,c1,c2);
endfor
endfor

hifile = sprintf("V2HI_%s.dat", num2str(b));
vhi = load(hifile);

v2 += vhi;
[v l] = eig(v2);
LL(b,:) = diag(l);


%figure;
%semilogy(abs(diag(l)),'.');
%disp("Click on figure to continue:");
%waitforbuttonpress();
%N = input("Principal values=")
%close

N = 2;

L = zeros(C);
for i = C-N+1:C
L(i,i) = l(i,i);
endfor

h = figure;
ttl = sprintf("Baseline %s", num2str(b));
title(ttl);
%
subplot(2,2,1)
colormap(ch);
surf(v2);axis([1 32 1 32]);axis square; grid off; shading interp; colorbar;view(0,90); grid off;
title('(a) Estimator : V_2')
xlabel('\nu_1'); ylabel('\nu_2');
%
hh = subplot(2,2,2);
set(hh, 'position',[0.5342,0.5825,0.342,0.342]);
%surf(v);axis([1 32 1 32]);axis square; grid off; shading interp; colorbar;view(0,90)
plot(v(:,C-N+1:C), 'LineWidth', 4); axis([1 32]); axis square; grid on; 
title('(b) Eigenvectors : v')
xlabel('\nu'); ylabel('v');
%
subplot(2,2,3)
colormap(ch);
surf(v*L*transpose(v));axis([1 32 1 32]);axis square; grid off; shading interp; colorbar;view(0,90);  grid off;
title('(c) KLT : vLv^+')
xlabel('\nu_1'); ylabel('\nu_2');
%
subplot(2,2,4)
colormap(ch);
surf(v2 - v*L*transpose(v));axis([1 32 1 32]);axis square; grid off; shading interp; colorbar;view(0,90);  grid off;
title('(d) Residual : V_2 - vLv^+')
xlabel('\nu_1'); ylabel('\nu_2');

fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 8);
%
name = sprintf("Baseline_%s_residual.eps", num2str(b));
print("-depsc2", "landscape", name);

res = v2 - v*L*transpose(v);

for c1 = 1:C
for c2 = 1:C
residual(nn++) = res(c1,c2);
endfor
endfor

endfor

residual = transpose(residual);

save -ascii residuals.dat residual

%quit
