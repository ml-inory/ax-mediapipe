#!/usr/bin/env bash

# 下载三方库
echo "Check for 3rdparty ==>"
echo "Check ax_bsp ==>"
if [[ ! -d 3rdparty/ax_bsp ]]
then
  echo "ax_bsp not exist, download from https://github.com/AXERA-TECH/ax-samples/releases/download/v0.3/arm_axpi_r1.22.2801.zip"
  cd 3rdparty
  wget -c https://github.com/AXERA-TECH/ax-samples/releases/download/v0.3/arm_axpi_r1.22.2801.zip
  unzip arm_axpi_r1.22.2801.zip -d ax_bsp
  cd ..
  echo "download ax_bsp done"
fi

echo "Check OpenCV ==>"
if [[ ! -d 3rdparty/opencv-arm-linux ]]
then
  echo "OpenCV not exist, download from https://github.com/AXERA-TECH/ax-samples/releases/download/v0.3/arm_axpi_r1.22.2801.zip"
  cd 3rdparty
  wget -c https://github.com/AXERA-TECH/ax-samples/releases/download/v0.1/opencv-arm-linux-gnueabihf-gcc-7.5.0.zip
  unzip opencv-arm-linux-gnueabihf-gcc-7.5.0.zip
  cd ..
  echo "download OpenCV done"
fi

# 编译
mkdir -p build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/arm-linux-gnueabihf.toolchain.cmake -DCMAKE_INSTALL_PREFIX=./install ..
make install