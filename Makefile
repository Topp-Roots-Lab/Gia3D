.DEFAULT_GOAL := help

IDIR =./cc/voxel/
CXX = g++
CXXFLAGS = -Wno-deprecated -std=c++0x
PREFIX = /usr/local

_SOURCES = Voxels.cc VoxelRef.cc VoxelFilter.cc PalagyiFilter.cc Template.cc Templates.cc 
SOURCES = main.cpp menu.cpp MedialCurve.cpp RootGraph.cpp util.cpp $(_SOURCES:%=$(IDIR)%)

_DEPS = /usr/local/src/qhull-2012.1/src/libqhull/
DEPS =$(_DEPS:%=-I%)

LIBS =-lqhullstatic -lboost_program_options

.PHONY: help
help:
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

.PHONY: clean ## Purge build artifacts
clean:
	rm -rvf build/

.PHONY: build
build: clean ## Compile Gia3D
	mkdir -pv build/bin
	$(CXX) $(CXXFLAGS) $(SOURCES) $(DEPS) $(LIBS) -o build/bin/gia3d

.PHONY: install-deps
install-deps: ## Install dependencies (i.e., qhull)
	if [ ! -d "/usr/local/src/qhull-2012.1" ]; then \
	echo "Installing qhull..."; \
	wget https://github.com/qhull/qhull/archive/2012.1.tar.gz -O - | tar -xvz -C /usr/local/src/ && make --directory /usr/local/src/qhull-2012.1; \
	find /usr/local/src/qhull-2012.1/bin -type f -exec ln -sv {} /usr/local/bin \; ; \
	ln -sfv /usr/local/src/qhull-2012.1/lib/libqhull_p.so /usr/local/lib/; \
	ln -sfv /usr/local/src/qhull-2012.1/lib/libqhull.so /usr/local/lib/; \
	ln -sfv /usr/local/src/qhull-2012.1/lib/libqhullstatic.a /usr/local/lib/; \
	ln -sfv /usr/local/src/qhull-2012.1/lib/libqhullstatic_p.a /usr/local/lib/; \
	ln -sfv /usr/local/src/qhull-2012.1/lib/libqhullcpp.a /usr/local/lib/; \
	ldconfig; \
	fi;

.PHONY: install
install: install-deps build ## Install Gia3D
	mkdir -pv $(DESTDIR)$(PREFIX)/bin
	cp -v build/bin/gia3d $(DESTDIR)$(PREFIX)/bin/Skeleton

.PHONY: uninstall
uninstall: ## Unintall Gia3D
	rm -vf $(DESTDIR)$(PREFIX)/bin/Skeleton