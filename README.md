# dicom-tools
Tools to parse large DICOM database to formats suitable for deep learning.


## Installation
In Ubuntu 18.04 you need to have CMake 3.15, which is not in the apt repository. Additionally you need:
```shell script
sudo apt-get update
sudo apt-get install -y libdcmtk-dev libsqlite3-dev libboost-filesystem1.65-dev libboost-program-options1.65-dev
```

To build use
```shell script
mkdir build && cd build
ccmake ..
make
```