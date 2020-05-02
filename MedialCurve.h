#pragma once
#include <cmath>
#include "cc/voxel/Voxels.h"
#include "RootGraph.h"

class MedialCurve
{
public:
	hash_map <int,float> voxset;
	int xsize;
	int ysize;
	int zsize;
	int offset;
	string name;

	float maxd;
	float mind;
	float scale;

	MedialCurve(void);
	MedialCurve(MedialCurve &);
	MedialCurve(string filename, int off);
	MedialCurve(string filename, int off, bool ying);
	~MedialCurve(void);

	inline int subind2linind(int i, int j, int k){return k*ysize*xsize+j*xsize+i;};
	inline void linind2subind(int ind, int &x, int &y, int &z){z=(int)floor((double)ind/(double)(ysize*xsize)); y=(int)floor((double)(ind-z*ysize*xsize)/(double)xsize); x=ind-z*ysize*xsize-y*xsize; return;}

	void createSkeleton(float scale);
	void thinning();
	void thinning(hash_map<int, float> &distR);
	void scaleAxis(float sc);
	void setScale();
	void repair();
	void computeFeatures(vector<pair<string,double>> &features, MedialCurve &skel);
	void getBifClusterFeatures(vector<pair<string, double>> &features);
	void getEdgesFeatures(vector<pair<string,double>> &features);
	void dt(hash_map<int,float> &dtmap);
	void computeSkeletonEstimatedFeatures(vector<pair<string,double>> &features, hash_map<int,float> &dtmap);


private:
	int getConnectedComponents(hash_set<int> &vset, vector<set<int>> &cc, int conn);
	void get_26_neighbours(int ind, vector<int> &nb);
	void get_6_neighbours(int ind, vector<int> &nb);
	void copyDistances(hash_map<int,float> &distR);
	void getTipBranches(vector<vector<int>> &tipbranches);
	void getIncidentBranch(int tip, vector<int> &branch);
	float getBranchLength(vector<int> &br);
	void getBiffurcationList(hash_set<int> &br);
	void getTipsList(hash_set<int> &tips);
	void fillCavities(set<int> &cc, set<int> &cavity);
	double computeSurfaceArea();
	double computeSurfaceArea_lateral();
	double computeConvexVolume();
	void computeSweepingFeatures(double &medR, double &maxR, double &maxW, double &depth, double &miny);
	double computeLengthDistribution(MedialCurve &skel, double depth, double miny);
	void getBoundaryVector(vector<double> &v);
	void getBifClustersList(set<set<int>> &v_cl);
	void getEdges(vector<vector<int>> &edges);
	void getTotalLength(hash_map<int,float> &dtmap, float &len, float &vol, float &sa);
};

