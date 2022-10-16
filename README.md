# Skel

Root3D Skeleton Traits calculates the traits based on skeleton by Olga Symonova (olga.symonova@gmail.com).

## Usage

```bash
# skel [fileinput] [fileoutput] [scale]
skel /path/to/point/cloud.out /path/to/features.tsv 2.25
```

### Input & Output

#### Input: point cloud data

```txt
0.15
             1904987
354 359 613
355 359 613
356 359 613
357 359 613
347 360 613
348 360 613
...
```

#### Output

|File Format|Description|Purpose|
|-|-|-|
or.iv|||
or.wrl|Object Mesh|Quality Control|
sk.iv|||
sk.wrl|Skeleton Mesh|Quality Control|
.tsv|Measurements|Analysis|

### Build & Installation

```bash
# Download and compile skel binary
git clone https://github.com/Topp-Roots-Lab/Gia3D.git && cd Gia3D && make
```

## Additional Information

* Original guide can be found in `compile-cmd`
