IDIR =./cc/voxel/
CXX = g++
CXXFLAGS = -Wno-deprecated -std=c++0x

_SOURCES = Voxels.cc VoxelRef.cc VoxelFilter.cc PalagyiFilter.cc Template.cc Templates.cc 
SOURCES = main.cpp menu.cpp MedialCurve.cpp RootGraph.cpp util.cpp $(_SOURCES:%=$(IDIR)%)

_DEPS = /usr/local/src/qhull-2012.1/src/libqhull/
DEPS =$(_DEPS:%=-I%)

LIBS =-lqhullstatic

skel:
	$(CXX) $(CXXFLAGS) $(SOURCES) $(DEPS) -o $@ $^ $(LIBS)

.PHONY: clean

clean:
	rm -vf skel
