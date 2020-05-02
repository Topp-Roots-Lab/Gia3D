# Skel

Root3D Skeleton Traits calculates the traits based on skeleton by Olga Symonova (olga.symonova@gmail.com).

## How to Compile

Original guide can be found in `compile-cmd`

```bash
# Download qhull-2012.1 and compile for liqhullstatic.a
wget https://github.com/qhull/qhull/archive/2012.1.tar.gz -O - | tar -xz -C /path/to/directory
pushd qhull-2012.1 && make && popd

git clone https://github.com/tparkerd/skel.git && cd skel && make
```
