Copyright (c) 2011-2017 SAKAI Hiroaki.
All Rights Reserved.

This environment is to enjoy assembler of various architectures,
and confirmed on FreeBSD, CentOS, Ubuntu, and Cygwin.

cross-gccXXX  - Without library. Cannot use standard C library functions.
cross2-gccXXX - With newlib. You can use printf, but need disk space too much.

****************************************************************
* Setup building environment
****************************************************************

* Setup

  Download archive.

    ~>% wget -nd http://kozos.jp/books/asm/cross-gccXXX-XXXXXXXX.tgz
    (or cross2-gccXXX-XXXXXXXX.tgz)

  Extract building environment from archive.

    ~>% tar xvzf cross-gccXXX-XXXXXXXX.tgz

    or

    ~>% unzip cross-gccXXX-XXXXXXXX.zip

* Get archives of toolchain

    ~>% cd cross-gccXXX/toolchain
    ~/cross-gccXXX/toolchain>% ./fetch.sh
    (download archives and check MD5)

  Or get archives manually and put them in cross-gccXXX/toolchain/orig.

  If archives already exist and you run fetch.sh, only check of MD5 is done.

* Setup toolchain

    ~/cross-gccXXX/toolchain>% ./setup.sh
    (extract sources from archives and apply some patches)

* Setup build environment

  If your PC has multi-core processor, enable the following line of config.sh.

    ~/cross-gccXXX/toolchain>% cd ..
    ~/cross-gccXXX>% vi config.sh

  ----------------
  #makeopt="-j 2"
  ----------------

  Modify above line as follows to the number of cores.

  ----------------
  makeopt="-j 4"
  ----------------

****************************************************************
* Install libraries for GCC4
****************************************************************

* Check system

  Run script to check your system as follows.

    ~/cross-gccXXX>% cd build
    ~/cross-gccXXX/build>% ./config.sh

  GCC4 needs GMP, MPFR, and MPC libraries. If you cannot to
  detect them, the following operations is need to install them.

  If these libraries have been already installed in your system
  and were able to detect them by config.sh, you may use them.
  If you want to do so, following operations are not necessary.

  However, it is recommended to install these libraries independently
  from one of the system by following operations to avoid
  combination problems between them and GCC4.

  You can install them at once or by step.

* Install all at once

  To install all at once, open the directory to install toolchain.

    ~/cross-gccXXX/build>% su
    # mkdir /usr/local/cross-gccXXX (or /usr/local/cross2-gccXXX)
    # chmod 777 /usr/local/cross-gccXXX
    # exit

  Install all at once as follows.

    ~/cross-gccXXX/build>% ./setup-all.sh

* Install by step

  If you want not to open system directory to normal user,
  install them by step.

    ~/cross-gccXXX/build>% cd setup
    ~/cross-gccXXX/build/setup>% ./build-gmp.sh
    ~/cross-gccXXX/build/setup>% su
    # ./install-gmp.sh
    # exit
    ~/cross-gccXXX/build/setup>% ./build-mpfr.sh
    ~/cross-gccXXX/build/setup>% su
    # ./install-mpfr.sh
    # exit
    ~/cross-gccXXX/build/setup>% ./build-mpc.sh
    ~/cross-gccXXX/build/setup>% su
    # ./install-mpc.sh
    # exit
    ~/cross-gccXXX/build/setup>% cd ..

* Check build environment once again

  Check to detect installed libraries.

    ~/cross-gccXXX/build>% ./config.sh

  GMP, MPFR, and MPC installed directories are /usr/local/cross-gccXXX.

****************************************************************
* Install toolchain
****************************************************************

* Install all at once

  If /usr/local/cross-gccXXX is opened to normal user,
  you can build and install all at once as follows.

    ~/cross-gccXXX/build>% ./build-install-all.sh
    (need a lot of space on HDD and long time)

  If you want to save the space of HDD,
  you can build while deleting the working directories as follows.

    ~/cross-gccXXX/build>% ./build-install-clean-all.sh

  If you want to build only specific architectures, as follows.

    ~/cross-gccXXX/build>% ./build-install-all.sh arm-elf mips-elf

  If you want to build only BINUTILS and specific architectures, as follows.

    ~/cross-gccXXX/build>% cd binutils
    ~/cross-gccXXX/build/binutils>% ./all.sh -build -install arm-elf mips-elf

* Install by step

    ~/cross-gccXXX/build>% cd binutils
    ~/cross-gccXXX/build/binutils>% ./build.sh
    ~/cross-gccXXX/build/binutils>% su
    # ./install.sh
    # exit
    ~/cross-gccXXX/build/binutils>% cd ../gcc
    ~/cross-gccXXX/build/gcc>% ./build.sh
    ~/cross-gccXXX/build/gcc>% su
    # ./install.sh
    # exit
    ~/cross-gccXXX/build/gcc>% cd ../gdb
    ~/cross-gccXXX/build/gdb>% ./build.sh
    ~/cross-gccXXX/build/gdb>% su
    # ./install.sh
    # exit

  If you want to build or install only specific architectures,
  you can use script as follows.

    ~/cross-gccXXX/build>% cd binutils
    ~/cross-gccXXX/build/binutils>% ./build.sh arm-elf mips-elf

    or

    ~/cross-gccXXX/build/binutils>% su
    # ./install.sh arm-elf mips-elf

****************************************************************
* Output assemblers
****************************************************************

* Output assemblers.

    ~>% cd cross-gccXXX/sample (or cross2-gccXXX/sample)
    ~/cross-gccXXX/sample>% make clean
    ~/cross-gccXXX/sample>% make

  See assemblers as follows.

    ~/cross-gccXXX/sample>% ls *.d
    ~/cross-gccXXX/sample>% more mips-elf.d

  If you want to output only specific architectures, as follows.

    ~/cross-gccXXX/sample>% make mips-elf.d

* Execute sample program.

    ~/cross-gccXXX/sample>% cd ../exec
    ~/cross-gccXXX/exec>% make clean
    ~/cross-gccXXX/exec>% make
    ~/cross-gccXXX/exec>% make run
    ~/cross-gccXXX/exec>% cat *.not *.sot

  If you want to execute only specific architectures, as follows.

    ~/cross-gccXXX/exec>% make mips-elf.sot

* Execute sample program with newlib. (cross2-gccXXX only)

    ~/cross-gccXXX/exec>% cd ../printf
    ~/cross-gccXXX/printf>% make clean
    ~/cross-gccXXX/printf>% make
    ~/cross-gccXXX/printf>% make run
    ~/cross-gccXXX/printf>% cat *.not *.sot

****************************************************************
* Clean working directories
****************************************************************

* Clean installed toolchain

    ~>% su
    # cd /usr/local
    # rm -fR cross-gccXXX (or cross2-gccXXX)

* Clean source directory

    ~>% cd cross-gccXXX/toolchain
    ~/cross-gccXXX/toolchain>% ./clean.sh
    ~/cross-gccXXX/toolchain>% cd orig
    ~/cross-gccXXX/toolchain/orig>% rm -f *

* Clean build directory

    ~>% cd cross-gccXXX/build
    ~/cross-gccXXX/build>% ./clean-all.sh

  If you want to delete only specific architectures, as follows.

    ~/cross-gccXXX/build>% ./clean-all.sh arm-elf mips-elf

  If you want to delete only BINUTILS and specific architectures, as follows.

    ~/cross-gccXXX/build>% cd binutils
    ~/cross-gccXXX/build/binutils>% ./clean.sh arm-elf mips-elf

  If you want to delete only BINUTILS, as follows.

    ~/cross-gccXXX/build/binutils>% ./clean.sh

Let's enjoy assembler!
