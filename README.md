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

|File Format|Description|Purpsoe|
|-|-|-|
or.iv|||
or.wrl|Object Mesh|Quality Control|
sk.iv|||
sk.wrl|Skeleton Mesh|Quality Control|
.tsv|Measurements|Analysis|

## Build

### Compile Dependency: `qhull-2012.1`

Run the following commands as root.

```bash
# Download qhull-2012.1 and compile for liqhullstatic.a
wget https://github.com/qhull/qhull/archive/2012.1.tar.gz -O - | tar -xvz -C /usr/local/src/ && pushd /usr/local/src/qhull-2012.1 && make && popd

find /usr/local/src/qhull-2012.1/bin -type f -exec ln -sv {} /usr/local/bin \;
ln -sv /usr/local/src/qhull-2012.1/lib/libqhull_p.so /usr/local/lib/
ln -sv /usr/local/src/qhull-2012.1/lib/libqhull.so /usr/local/lib/
ln -sv /usr/local/src/qhull-2012.1/lib/libqhullstatic.a /usr/local/lib/
ln -sv /usr/local/src/qhull-2012.1/lib/libqhullstatic_p.a /usr/local/lib/
ln -sv /usr/local/src/qhull-2012.1/lib/libqhullcpp.a /usr/local/lib/

ldconfig
```

### Compile Project

```bash
# Download and compile skel binary
git clone https://github.com/tparkerd/skel.git && cd skel && make
```

## Additional Information

* Original guide can be found in `compile-cmd`
