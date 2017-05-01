Summary: Options and Parameter parsing Tool
Name: opt
Version: 3.7
Release: 1
Source: nis-www.lanl.gov/~jt/Software/opt/opt-3.7.tar.gz
Vendor: Homegrown and Halfbaked
Packager: Nambe Institute Software
Copyright: LGPL
Group: Development/Libraries

%description
opt, v3.7, is a subroutine library for communicating options and
parameter values to a C program via the command line, parameter files, 
environment variables, or a rudimentary builtin interactive menu.  It is
similar in aims to the standard getopt() utility, but it has a different
(I would say better) interface to the programmer, and a few more bells
and whistles for the end-users of programs that employ opt.

## BuildRoot: /tmp/opt-3.7
Prefix: /usr/local

%prep
%setup
%build
./configure --prefix=/usr/local --disable-test --with-readline
make

%install
make install

%files
## %doc opt.texi 
/usr/local/lib/libopt.a
/usr/local/include/opt.h
/usr/local/info/opt.info
