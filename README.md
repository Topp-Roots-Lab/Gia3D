# Skel

Root3D Skeleton Traits calculates the traits based on skeleton by Olga Symonova (olga.symonova@gmail.com).

## How to Compile

Original guide can be found in `compile-cmd`

```bash
# Download qhull-2012.1 and compile for liqhullstatic.a
wget https://github.com/qhull/qhull/archive/2012.1.tar.gz -O - | tar -xz
cd qhull-2012.1 && make && cd ..

echo "export LD_LIBRARY_PATH=$PWD/lib:$LD_LIBRARY_PATH" >> /etc/profile.d/qhull.sh
source /etc/profile.d/qhull.sh

g++ -Wno-deprecated main.cpp menu.cpp MedialCurve.cpp RootGraph.cpp \
./cc/voxel/Voxels.cc ./cc/voxel/VoxelRef.cc  \
./cc/voxel/VoxelFilter.cc ./cc/voxel/PalagyiFilter.cc \
./cc/voxel/Template.cc ./cc/voxel/Templates.cc  \
-I./qhull-2012.1/src/libqhull -o skel ./qhull-2012.1/lib/libqhullstatic.a
```
