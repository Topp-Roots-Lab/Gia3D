#pragma once
#include <set>
#include <map>
#include <hash_map>
#include <hash_set>
#include <vector>
using namespace std;
using __gnu_cxx::hash_set;
using __gnu_cxx::hash_map;

struct coord
{
	float x;
	float y;
	float z;
} ;

class RootGraph
{

public:
	map<int,pair<int,int>> E; //list of edges (edge_id, <v1_id, v2_id>), vertex and edge indices start from 0
	map<int,set<int>> v_einc; //map of vertex_id and a set of incident edges (edge_id)
	map<int,float> elength; //map of edge_id to edge length
	map<int,int> e_order; //map of edge_id to Horton's order
	map<int, coord> v_coord; // set of vertex coordinates  // indices start from 0
	map<int, vector<int>> arc_v; //map between edges and sequence of voxels

	RootGraph(void);
	~RootGraph(void);

	void init();
	void createDistEdgeMap(map<int,float> &cond, hash_map<int,float> &dmap);
	void getSpanningTree(bool minsp=true);
	void getSpanningTree(bool minsp, map<int,float> feature);
	void getMinCurvSpanningTree();
	void getMinCurvSpanningTreeBreadthFirstSearch();
	int getHighestNode(); // returns the id of the node with the smallest y-value
	int getLowestNode(); // returns the id of the node with the max y-value
	void HortonOrder();
	int getMaxHortonOrder();
	void writeHortonNetworkToIV(string filename, int xs, int ys, int zs);
	void graph_adjacency_output(string filename);
	void graph_description_output(string filename, map<int,vector<float>> &descr);
	void radial_geod_distr_per_bifurcation(map<int,vector<float>> &descr, float mval, int nbin);
	void smoothEdges(int iter);
	void removeSelfLoops();
	void outputFullGraphToIV(string filename);
	void orientArcs();
	//void graph_edge_description_output(string filename);
	/*void get_adjacency(vector<vector<float>> &a, map<int,int> &indexmap);*/


private:
	void get_node_list(set<int> &nlist);
	int get_node_number();
	void get_bif_node_list(set<int> &blist);
	void updateVertexIncidenceMap();
	void updateEdgeLengthMap();
	void getEdgeToVoxelMap(map<int,int> &ev);
	void getIncidentVertices(set<int> &vset,int vid);
	int findHortonOrder(vector<bool> &visited,int vtop, vector<int> &vo, vector<vector<int>> &ve);
	void curveEdgeMap(map<int,multimap<float,int>> &eemap);
	float getAngle(int e1, int e2);
	void getAngle(int e1, int e2, int &v1, int &v2);
	int getLongestEdge();
	void getCurvEdgePairMap(map<int,multimap<float,int>> &feature, multimap<float,pair<int,int>> &cee);
	void getEndVertices(int e1, int e2, int &v1, int &v2, int &vc);
	float getEdgeLength(int eid);
	void merge_edges(int e1, int e2);

};
