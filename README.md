# Skel

Root3D Skeleton Traits calculates the traits based on skeleton by Olga Symonova (olga.symonova@gmail.com).

## How to Compile
Original guide can be found in `compile-cmd`

```bash
g++ main.cpp menu.cpp MedialCurve.cpp RootGraph.cpp util.cpp ./cc/voxel/Voxels.cc ./cc/voxel/VoxelRef.cc ./cc/voxel/VoxelFilter.cc ./cc/voxel/PalagyiFilter.cc ./cc/voxel/Template.cc ./cc/voxel/Templates.cc -I./qhull-2012.1/src/libqhull -o skel ./qhull-2012.1/lib/libqhullstatic.a
```