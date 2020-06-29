IDIR =./cc/voxel/
CXX = g++
CXXFLAGS = -Wno-deprecated -std=c++0x
PREFIX = /usr/local

_SOURCES = Voxels.cc VoxelRef.cc VoxelFilter.cc PalagyiFilter.cc Template.cc Templates.cc 
SOURCES = main.cpp menu.cpp MedialCurve.cpp RootGraph.cpp util.cpp $(_SOURCES:%=$(IDIR)%)

_DEPS = /usr/local/src/qhull-2012.1/src/libqhull/
DEPS =$(_DEPS:%=-I%)

LIBS =-lqhullstatic -lboost_program_options

skel:
	$(CXX) $(CXXFLAGS) $(SOURCES) $(DEPS) $(LIBS) -o $@ $^

.PHONY: clean

clean:
	rm -vf skel

.PHONY: install
install: skel
	mkdir -pv $(DESTDIR)$(PREFIX)/bin
	cp -v skel $(DESTDIR)$(PREFIX)/bin/Skeleton

.PHONY: uninstall
uninstall:
	rm -vf $(DESTDIR)$(PREFIX)/bin/Skeleton