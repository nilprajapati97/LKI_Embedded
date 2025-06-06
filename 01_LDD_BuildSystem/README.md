# LDD_Build_System
===========================


1. mkdir LKI
2. cd LKI
3. tar -xvf ~/Downloads/bbb-builds.tgz .
4. cd bbb-buils/OS
5. wget https://mirrors.edge.kernel.org/pub/linux/kernel/v4.x/linux-4.19.103.tar.gz
6. tar -xvf linux-4.19.103.tar.gz
7. cd linux-4.19.103/
8. cp bbb-builds/OS/Configs/config.4.19.103.default .config
9. vi Makefile +322
10. add
11.    - CROSS_COMPILE=arm-linux-gnueabihf-
      - ARCH=arm
10 Make zImage
