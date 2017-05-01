#!/bin/bash

PKGROOT=$PWD
CALPATH=$PKGROOT/inc/

if [ `which icc | grep 'icc'` ]; 
    then CCOM=icc;
    echo;
    echo "Compiler found : "`which $CCOM`;
    `$CCOM -v`;
    if [ `uname -m | grep 'x86_64'` ];
    then CFLAGS="-debug";
    else CFLAGS="-debug -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64";
    fi
elif [ `which gcc | grep 'gcc'` ]; 
    then CCOM=gcc;
    echo;
    echo "Compiler found : "`which $CCOM`;
    `$CCOM -v`;
    if [ `uname -m | grep 'x86_64'` ];
    then CFLAGS="-g -Wall";
    else CFLAGS="-g -Wall -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64";
    fi
else CCOM="" ;
    echo;
    echo "No compiler found ";
    exit -1;
fi

if [ `which ifort | grep 'ifort'` ]; 
    then FCOM=ifort;
    echo;
    echo "Compiler found : "`which $FCOM`;
    `$FCOM -v`;
elif [ `which gfortran | grep 'gfortran'` ]; 
    then FCOM=gfortran;
    echo;
    echo "Compiler found : "`which $FCOM`;
    `$FCOM -v`;
elif [ `which g77 | grep 'g77'` ]; 
    then FCOM=g77;
    echo;
    echo "Compiler found : "`which $FCOM`;
    `$FCOM -v`;
else FCOM="" ;
    echo;
    echo "No compiler found ";
    exit -1;
fi

if [ `ls $PKGROOT/lib/ | grep 'libopt.a'` ];
then echo;
     echo "opt library found";
else
echo;
echo "Copying opt library and header file" ;
cp $PKGROOT/opt/lib/libopt.a $PKGROOT/lib/. ;
cp $PKGROOT/opt/include/opt.h $PKGROOT/inc/. ;
fi

if [ `ls $PKGROOT/lib/ | grep 'libcfitsio.a'` ];
   then echo;
        echo "cfitsio libraries found";
elif [ `ls $PKGROOT/lib/ | grep 'libcfitsio.dylib'` ];
   then echo;
        echo "cfitsio libraries found";
else 
echo "Making CFITSIO libraries";
cd $PKGROOT/cfitsio;
./configure --prefix=$PKGROOT;
sed s/cc/$CCOM/ < Makefile > temp.Makefile;
sed s/gfortran/$FCOM/ < temp.Makefile > Makefile;
make shared; 
make install;
rm -f $PKGROOT/lib/libcfitsio.a
cd $PKGROOT;
mv $PKGROOT/include/* $PKGROOT/inc/. ;
rmdir include ;
rm -f $PKGROOT/cfitsio/*.o
fi

if [ `echo $LD_LIBRARY_PATH | grep $PKGROOT/lib` ]; 
    then echo $LD_LIBRARY_PATH > /dev/null;
else LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PKGROOT/lib;
fi

if [ `echo $CPATH | grep $IPPROOT/inc` ]; 
    then echo $CPATH > /dev/null;
else CPATH=$CPATH:$IPPROOT/include;
fi

echo
echo $CALPATH
echo "is the full path to csource.mas."
echo "Export it as CALPATH if you are running "
echo "your program from a new terminal."
echo

gain=0.075
tsys=150.0;
export CFLAGS=$CFLAGS" -DGAIN="$gain" -DTSYS="$tsys" -DSTEP=0.3 -D_PLOT -D_NO_VERBOSE -D_WS -D_OPENMP"
export CCOM
export FCOM
export PKGROOT
export CALPATH
export LD_LIBRARY_PATH
