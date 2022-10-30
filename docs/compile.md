# 快速编译

## 安装编译器
```
$ wget -c http://releases.linaro.org/components/toolchain/binaries/7.5-2019.12/arm-linux-gnueabihf/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar.xz
$ tar -xvf gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf.tar.xz

# 临时使用
$ export PATH=$PATH:$PWD/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin/

# 放到PATH
$ echo "export PATH=$PATH:$PWD/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf/bin/" >> ~/.bashrc
$ source ~/.bashrc
```

## 下载BSP
```
$ cd 3rdparty
$ wget -c https://github.com/AXERA-TECH/ax-samples/releases/download/v0.3/arm_axpi_r1.22.2801.zip
$ unzip arm_axpi_r1.22.2801.zip -d ax_bsp
```

## 下载OpenCV库
```
$ cd 3rdparty
$ wget -c https://github.com/AXERA-TECH/ax-samples/releases/download/v0.1/opencv-arm-linux-gnueabihf-gcc-7.5.0.zip
$ unzip opencv-arm-linux-gnueabihf-gcc-7.5.0.zip
```

## 编译
在工程根目录下
```
$ ./build.sh
```