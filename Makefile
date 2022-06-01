IDIR =./cc/voxel/
CXX = g++
CXXFLAGS = -Wno-deprecated -std=c++0x
PREFIX = /usr/local

_SOURCES = Voxels.cc VoxelRef.cc VoxelFilter.cc PalagyiFilter.cc Template.cc Templates.cc 
SOURCES = main.cpp menu.cpp MedialCurve.cpp RootGraph.cpp util.cpp $(_SOURCES:%=$(IDIR)%)

_DEPS = /usr/local/src/qhull-2012.1/src/libqhull/
DEPS =$(_DEPS:%=-I%)

LIBS =-lqhullstatic -lboost_program_options

.PHONY: clean

clean:
	rm -rvf build

build: clean
	mkdir -pv build/bin
	$(CXX) $(CXXFLAGS) $(SOURCES) $(DEPS) $(LIBS) -o build/bin/gia3d

.PHONY: install
install: build
	mkdir -pv $(DESTDIR)$(PREFIX)/bin
	cp -v build/bin/gia3d $(DESTDIR)$(PREFIX)/bin/Skeleton

.PHONY: uninstall
uninstall:
	rm -vf $(DESTDIR)$(PREFIX)/bin/Skeleton