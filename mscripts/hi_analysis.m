#!/usr/bin/octave --persist

C = 32;
v = load('V2CORR.dat');
n=1;

for b = 1:6
for c1=1:32
for c2=1:32
V(b,c1,c2) = v(n++);
endfor
endfor
endfor

for c1=1:32
for c2=1:32
V2_1(c1,c2) = V(1,c1,c2);
V2_2(c1,c2) = V(2,c1,c2);
V2_3(c1,c2) = V(3,c1,c2);
V2_4(c1,c2) = V(4,c1,c2);
V2_5(c1,c2) = V(5,c1,c2);
V2_6(c1,c2) = V(6,c1,c2);
endfor
endfor

h1 = load('V2HI_1.dat');
h2 = load('V2HI_2.dat');
h3 = load('V2HI_3.dat');
h4 = load('V2HI_4.dat');
h5 = load('V2HI_5.dat');
h6 = load('V2HI_6.dat');

%HI Surfaces
h = figure;surf(h1); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h1.eps');

h=figure;surf(h2); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h2.eps');

h=figure;surf(h3); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h3.eps');

h=figure;surf(h4); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h4.eps');

h=figure;surf(h5); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h5.eps');

h=figure;surf(h6); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h6.eps');

call = sprintf("mv h*.eps HI_V2_surfaces");
system(call);


[u1 l1] = eig(V2_1);
[u2 l2] = eig(V2_2);
[u3 l3] = eig(V2_3);
[u4 l4] = eig(V2_4);
[u5 l5] = eig(V2_5);
[u6 l6] = eig(V2_6);

% Two eigenvectors
L1 = L2 = L3 = L4 = L5 = L6 = zeros(32);
for i = 31:32
L1(i,i) = 1.0;
L2(i,i) = 1.0;
L3(i,i) = 1.0;
L4(i,i) = 1.0;
L5(i,i) = 1.0;
L6(i,i) = 1.0;
endfor

%Project HI on the two eigenvectors
h=figure;surf(h1*(u1*L1*u1')); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h1.proj.2vec.eps');

h=figure;surf(h2*(u2*L2*u2')); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h2.proj.2vec.eps');

h=figure;surf(h3*(u3*L3*u3')); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h3.proj.2vec.eps');

h=figure;surf(h4*(u4*L4*u4')); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h4.proj.2vec.eps');

h=figure;surf(h5*(u5*L5*u5')); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h5.proj.2vec.eps');

h=figure;surf(h6*(u6*L6*u6')); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h6.proj.2vec.eps');

call = sprintf("mv h*proj*vec.eps HI_projected_on_two_eigenvectors");
system(call);


% Positive eigenvectors
L1 = L2 = L3 = L4 = L5 = L6 = zeros(32);
for i = 1:32
if(l1(i,i) > 0) L1(i,i) = 1.0;endif
if(l2(i,i) > 0) L2(i,i) = 1.0;endif
if(l3(i,i) > 0) L3(i,i) = 1.0;endif
if(l4(i,i) > 0) L4(i,i) = 1.0;endif
if(l5(i,i) > 0) L5(i,i) = 1.0;endif
if(l6(i,i) > 0) L6(i,i) = 1.0;endif
endfor

%Project HI on the positive eigenvectors
h=figure;surf(h1*(u1*L1*u1')); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h1.proj.pos.vec.eps');

h=figure;surf(h2*(u2*L2*u2')); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h2.proj.pos.vec.eps');

h=figure;surf(h3*(u3*L3*u3')); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h3.proj.pos.vec.eps');

h=figure;surf(h4*(u4*L4*u4')); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h4.proj.pos.vec.eps');

h=figure;surf(h5*(u5*L5*u5')); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h5.proj.pos.vec.eps');

h=figure;surf(h6*(u6*L6*u6')); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h6.proj.pos.vec.eps');

call = sprintf("mv h*proj*pos.vec.eps HI_projected_on_positively_eigenvalued_eigenvectors");
system(call);


% Negative eigenvectors
L1 = L2 = L3 = L4 = L5 = L6 = zeros(32);
for i = 1:32
if(l1(i,i) < 0) L1(i,i) = 1.0;endif
if(l2(i,i) < 0) L2(i,i) = 1.0;endif
if(l3(i,i) < 0) L3(i,i) = 1.0;endif
if(l4(i,i) < 0) L4(i,i) = 1.0;endif
if(l5(i,i) < 0) L5(i,i) = 1.0;endif
if(l6(i,i) < 0) L6(i,i) = 1.0;endif
endfor

%Project HI on the negative eigenvectors
h=figure;surf(h1*(u1*L1*u1')); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h1.proj.neg.vec.eps');

h=figure;surf(h2*(u2*L2*u2')); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h2.proj.neg.vec.eps');

h=figure;surf(h3*(u3*L3*u3')); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h3.proj.neg.vec.eps');

h=figure;surf(h4*(u4*L4*u4')); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h4.proj.neg.vec.eps');

h=figure;surf(h5*(u5*L5*u5')); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h5.proj.neg.vec.eps');

h=figure;surf(h6*(u6*L6*u6')); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'h6.proj.neg.vec.eps');

call = sprintf("mv h*proj*neg.vec.eps HI_projected_on_negatively_eigenvalued_eigenvectors");
system(call);

%Signal + foreground
v1 = V2_1 + h1;
v2 = V2_2 + h2;
v3 = V2_3 + h3;
v4 = V2_4 + h4;
v5 = V2_5 + h5;
v6 = V2_6 + h6;


[u1 l1] = eig(v1);
[u2 l2] = eig(v2);
[u3 l3] = eig(v3);
[u4 l4] = eig(v4);
[u5 l5] = eig(v5);
[u6 l6] = eig(v6);

% Two eigenvectors
L1 = L2 = L3 = L4 = L5 = L6 = zeros(32);
for i = 31:32
L1(i,i) = l1(i,i);
L2(i,i) = l2(i,i);
L3(i,i) = l3(i,i);
L4(i,i) = l4(i,i);
L5(i,i) = l5(i,i);
L6(i,i) = l6(i,i);
endfor

%Subtract the largest two eigenvectors
h=figure;surf(v1 - u1*L1*u1'); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'B1.sub.2vec.eps');

h=figure;surf(v2 - u2*L2*u2'); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'B2.sub.2vec.eps');

h=figure;surf(v3 - u3*L3*u3'); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'B3.sub.2vec.eps');

h=figure;surf(v4 - u4*L4*u4'); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'B4.sub.2vec.eps');

h=figure;surf(v5 - u5*L5*u5'); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'B5.sub.2vec.eps');

h=figure;surf(v6 - u6*L6*u6'); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'B6.sub.2vec.eps');

call = sprintf("mv B*sub*vec.eps FG_subtracted_through_two_eigenvectors");
system(call);


% Subtract only the Positive eigenvectors
L1 = L2 = L3 = L4 = L5 = L6 = zeros(32);
for i = 1:32
if(l1(i,i) > 0) L1(i,i) = l1(i,i);endif
if(l2(i,i) > 0) L2(i,i) = l2(i,i);endif
if(l3(i,i) > 0) L3(i,i) = l3(i,i);endif
if(l4(i,i) > 0) L4(i,i) = l4(i,i);endif
if(l5(i,i) > 0) L5(i,i) = l5(i,i);endif
if(l6(i,i) > 0) L6(i,i) = l6(i,i);endif
endfor

%Subtract the positive eigenvectors
h=figure;surf(v1 - u1*L1*u1'); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'B1.sub.pos.vec.eps');

h=figure;surf(v2 - u2*L2*u2'); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'B2.sub.pos.vec.eps');

h=figure;surf(v3 - u3*L3*u3'); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'B3.sub.pos.vec.eps');

h=figure;surf(v4 - u4*L4*u4'); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'B4.sub.pos.vec.eps');

h=figure;surf(v5 - u5*L5*u5'); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'B5.sub.pos.vec.eps');

h=figure;surf(v6 - u6*L6*u6'); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'B6.sub.pos.vec.eps');

call = sprintf("mv B*sub*pos.vec.eps FG_subtracted_through_positively_eigenvalued_eigenvectors");
system(call);


% Negative eigenvectors
L1 = L2 = L3 = L4 = L5 = L6 = zeros(32);
for i = 1:32
if(l1(i,i) < 0) L1(i,i) = l1(i,i);endif
if(l2(i,i) < 0) L2(i,i) = l2(i,i);endif
if(l3(i,i) < 0) L3(i,i) = l3(i,i);endif
if(l4(i,i) < 0) L4(i,i) = l4(i,i);endif
if(l5(i,i) < 0) L5(i,i) = l5(i,i);endif
if(l6(i,i) < 0) L6(i,i) = l6(i,i);endif
endfor

%Residual on the negative eigenvectors
h=figure;surf(u1*L1*u1'); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'B1.res.neg.vec.eps');

h=figure;surf(u2*L2*u2'); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'B2.res.neg.vec.eps');

h=figure;surf(u3*L3*u3'); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'B3.res.neg.vec.eps');

h=figure;surf(u4*L4*u4'); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'B4.res.neg.vec.eps');

h=figure;surf(u5*L5*u5'); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'B5.res.neg.vec.eps');

h=figure;surf(u6*L6*u6'); axis([1 32 1 32]); axis square; shading interp; colorbar; view(0,90); xlabel('Channel'); ylabel('Channel');
fn = findall(h, '-property', 'FontName');
set(fn,'FontName','/usr/share/fonts/truetype/freefont/FreeSansBold.ttf');
fs = findall(h,'-property','FontSize');
set(fs,'FontSize', 14);
print("-depsc2", "landscape", 'B6.res.neg.vec.eps');

call = sprintf("mv B*res*neg.vec.eps FG_residual_in_negatively_eigenvalued_eigenvectors");
system(call);
