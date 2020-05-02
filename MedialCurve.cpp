#define _USE_MATH_DEFINES
#include "MedialCurve.h"
#include <stack>
#include <queue>
#include <time.h>
#include <math.h>
#include "util.h"
#include "RootGraph.h"
#include "cc/voxel/VoxelFilter.h"
#include "cc/voxel/PalagyiFilter.h"
#ifdef __cplusplus /* identifies C++ */
extern "C" { // tell the C++ compiler that you have C decls
#include "libqhull.h"
}
#endif

//#define DEBUG

MedialCurve::MedialCurve(void)
{
}

/**
* clone the object
*/
MedialCurve::MedialCurve(MedialCurve &rm)
{
	xsize=rm.xsize;
	ysize=rm.ysize;
	zsize=rm.zsize;
	name=rm.name;
	// copy the hash map of voxel set
	voxset.insert(rm.voxset.begin(), rm.voxset.end());
}
///**
//* create a new object from the file: Randy Clark's format of the root representation RTVOX
//*/
MedialCurve::MedialCurve(string filename, int off)
{
	ifstream f;
	f.open(filename.c_str());
	string line;

	if(!f)
		cout<< "Error during opening file!";

	offset=off;
	int a=0, b=0, c=0, zlen;
	int p=0, pt=0, pstop;
	int width, height, depth;
	string  zstr, xystr;
	vector<string> ptr;
	int k=0;
	while(std::getline(f, line)) // Read line by line
	{
		k++;
		//find the tag width
		p=line.find("<Width>", p);
		if(p>0) p =p+7;
		else
		{
			cout<<"Can't find the value of width - exitting...";
			exit(0);
		}
		pt=line.find("<\\Width>", p);
		xystr=line.substr(p,pt-p);
		width=atoi(xystr.c_str());
		//printf("Width: %d\n", width);

		//find the tag height
		p=line.find("<Height>", p);
		if(p>0) p =p+8;
		else
		{
			cout<<"Can't find the value of height - exitting...";
			exit(0);
		}
		pt=line.find("<\\Height>", p);
		xystr=line.substr(p,pt-p);
		height=atoi(xystr.c_str());
		//printf("Heigth: %d\n", height);

		//find the tag depth
		p=line.find("<Depth>", p);
		if(p>0) p =p+7;
		else
		{
			cout<<"Can't find the value of depth - exitting...";
			exit(0);
		}
		pt=line.find("<\\Depth>", p);
		xystr=line.substr(p,pt-p);
		depth=atoi(xystr.c_str());
		//printf("Depth: %d\n", depth);

		// add offset number of voxels on each side
		xsize=width+2*offset;
		ysize=height+2*offset;
		zsize=depth+2*offset;

		while(p>=0)
		{
			//slice number gives the hight values -> y coordinate
			p=line.find("<Slice slc=\"", p);
			if(p>0) p=p+12;
			else
				continue;
			pt=line.find("\">", p);
			zstr=line.substr(p,pt-p);
			zlen=zstr.length();
			b=atoi(zstr.c_str());
			// the root representation is upside-down: reverse it
			b=height-b-1+offset;
			pstop =line.find("</Slice>", p);
			while(pt<pstop-5)
			{
				//find x and z coordinated if a voxel
				p=line.find("<RV>", p)+4;
				pt=line.find("</RV>", p);
				xystr=line.substr(p,pt-p);
				tokenize(xystr, ptr, " ");
				a=atoi(ptr[0].c_str());
				a=a+offset;
				c=atoi(ptr[1].c_str());
				c=c+offset;
				// convert to linear indices and insert into the map
				voxset.insert(make_pair(subind2linind(a,b,c),1.f));
				ptr.clear();
			}
		}
	}
	f.close();
	name=filename;
}

/**
* create a new object from the file: Ying's format of the root representation
*/
MedialCurve::MedialCurve(string filename, int off, bool ying)
{
	ifstream f;
	f.open(filename.c_str());
	string line;

	if(!f)
		cout<< "Error during opening file!";

	offset=off;
	int a=0, b=0, c=0;
	int p=0, pt=0;
	string  zstr, xystr;
	vector<string> ptr;
	int k=0;

	int nvox;
	vector<vector<int>> coords;
	int minx, maxx, miny, maxy, minz, maxz;
	minx=INT_MAX; miny=INT_MAX; minz=INT_MAX;
	maxx=INT_MIN; maxy=INT_MIN; maxz=INT_MIN;
	//first line is the cell size - skip
	std::getline(f, line);
	//second line - is the number of non-empty cells
	std::getline(f, line);
	//reserve the space for all the coordinates
	nvox=atoi(line.c_str());
	coords.resize(nvox);
	for(int i=0; i<nvox; i++)
		coords[i].resize(3);
	while(std::getline(f, line)) // Read line by line
	{

		tokenize(line, ptr, " ");
		coords[k][0]=atoi(ptr[0].c_str());
		coords[k][2]=atoi(ptr[1].c_str());
		coords[k][1]=atoi(ptr[2].c_str());
		ptr.clear();

		//update min and max
		if (minx>coords[k][0]) minx=coords[k][0];
		if (miny>coords[k][1]) miny=coords[k][1];
		if (minz>coords[k][2]) minz=coords[k][2];
		if (maxx<coords[k][0]) maxx=coords[k][0];
		if (maxy<coords[k][1]) maxy=coords[k][1];
		if (maxz<coords[k][2]) maxz=coords[k][2];
		k++;
	}
	f.close();
	name=filename;
	//find the middle place of the root
	int midx=minx+(int)ceil((float)((maxx-minx)/2));
	int midy=miny+(int)ceil((float)((maxy-miny)/2));
	int midz=minz+(int)ceil((float)((maxz-minz)/2));
	xsize=maxx-minx+1+2*offset;
	ysize=maxy-miny+1+2*offset;
	zsize=maxz-minz+1+2*offset;
	for(int i=0; i<nvox; i++)
	{
		coords[i][0]=coords[i][0]-minx+offset;
		coords[i][1]=coords[i][1]-miny+offset;
		coords[i][2]=coords[i][2]-minz+offset;
	}

	/*xsize=2*midx+2*offset;
	ysize=2*midy+2*offset;
	zsize=2*midz+2*offset;*/
	//now convert coordinates to linear indices;
	for(int i=0; i<nvox; i++)
		voxset.insert(make_pair(subind2linind(coords[i][0],coords[i][1],coords[i][2]),1.f));
}

MedialCurve::~MedialCurve(void)
{
	voxset.clear();
}


/**
* This function creates a vector of branches incident to tips.
* Each branch is represented by a vector of linear indices of voxels
*/
void MedialCurve::getTipBranches(vector<vector<int>> &tipbranches)
{
	tipbranches.clear();
	hash_set<int> tips;
	hash_set<int>::iterator itt;
	//get a set of indices of tip voxels
	getTipsList(tips);
	for(itt=tips.begin(); itt!=tips.end(); itt++)
	{
		vector<int> br;
		//find an incident branch
		getIncidentBranch(*itt,br);
		tipbranches.push_back(br);
	}
}

/**
* Given an index of a pendant voxel tip, the function returns a set of indices composing the incident branch.
* This function takes a tip voxel and visits incident voxel,
* one after another until it incounters a bifurcation voxel with # of incident voxels >2.
* The bifurcation voxel is included in the branch.
*/
void MedialCurve::getIncidentBranch(int tip, vector<int> &branch)
{
	branch.clear();
	int nb=1;
	int v=tip;
	hash_set<int>visited;
	while(nb==1)
	{
		branch.push_back(v);
		visited.insert(v);
		vector<int> nbv(0);
		// try to find new neighbor
		for(int k=-1; k<=1; k++)
			for(int j=-1; j<=1; j++)
				for(int i=-1; i<=1; i++)
				{
					int ind=v+k*ysize*xsize+j*xsize+i;
					if(ind==v) continue;
					//if such neighbor exists and it wasn't visited
					if(voxset.find(ind)!=voxset.end() && visited.find(ind)==visited.end())
					{
						nbv.push_back(ind);
					}
				}
		nb=nbv.size();
		if(nb>0)
			v=nbv[0];
	}
}

/**
* get the length of the branch
*/
float MedialCurve::getBranchLength(vector<int> &br)
{
	float len=0;
	int n=br.size();
	int x1, x2, y1, y2, z1, z2;
	for(int i=0; i<n-1; i++)
	{
		linind2subind(br[i],x1,y1,z1);
		linind2subind(br[i+1],x2,y2,z2);
		len=len+(float)sqrt((float)((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2)));
	}
	return len;
}

/**
* construct a skeleton
*/
void MedialCurve::createSkeleton(float scale)
{
	//create a map where erosion distance will be stored
	hash_map<int, float> distR;
	//apply thinning with erosion distance computation
	thinning(distR);
	//filter the skeleton
	//writeVisibleIVEff("root_skel_thin.iv",skel);
	setScale();
	scaleAxis((float)scale);
}

/**
* Compute Scale Axis -  a filtered skeleton based on
* the comparison of the size of the edge with the size
* of the incident bifurcation region which is estimated
* using the erosion distance. Erosion distance is computed
* during the thinning algorithm, it is estimated by
* the number of the iterations which it takes to erode
* the shape til one-voxel wide curve.
*/
void MedialCurve::scaleAxis(float sc)
{
	scale=sc;
	bool removed_edges=true;
	hash_map<int,float> distR(voxset);
	hash_set<int> br_list;
	hash_set<int>::iterator brit;
	setScale();

	// continue filtering while the edges can be removed
	while(removed_edges)
	{
		removed_edges=false;

		//get all pendant branches - the branches incident to tip voxels
		vector<vector<int>> tipbranches;
		getTipBranches(tipbranches);
		// try to filter pendant branches, return true if some branches were removed
		int nbr=tipbranches.size();
		for(int i=0; i<nbr; i++)
		{
			int node_branch=tipbranches[i].size();
			//by branch construction the bif node should be the last
			int bifnode=tipbranches[i][node_branch-1];
			if(voxset.find(bifnode)==voxset.end()) continue;
			// get the value of the erosion distance at the bifurcation node
			float erd_bif=voxset.find(bifnode)->second;
			//by branch construction the tip node should be the first
			int tipnode=tipbranches[i][0];
			// get the value of the erosion distance at the tip node
			if(voxset.find(tipnode)==voxset.end()) continue;
			float erd_tip=voxset.find(tipnode)->second;
			//get the size of the branch
			float elen =(float)tipbranches[i].size();
			//float elen =getBranchLength(tipbranches[i]);
			// if elen+erd_tip < erd_bif*sc then remove the branch
			// compare the size of the branch with the scaled value
			// of the erosion distance at the bifurcation node
			if(elen+erd_tip < erd_bif*sc)
			{
				//erase all voxels except the last voxel,
				//last voxels is bufurcation, its removal can disconnect the skeleton
				for(int j=0; j<node_branch-1;j++)
					voxset.erase(tipbranches[i][j]);
				removed_edges=true;
			}
		}
		distR=voxset;
		//apply additional thinnging because after
		//removing branches there might remain bifurcation clusters
		thinning();
		//updater the hash map with the erosion distances
		copyDistances(distR);
	}
	//storeDTIVFile("scaleAxis", voxset, maxd);
}

/**
* copy distance values associated
* with the voxel indices in the map distR to the current object
*/
void MedialCurve::copyDistances(hash_map<int,float> &distR)
{
	hash_map<int,float>::iterator it;
	hash_map<int,float>::iterator it1;
	for(it=voxset.begin(); it!=voxset.end(); it++)
	{
		it1=distR.find(it->first);
		it->second=it1->second;
	}
}

/**
* Get the vector of 26 neighbors of the given voxel ind
*/
void MedialCurve::get_26_neighbours(int ind, vector<int> &nb)
{
	nb.resize(26);
	int s=0;
	for(int k=-1; k<=1; k++)
		for(int j=-1; j<=1; j++)
			for(int i=-1; i<=1; i++)
			 {
				 int nbind=ind+k*ysize*xsize+j*xsize+i;
				 if(nbind!=ind)
				 {
					 nb[s]=nbind;
					 s++;
				 }
			 }
}

/**
* Get the vector of 6 face-sharing neighbors of the given voxel ind
*/
void MedialCurve::get_6_neighbours(int ind, vector<int> &nb)
{
	nb.resize(6);
	nb[0]=ind+ysize*zsize;
	nb[1]=ind-ysize*zsize;
	nb[2]=ind+zsize;
	nb[3]=ind-zsize;
	nb[4]=ind+1;
	nb[5]=ind-1;
}


/***
* Get all the indices of bifurcation nodes stored in the hash_set
*/
void MedialCurve::getBiffurcationList(hash_set<int> &br)
{
	br.clear();
	hash_map<int,float>::iterator it;
	for(it=voxset.begin(); it!=voxset.end(); ++it)
	{
		int nb=0;
		//check if the current node is a branch
		for(int k=-1; k<=1; k++)
			for(int j=-1; j<=1; j++)
				for(int i=-1; i<=1; i++)
				{
					int temp=it->first+k*ysize*xsize+j*xsize+i;
					if(voxset.find(temp)!=voxset.end() && temp!=it->first)
						nb++;
				}
		if(nb>2)
			br.insert(it->first);
	}
}

/**
* Get a vector of branches in the object.
* A one-voxel wide structure is assumed.
* Each edge is represented as a vector of ordered voxels indices.
*/
void MedialCurve::getEdges(vector<vector<int>> &edges)
{
	edges.clear();
	hash_map<int,float>::iterator it;
	hash_map<int,float>::iterator itf;
	hash_set<int>::iterator its;
	hash_set<int> branches;
	hash_set<int> nonbr;
	bool flag;
	int n;
	// in this loop I collect bifurcation and non-bif voxels into 2 different sets
	for(it=voxset.begin(); it!=voxset.end(); it++)
	{
		n=0;
		//check how many neighbours
		for(int k=-1; k<=1; k++)
			for(int j=-1; j<=1; j++)
				for(int i=-1; i<=1; i++)
				{
					int ind=it->first+k*ysize*xsize+j*xsize+i;
					if(ind==it->first) continue;
					itf=voxset.find(ind);
					if(itf!=voxset.end())
						n++;
				}
		if(n>2)
			branches.insert(it->first);
		else
			nonbr.insert(it->first);
	}

	queue<int> q;
	vector<int> start_edge;
	//while there are non-bifurcation voxels
	while(!nonbr.empty())
	{
		int e=*nonbr.begin();
		q.push(e);
		vector<int> ne;
		int br=0;
		// collect all non-branching voxels belonging to the same branch
		while(!q.empty())
		{
			e=q.front();
			ne.push_back(e);
			q.pop();
			nonbr.erase(e);
			n=0;
			//try to find neighbors
			for(int k=-1; k<=1; k++)
				for(int j=-1; j<=1; j++)
					for(int i=-1; i<=1; i++)
					{
						int ind=e+k*ysize*xsize+j*xsize+i;
						if(ind==e) continue;
						its=nonbr.find(ind);
						if(its!=nonbr.end())
						{
							n++;
							q.push(ind);
							nonbr.erase(ind);
						}
				}
			//if no neighbours - find adjacent branching point
			if(n==0)
			{
				flag=false;
				for(int k=-1; k<=1; k++)
					{
						for(int j=-1; j<=1; j++)
						{
							for(int i=-1; i<=1; i++)
							{
								int ind=e+k*ysize*xsize+j*xsize+i;
							if(ind==e) continue;
							its=branches.find(ind);
							if(its!=branches.end())
							{
								br++;
								ne.push_back(ind);
								if(br==1)
								{
									start_edge.push_back(ind);
									flag=true;
									break;
								}
							}
						}if(flag==true) break;
					}
					if(flag==true) break;
				}
				// if no neighbours at all, this is an ending point
				if(br==0)
				{
					br=1;
					start_edge.push_back(e);
					flag=true;
				}
			}
		}
		edges.push_back(ne);
	}

	//now we have all indices of the voxels composing branches
	// order the indices starting from a branching point
	// starting branching points are stored in the start_edge
	vector<int>edge;
	for(int ii=0; ii<(int)edges.size(); ii++)
	{
		edge.clear();
		hash_set<int>edge_pool(edges[ii].begin(),edges[ii].end());
		int v=start_edge[ii];
		edge.push_back(v);
		while(!edge_pool.empty())
		{
			edge_pool.erase(v);
			flag=false;
			for(int k=-1; k<=1; k++){
				for(int j=-1; j<=1; j++){
					for(int i=-1; i<=1; i++)
						{
							int ind=v+k*ysize*xsize+j*xsize+i;
							if(ind==v) continue;
							its=edge_pool.find(ind);
							if(its!=edge_pool.end())
							{
								v=*its;
								edge.push_back(v);
								flag=true;
								break;
							}
						}
						if(flag==true) break;
				}
				if(flag==true) break;
			}
		}
		edges[ii].swap(edge);
	}
}

/**
* Compute features associate with skeleton edges.
* Only the pendant edges are concidered in order not to mix in different meanings
* The edge features are stored in the map features, the key is the name of the feature;
* Edge_num, Av_Edge_length
*/
void MedialCurve::getEdgesFeatures(vector<pair<string,double>> &features)
{
	vector<vector<int>> edges;
	// first get all edges
	getEdges(edges);
	hash_set<int> tips;
	//get the set of pendant voxels
	getTipsList(tips);

	vector<int>edge;
	//compute features: number of edges, av length, len_distr
	float av_len=0;
	int nume=0;
	//multiset<float> elen_hist;
	for(int ii=0; ii<(int)edges.size(); ii++)
	{
		//recall that edges are ordered from a branching point
		//check if the last index in the edge is the tip
		int esize=(int)edges[ii].size();
		//compute features for non-tip-incident edges
		if(tips.find(edges[ii][esize-1])!=tips.end())
			continue;
		nume++;
		float eleni = getBranchLength(edges[ii]);
		av_len=av_len+eleni;
		//elen_hist.insert(eleni);
	}
	av_len=av_len/nume;
	//output_histogram("edge_len_hist.txt", elen_hist);
	features.push_back(make_pair("Edge_num",nume));
	features.push_back(make_pair("Av_Edge_length",av_len));
	//printf("Edge length histogram was output into the file edge_len_hist.txt\n");
}

/**
* Compute the features associated with bifurcation clusters.
* The value of the features are stored in the map,
* where the key is the name of the feature:
* Number_bif_cl, Av_size_bif_cl
*/
void MedialCurve::getBifClusterFeatures(vector<pair<string, double>> &features)
{
	//first compute conn components of bif clusters
	set<set<int>> bifcl;
	getBifClustersList(bifcl);
	int nb=bifcl.size();
	features.push_back(pair<string,double>("Number_bif_cl",nb));
	//compute average size of the clusters
	float brsize=0;
	set<set<int>>::iterator it;
	for(it=bifcl.begin(); it!=bifcl.end(); it++)
		brsize=brsize+(*it).size();
	brsize=brsize/nb;
	features.push_back(pair<string,double>("Av_size_bif_cl",brsize));
	//average incidence degree of bif cluster?
}

/**
* Get the set of connested components of the bifurcation clusters.
* Each bifurcation cluster is represented
* as a set of indices of bifurcation voxels.
*/
void MedialCurve::getBifClustersList(set<set<int>> &v_cl)
{
	v_cl.clear();
	vector<double> pos(3);
	hash_map<int,float>::iterator it;
	hash_map<int,float>::iterator itf;
	vector<vector<int>> brcomp;
	hash_set<int>::iterator its;
	hash_set<int> branches;
	int n;
	//get all bifurcation voxels
	for(it=voxset.begin(); it!=voxset.end(); it++)
	{
		n=0;
		//check how many neighbours
		for(int k=-1; k<=1; k++)
			for(int j=-1; j<=1; j++)
				for(int i=-1; i<=1; i++)
				{
					int ind=it->first+k*ysize*xsize+j*xsize+i;
					if(ind==it->first) continue;
					itf=voxset.find(ind);
					if(itf!=voxset.end())
						n++;
				}
		if(n>2)
			branches.insert(it->first);
	}
	//collect branching nodes into connceted components
	queue<int> q;
	while(!branches.empty())
	{
		int node=*branches.begin();
		q.push(node);
		vector<int> brc;

		while(!q.empty())
		{
			node=q.front();
			brc.push_back(node);
			q.pop();
			branches.erase(node);
			n=0;
			//try to find neighbors
			for(int k=-1; k<=1; k++)
				for(int j=-1; j<=1; j++)
					for(int i=-1; i<=1; i++)
					{
						int ind=node+k*ysize*xsize+j*xsize+i;
						if(ind==node) continue;
						its=branches.find(ind);
						if(its!=branches.end())
						{
							n++;
							q.push(ind);
							branches.erase(ind);
						}
					}
		}
		brcomp.push_back(brc);
	}
	//convert from vector to set representation
	for(int i=0; i<(int)brcomp.size();i++)
	{
		set<int> c;
		for(int j=0; j<(int)brcomp[i].size();j++)
			c.insert(brcomp[i][j]);
		v_cl.insert(c);
	}
}

/**
* Set scale: set the values mind and maxd
* to the min and max values stored in the map voxset
*/
void MedialCurve::setScale()
{
	hash_map<int,float>::iterator it;
	mind=10000;
	maxd=0;

	for(it=voxset.begin(); it!=voxset.end(); ++it)
	{
		if(maxd<it->second) maxd=it->second;
		if(mind>it->second) mind=it->second;
	}
}


/**
* Apply thinning algorithm developed by Patrick Min.
* This function simply erodes boundary voxels of the object
* without changing its topology.
* This function computes erosion distance which is stored in the map distR.
*/
void MedialCurve::thinning(hash_map<int, float> &distR)
{
	distR.clear();
	//create voxel structure for thinning
	Voxels vv(xsize, ysize, zsize,0);
	vv.set_voxels(voxset);

	string filename;
	string voxel_extension;
	//int type;
	//output to a file
	//filename = "bin_test";
	//voxel_extension = "binvox";
	//VoxelFile vf(vv,filename);
	//type = vf.get_filetype(voxel_extension);
    //vf.open_for_write(type);
	//vf.write_file();


	VoxelFilter *filter = 0;
	string filter_type = "palagyi";

	if (filter_type.compare("palagyi") == 0) {
		filter = new PalagyiFilter(vv); // , *voxel_dt);
	  }

	distR.clear();
	if (filter) {
#ifdef DEBUG
		cout << "running filter [" << filter_type << "]" << endl;
#endif
		filter->applyWdist(distR);
	}
	//put thinned voxel structure back into medialCurve object
	hash_map<int, float> newvs;
	hash_map<int, float>::iterator it;
	int s=vv.get_size();
	for(int i=0; i<s; i++)
		if(vv[i]==1)
		{
			it=distR.find(i);
			if(it==distR.end())
			{
				printf("Error in thinning, can't find erosion distance!\n");
				continue;
			}
			newvs.insert(make_pair<int,float>(i,it->second));
		}
		//output to a file
	/*filename = "thin_test";
	voxel_extension = "binvox";
	VoxelFile vf2(vv,filename);
	type = vf2.get_filetype(voxel_extension);
    vf2.open_for_write(type);
	vf2.write_file();*/

	voxset.swap(newvs);
}

/**
* Returns #of faces to be removed = #non-shared faces
*/
int getFacesToBeRemoved(vector<bool> &nb26)
{
	int frem=0;
	if(nb26[4]==0) frem++;
	if(nb26[10]==0) frem++;
	if(nb26[12]==0) frem++;
	if(nb26[13]==0) frem++;
	if(nb26[15]==0) frem++;
	if(nb26[21]==0) frem++;
	return frem;
}

/**
* Returns #of edges to be removed = #non-shared edges
* In addition returns array of the #of edge with 0 corresponding to  the removed edge
* Neighbor voxels are indexed in the following manner:
* 			6 7 8			14 15 16		23 24 25
* buttom:	3 4 5	middle: 12    13  top:	20 21 22
* 			0 1 2			9  10 11		17 18 19
* the edges are indexed in the following manner:
* 		 /|-----10-----/|
* 		/ |			  / |
* 	   11 |          9  |
*     /   |	        /   |
* 	 /----+--- 8---|    |
* 	 |    |        |    |
* 	 |    7	       |    6
* 	 4    |        5    |
* 	 |    /---- 2--+----/
*    |   /         |   /
* 	 |  3          |  1
* 	 | /           | /
* 	 |------ 0-----|/
*/
int getEdgeToBeRemoved(vector<bool> &nb26, vector<bool> &earr)
{
	int erem=0;
	int s;
	earr.assign(12,true);
	s=nb26[10]+nb26[1]+nb26[4];
	if(s==0) {erem++; earr[0]=0;}

	s=nb26[4]+nb26[5]+nb26[13];
	if(s==0) {erem++; earr[1]=0;}

	s=nb26[4]+nb26[7]+nb26[15];
	if(s==0) {erem++; earr[2]=0;}

	s=nb26[4]+nb26[3]+nb26[12];
	if(s==0) {erem++; earr[3]=0;}

	s=nb26[12]+nb26[9]+nb26[10];
	if(s==0) {erem++; earr[4]=0;}

	s=nb26[10]+nb26[11]+nb26[13];
	if(s==0) {erem++; earr[5]=0;}

	s=nb26[13]+nb26[16]+nb26[15];
	if(s==0) {erem++; earr[6]=0;}

	s=nb26[12]+nb26[14]+nb26[15];
	if(s==0) {erem++; earr[7]=0;}

	s=nb26[10]+nb26[18]+nb26[21];
	if(s==0) {erem++; earr[8]=0;}

	s=nb26[13]+nb26[22]+nb26[21];
	if(s==0) {erem++; earr[9]=0;}

	s=nb26[15]+nb26[24]+nb26[21];
	if(s==0) {erem++; earr[10]=0;}

	s=nb26[12]+nb26[20]+nb26[21];
	if(s==0) {erem++; earr[11]=0;}
	return erem;
}

/**
* Returns #of vertices to be removed = #non-shared vertices
* Consider the array of edges returned by the above method.
* A vertex is non-shared if all three incident edges in a voxel
* are non-shared, i.e. there value is =0
*/
int getVertexToBeRemoved(vector<bool> &earr)
{
	int vrem=0;
	int s=0;
	s=earr[0]+earr[3]+earr[4];
	if(s==0) vrem++;

	s=earr[0]+earr[1]+earr[5];
	if(s==0) vrem++;

	s=earr[1]+earr[2]+earr[6];
	if(s==0) vrem++;

	s=earr[2]+earr[3]+earr[7];
	if(s==0) vrem++;

	s=earr[4]+earr[6]+earr[11];
	if(s==0) vrem++;

	s=earr[5]+earr[8]+earr[9];
	if(s==0) vrem++;

	s=earr[6]+earr[9]+earr[10];
	if(s==0) vrem++;

	s=earr[7]+earr[10]+earr[11];
	if(s==0) vrem++;
	return vrem;
}

//
/**
* Apply thinning algorithm developed by Patrick Min.
* This function simply erodes boundary voxels of the object
* without changing its topology.
* This function does NOT compute erosion distance.
*/
void MedialCurve::thinning()
{
	//create voxel structure for thinning
	Voxels vv = Voxels(xsize, ysize, zsize,0);
	vv.set_voxels(voxset);

	VoxelFilter *filter = 0;
	string filter_type = "palagyi";

	if (filter_type.compare("palagyi") == 0) {
		filter = new PalagyiFilter(vv); // , *voxel_dt);
	  }

	if (filter) {
#ifdef DEBUG
		cout << "running filter [" << filter_type << "]" << endl;
#endif
		filter->apply_fast();
	}
	//put thinned voxel structure back into medialCurve object
	hash_map<int, float> newvs;
	int s=vv.get_size();
	for(int i=0; i<s; i++)
		if(vv[i]==1)
			newvs.insert(make_pair<int,float>(i,1.f));
	voxset.swap(newvs);
}

/**
* Get a set of tip nodes (degree=1)
*/
void MedialCurve::getTipsList(hash_set<int> &tips)
{
	tips.clear();
	hash_map<int,float>::iterator it;
	for(it=voxset.begin(); it!=voxset.end(); ++it)
	{
		int nb=0;
		//check if the current node is a branch
		for(int k=-1; k<=1; k++)
			for(int j=-1; j<=1; j++)
				for(int i=-1; i<=1; i++)
				{
					int temp=it->first+k*ysize*xsize+j*xsize+i;
					if(voxset.find(temp)!=voxset.end() && temp!=it->first)
						nb++;
				}
		if(nb==1)
			tips.insert(it->first);
	}
}

/**
* This function performs a number of sjape repair prpcedures.
* One connceted component without inner cavities is assumed.
* First all connceted components except the biggest will be removed.
* Then, connceted components of the background are computed,
* and smaller components corresponding to inner cavities are removed (added to the object)
*/
void MedialCurve::repair()
{
//#define DEBUG
	vector<set<int>> cc;
#ifdef DEBUG
	printf("Computing connected components...\n");
#endif
	hash_set<int> vset;
	for(hash_map<int,float>::iterator it=voxset.begin(); it!=voxset.end(); it++)
		vset.insert(it->first);
	getConnectedComponents(vset,cc,26);
	//printf("Number connected components of foreground 26nb: %d\n",cc.size());
#ifdef DEBUG
	printf("Removing small connected components...\n");
#endif
	int del=0;
	//get max size of connected components
	int ms=0;
	for(int i=0; i<(int)cc.size(); i++)
	{
		if((int)cc[i].size()>ms) ms=cc[i].size();
		if(cc[i].size()<100)
		{
			for(set<int>::iterator it=cc[i].begin(); it!=cc[i].end(); ++it)
			{
				voxset.erase(*it);
			}
			del++;
		}
	}
	// remove all conncetd components of the size smaller than ms
	for(int i=0; i<(int)cc.size(); i++)
	{
		if((int)cc[i].size()<ms)
		{
			for(set<int>::iterator it=cc[i].begin(); it!=cc[i].end(); ++it)
			{
				voxset.erase(*it);
			}
			del++;
		}
	}
#ifdef DEBUG
	printf("Removed %i connected components\n", del);
#endif
	//fill the cavities only in max component
#ifdef DEBUG
	printf("Filling cavities...\n");
#endif
	set<int> cavity;
	for(int i=0; i<(int)cc.size(); i++)
	{
		// fill cavities only in the remained connected components
		if(cc[i].size()==ms)
			fillCavities(cc[i],cavity);
	}
	//add voxels of the cavity to the object
	for(set<int>::iterator it=cavity.begin(); it!=cavity.end(); ++it)
		voxset.insert(make_pair(*it,1.f));
}


/**
* This function computes connected components of the list of linear indices given in vset.
* Each connceted component is stored as an element of the vector,
* and it is represented by a  set of linear indices.
* The third argument is the connectevity scheme: it should be 26 for foregrounf and 6 for background
*/
int MedialCurve::getConnectedComponents(hash_set<int> &vset, vector<set<int>> &cc, int conn)
{
	cc.clear();
	queue<int> q;
	//create a temporary list of indices
	hash_set<int> temp(vset);
	hash_set<int>::iterator ittemp;

	int n=0;
	int indn;
	while(!temp.empty())
	{
		n++;
		q.push(*temp.begin());
		set<int> subset;
		cc.push_back(subset);
		temp.erase(temp.begin());
		while(!q.empty())
		{
			int ind =q.front();
			q.pop();
			//check all neigbours of ind - connectivity 26;
			if(conn==26)
			{
				for(int k=-1; k<=1; k++)
					for(int j=-1; j<=1; j++)
						for(int i=-1; i<=1; i++)
						{
							indn=ind+k*ysize*xsize+j*xsize+i;
							ittemp=temp.find(indn);
							if(ittemp!=temp.end() &&  ind!=indn)
							{
								q.push(indn);
								temp.erase(ittemp);
							}
						}
			}
			//check only face neigbours of ind - connectivity 6;
			if(conn==6)
			{
				for(int k=-1; k<=1; k++)
					for(int j=-1; j<=1; j++)
						for(int i=-1; i<=1; i++)
						{
							if(abs(i)+abs(j)+abs(k)!=1) continue;
							indn=ind+k*ysize*xsize+j*xsize+i;
							ittemp=temp.find(indn);
							if(ittemp!=temp.end() &&  ind!=indn)
							{
								q.push(indn);
								temp.erase(ittemp);
							}
						}
			}
			cc[n-1].insert(ind);
		}
	}
	return n;
}

/**
* This function returns the list of voxels which are empty
* inside the connected component represented
* by a set of linear indices given in cc.
*/
void MedialCurve::fillCavities(set<int> &cc, set<int> &cavity)
{
	set<int>::iterator it;
	hash_set<int> bd;
#ifdef DEBUG
	printf("Collecting boundary voxels...\n");
#endif
	// find all boundary voxels, i.e. voxels which has an empty neighbor voxel
	for(it=cc.begin(); it!=cc.end(); ++it)
	{
		int ind=*it;
		for(int k=-1; k<=1; k++)
			for(int j=-1; j<=1; j++)
				for(int i=-1; i<=1; i++)
				{
					int temp=ind+k*ysize*xsize+j*xsize+i;
					if(voxset.find(temp)==voxset.end())
						bd.insert(temp);
				}
	}
#ifdef DEBUG
	printf("Computing cc of bd voxels...\n");
#endif
	//now compute connected components of bd voxels using 6 connectivity scheme
	vector<set<int>>bcc(0);
	getConnectedComponents(bd,bcc,6);
	//printf("Number connected components of background 6nb: %d\n",bcc.size());
#ifdef DEBUG
		printf(" There are %i boundary components.\n",bcc.size());
#endif
	if(bcc.size()==1) return;

	//find the maximum connceted component - it is the outmost boundary
	int ms=0;
	int indmax;
	for(int i=0; i<(int)bcc.size(); i++)
		if((int)bcc[i].size()>ms)
		{
			ms=bcc[i].size();
			indmax=i;
		}
	//now consider only smaller components -remove the outmost one from the list
	bcc.erase(bcc.begin()+indmax);

	//now fill in inside of the smaller boundary components using the 6-connectivity scheme
	queue<int> q;
	q.empty();
	int temp;
	set<int> bcfill;
	for(int i=0; i<(int)bcc.size(); i++)
	{
		bcfill.clear();
		q.push(*bcc[i].begin());
		while(!q.empty())
		{
			int ind=q.front();
			q.pop();
			bcfill.insert(ind);
			//check 6neighbors
			temp=ind-ysize*xsize;
			if(voxset.find(temp)==voxset.end() && bcfill.find(temp)==bcfill.end())
			{q.push(temp); bcfill.insert(temp);}
			temp=ind+ysize*xsize;
			if(voxset.find(temp)==voxset.end() && bcfill.find(temp)==bcfill.end())
			{q.push(temp); bcfill.insert(temp);}
			temp=ind-xsize;
			if(voxset.find(temp)==voxset.end() && bcfill.find(temp)==bcfill.end())
			{q.push(temp); bcfill.insert(temp);}
			temp=ind+xsize;
			if(voxset.find(temp)==voxset.end() && bcfill.find(temp)==bcfill.end())
			{q.push(temp); bcfill.insert(temp);}
			temp=ind-1;
			if(voxset.find(temp)==voxset.end() && bcfill.find(temp)==bcfill.end())
			{q.push(temp); bcfill.insert(temp);}
			temp=ind+1;
			if(voxset.find(temp)==voxset.end() && bcfill.find(temp)==bcfill.end())
			{q.push(temp); bcfill.insert(temp);}
		}
#ifdef DEBUG
		printf(" %i boundary cc has %i voxels\n",i+1, bcfill.size());
#endif
		bcc[i].swap(bcfill);
	}

	int filled=0;
	for(int i=0; i<(int)bcc.size(); i++)
	{
		for(it=bcc[i].begin(); it!=bcc[i].end(); ++it)
			cavity.insert(*it);
		filled=filled+bcc[i].size();
	}
#ifdef DEBUG
	printf("Number of filled voxels: %i\n", filled);
#endif

}

//
///**
//* This function computes root features.
//* The features are stored in the map, where the key is the name of the feature.
//* The following features are computed:
//* SurfArea, Volume, Convex_Volume, Solidity, Medium number of roots (MedR),
//* Max number of roots (MaxR), Bushiness, Depth, Horizontal Equivalent Radius(HorEqDiameter),
//* TotalLength, SRL((=TotalLength/Volume)), Length_Distr, W_D_ratio (=HorEqDiameter/Depth)
//*/
void MedialCurve::computeFeatures(vector<pair<string,double>> &features, MedialCurve &skel)
{
	clock_t start, end;
	double elapsed;
	start=clock();
	//surface area
	features.push_back(make_pair("SurfArea",computeSurfaceArea()));
	//lateral_surface_area
	//features.insert(make_pair("SurfArea_lateral",computeSurfaceArea_lateral()));
	//volume
	double volume=voxset.size();
	features.push_back(make_pair("Volume",volume));
	//convex volume
	double conv_volume=computeConvexVolume();
	features.push_back(make_pair("Convex_Volume",conv_volume));
	//solidity
	features.push_back(make_pair("Solidity",volume/conv_volume));
	// compute sweeping features
	double maxHorConvR;
	double medR, maxR, depth, miny;
	// med number of roots
	//max number of roots
	computeSweepingFeatures(medR, maxR, maxHorConvR, depth, miny);
	features.push_back(make_pair("MedR",medR));
	features.push_back(make_pair("MaxR",maxR));
	//bushiness
	features.push_back(make_pair("Bushiness",maxR/medR));
	// average radius
	// depth
	features.push_back(make_pair("Depth",depth));
	//max horizontal distance = equivalent diameter of the convex hull
	features.push_back(make_pair("HorEqDiameter",maxHorConvR));
	//total length
	features.push_back(make_pair("TotalLength",skel.voxset.size()));
	// SRL - total root length/ volume
	features.push_back(make_pair("SRL",skel.voxset.size()/volume));
	// len distr
	features.push_back(make_pair("Length_Distr",computeLengthDistribution(skel,depth,miny)));
	// W/D
	features.push_back(make_pair("W_D_ratio",maxHorConvR/depth));

	//print features
	map<string,double>::iterator it;
	end = clock();
	elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
	//printf("Computing traits... Elapsed time: %.2f\n\n", elapsed);
	//for(it=features.begin(); it!=features.end(); it++)
	//	printf("%s:\t %.2f\n",it->first.c_str(), it->second);
	//remove temporal file
}


/*
* This function computes features estimated with the use of skeleton: depth, total length, #tips, volume, surface area
*/
void MedialCurve::computeSkeletonEstimatedFeatures(vector<pair<string,double>> &features, hash_map<int,float> &dtmap)
{
	//depth compuation - y coordinate
	hash_map<int,float>::iterator it;
	hash_map<int,float>::iterator it2;
	int x,y,z;
	int miny=ysize+1;
	int maxy=-1;
	for(it=voxset.begin(); it!=voxset.end(); it++)
	{
		linind2subind(it->first,x,y,z);
		if(y<miny)
			miny=y;
		if(y>maxy)
			maxy=y;
	}
	int depth=maxy-miny+1;
	int total_length=voxset.size();
	hash_set<int> tips;
	getTipsList(tips);
	int ntips=tips.size();
	//volume and surface area
	float vol=0.0;
	float sa=0.0;
	float av_r=0.0;
	int   nvox=0;
	for(it=voxset.begin(); it!=voxset.end(); it++)
	{
		it2 = dtmap.find(it->first);
		if(it2==dtmap.end())
		{
			printf("Oooops.. computeSkeletonEstimatedFeatures:: no value for the voxel in the distance transform map!");
			continue;
		}
		vol=vol+it->second*it->second;
		sa=sa+it->second;
		nvox++;
	}
	av_r=sa/(float)nvox;
	vol=vol*(float)M_PI;
	sa=sa*2*(float)M_PI;
	//features.insert(make_pair("depth",depth));
	//features.insert(make_pair("total_length",total_length));
	features.push_back(make_pair("number_tips",ntips));
	features.push_back(make_pair("volume",vol));
	features.push_back(make_pair("surface_area",sa));
	features.push_back(make_pair("av_radius",av_r));
}



/**
* Compute Length Distribution root feature. It is defined as the ration of root length
* in the upper 1/3 of the volume to the root length in the lower 2/3 of the volume.
* Root length is approximated by the number of voxels in the skeleton skel.
* Argument miny is the minimum value of y coordinate at which there is a root voxel.
*/
double MedialCurve::computeLengthDistribution(MedialCurve &skel, double depth, double miny)
{
	double len1=0;
	double len2=0;
	int x,y,z;
	int third_depth=(int)miny+(int)(depth/3);
	hash_map<int,float>::iterator it;
	for(it=skel.voxset.begin(); it!=skel.voxset.end(); it++)
	{
		skel.linind2subind(it->first,x,y,z);
		if(y<third_depth)
			len1++;
		else
			len2++;
	}
	if(len2==0) return len1;
	else return len1/len2;
}

/**
* Compute Surface Area root feature. Surface area is
* estimated as a number of boundary voxel faces.
*/
double MedialCurve::computeSurfaceArea_lateral()
{
	double surfarea =0;

	hash_map<int,float>::iterator it;
	int temp;
	for(it=voxset.begin(); it!=voxset.end(); it++)
	{
		int ind=it->first;
		temp=ind+1;
		if(voxset.find(temp)==voxset.end())
			surfarea++;
		temp=ind-1;
		if(voxset.find(temp)==voxset.end())
			surfarea++;
		temp=ind+ysize*xsize;
		if(voxset.find(temp)==voxset.end())
			surfarea++;
		temp=ind-ysize*xsize;
		if(voxset.find(temp)==voxset.end())
			surfarea++;
		//temp=ind+xsize;
		//if(voxset.find(temp)==voxset.end())
		//	surfarea++;
		//temp=ind-xsize;
		//if(voxset.find(temp)==voxset.end())
		//	surfarea++;
	}
	return surfarea;
}

double MedialCurve::computeSurfaceArea()
{
		double surfarea =0;

	hash_map<int,float>::iterator it;
	for(it=voxset.begin(); it!=voxset.end(); it++)
	{
		int ind=it->first;
		for(int k=-1; k<=1; k++)
			for(int j=-1; j<=1; j++)
				for(int i=-1; i<=1; i++)
				{
					if(abs(i)+abs(j)+abs(k)!=1) continue;
					int temp=ind+k*ysize*xsize+j*xsize+i;
					if(voxset.find(temp)==voxset.end())
						surfarea++;
				}
	}
	return surfarea;
}

/**
* This function computes Convex Volume root feature using qhull library.
*/
double MedialCurve::computeConvexVolume()
{
	double convexvolume=0;
	//for optimization get only boundary voxels whithin 6-scheme neightbourhood;
	vector<double> boundvoxs;
	getBoundaryVector(boundvoxs);
	int dim=3;  	              // dimension of points
	int numpoints=boundvoxs.size()/3;            // number of points
	double *pts=(double*)malloc(sizeof(double)*dim*numpoints);
	for(int i=0; i<numpoints*dim; i++)
		pts[i]=boundvoxs[i];
	//coordT *points = &(boundvoxs[0]);           // array of coordinates for each point //
	boolT ismalloc=False;           // True if qhull should free points in qh_freeqhull() or reallocation //
	char flags[]= "qhull Tv FA"; // option flags for qhull, see qh_opt.htm //
	FILE *outfile= fopen("temp.temp","w");//stdout;    // output from qh_produce_output()
	                             //use NULL to skip qh_produce_output() //
	FILE *errfile= stderr;    // error messages from qhull code //
	int exitcode;             // 0 if no error from qhull //
	//facetT *facet;	          // set by FORALLfacets //
	int curlong, totlong;	  // memory remaining after qh_memfreeshort //

	exitcode= qh_new_qhull (dim, numpoints, pts, ismalloc, flags, outfile, errfile);
	convexvolume = qh_qh.totvol;
	//printf("I found volume: %.2f\n", convexvolume);

	qh_freeqhull(!qh_ALL);

	qh_memfreeshort (&curlong, &totlong);
	if (curlong || totlong)
		fprintf (errfile, "qhull internal warning (main): did not free %d bytes of long memory (%d pieces)\n", totlong, curlong);
	free(pts);
	return convexvolume;
}

/**
* Compute sweeping features of the root object:
* Median and Max number of roots among all horizontal slices (medR and maxR),
* Equivalent horizontal radius (maxW), depth.
* The function also returns the value ymin of the minumum y value at which there is a non-empty root voxel.
*/
void MedialCurve::computeSweepingFeatures(double &medR, double &maxR, double &maxW, double &depth, double &miny)
{
	int i,j,k;
	int x,y,z;
	int y0, yfin;
	bool cont=true;
	hash_map<int, float>::iterator it;
	map<int, int>new2old;
	map<int, int>::iterator itn2o;
	int newindex;
	int upperbound;

	// set of variables needed for convex hull
	float maxwidth=0;
	double width;
	int dim=2;  	              // dimension of points
	int numpoints;                // number of points
	double *pts;
	boolT ismalloc=False;           // True if qhull should free points in qh_freeqhull() or reallocation //
	char flags[]= "qhull FA"; // option flags for qhull, see qh_opt.htm //
	FILE *outfile= fopen("temp.temp","w");//stdout;    // output from qh_produce_output() use NULL to skip qh_produce_output() //
	FILE *errfile= stderr;    // error messages from qhull code //
	int exitcode;             // 0 if no error from qhull //
	//facetT *facet;	          // set by FORALLfacets //
	int curlong, totlong;	  // memory remaining after qh_memfreeshort //

	//connected components
	int numcc;
	queue<int> cc;
	vector<int>numcomp(ysize);
	hash_set<int> allvox;
	hash_set<int>::iterator itset;

	//convert the linear indices so that
	//their valye increases as y-coordinate increases
	for(it=voxset.begin(); it!=voxset.end(); it++)
	{
		linind2subind((*it).first,x,y,z);
		newindex=y*xsize*zsize+x*zsize+z;
		new2old.insert(make_pair(newindex,(*it).first));
	}

	j=0;
	//find the coordinates of the top most (min y) non-empty voxel
	linind2subind(new2old.begin()->second,x,y,z);
	y0=y;
	itn2o = new2old.begin();
	//iterate through horizontal sextions
	for(i=y0; i<ysize && itn2o!=new2old.end(); i++)
	{
		//compute the upper bound of the linear indices in the current horizontal section
		upperbound =i*xsize*zsize+xsize*zsize;
		//	printf("Slice: %d\n", i);

		// collect all non-empty voxels of the slice
		while(cont)
		{
			if(itn2o!=new2old.end() && itn2o->first<upperbound)
			{
				allvox.insert(itn2o->second);
				// get vector of coordinates
				itn2o++;
			}
			else
				cont = false;
		}
		// if no voxels in this slice then continue to the next slice
		if(allvox.empty())
		{

			cont = true;
			continue;
		}
		//yfin will contain the y-coordinate of the last non-empty slice
		yfin=i;
		//first compute convex hull of the slice
		// get coordinates of the points
		pts=(double*)malloc(sizeof(double)*allvox.size()*2);
		k=0;
		for(itset=allvox.begin(); itset!=allvox.end(); itset++)
		{
			linind2subind((*itset), x,y,z);
			pts[k++]=x; //pts[k++]=y;
			pts[k++]=z;
		}
		//if only two points - no convex hull - compute simply the distance
		numpoints=allvox.size();
		if(numpoints==2)
			width=(float)sqrt((pts[0]-pts[2])*(pts[0]-pts[2])+(pts[1]-pts[3])*(pts[1]-pts[3]));
		else
		{
			if(numpoints==1)
				width=0;
			else
			{
				exitcode= qh_new_qhull (dim, numpoints, pts, ismalloc, flags, outfile, errfile);
				width = qh_qh.totvol;
				width =2*sqrt((float)width /M_PI);
				qh_freeqhull(!qh_ALL);
				qh_memfreeshort (&curlong, &totlong);
				if (curlong || totlong)
					fprintf (errfile, "qhull internal warning (main): did not free %d bytes of long memory (%d pieces)\n", totlong, curlong);
			}
		}

		free(pts);
		//printf("Width: %.4f\n", width);

		if(width>maxwidth)
			maxwidth=(float)width;

		//now compute # of connected components	in a slice
		vector<set<int>> cc;
		numcc=getConnectedComponents(allvox,cc,26);
		cc.clear();
		numcomp[i-y0]=numcc;
		allvox.clear();
		cont=true;
	}
	//find median and maximum of the number of conncted components
	// maximum is equal to 85th percentile
	maxW=maxwidth;
	depth=yfin-y0+1;
	miny=y0;
	numcomp.resize((unsigned int)depth);
	sort(numcomp.begin(), numcomp.end());
	i=(int)(depth*0.85);
	maxR=numcomp[i];
	if((int)(depth)%2==0)
	{
		medR=(numcomp[(int)(depth/2)]+numcomp[(int)(depth/2)-1])/2;
	}
	else
		medR=numcomp[(int)floor((float)depth/2)];

}

/**
* This function returns a vector of the boundary coordinates:
* v[3*i] =xi, v[3*i+1] =yi, v[3*i+2] =zi,
* Only voxels that have an empty face neighbor are returned
*/
void MedialCurve::getBoundaryVector(vector<double> &v)
{
	int i,j,k;
	int ind=0;
	int count=0;
	hash_map<int,float>::iterator it;

	v.resize(voxset.size()*3,0);

	for(it=voxset.begin(); it!=voxset.end(); it++)
	{
		ind=it->first;
		if(voxset.find(ind-ysize*zsize)==voxset.end()|| voxset.find(ind+ysize*zsize)==voxset.end()
			|| voxset.find(ind-zsize)==voxset.end()|| voxset.find(ind+zsize)==voxset.end()
			|| voxset.find(ind-1)==voxset.end()|| voxset.find(ind+1)==voxset.end())
		{
			linind2subind(ind,i,j,k);
			//printf("%d %d %d\n",i,j,k);
			v[count++]=i; v[count++]=j; v[count++]=k;
		}
	}
	v.resize(count);
}

void MedialCurve::dt(hash_map<int,float> &dtmap)
{
	hash_map<int,float>::iterator it;
	priority_queue<pair<float,int>, vector<pair<float,int>>, greater<pair<float,int>>> q;
	float val;
	float valtemp;
	float sqr2 = (float)sqrt(2.0);
	float sqr3 = (float)sqrt(3.0);
	pair<float,int> p;
	hash_map<int,float> curvals;
	int ind;
	for(it=voxset.begin(); it!=voxset.end(); it++)
	{
		val=FLT_MAX;
		ind=it->first;
		for(int k=-1; k<=1; k++)
			for(int j=-1; j<=1; j++)
				for(int i=-1; i<=1; i++)
				{
					int temp=ind+k*ysize*xsize+j*xsize+i;
					if(voxset.find(temp)==voxset.end())
					{
						if(abs(i)+abs(j)+abs(k)==1)
							valtemp=1;
						else
						{
							if(abs(i)+abs(j)+abs(k)==2)
								valtemp=sqr2;
							else
								valtemp=sqr3;
						}
						if(valtemp<val)
							val=valtemp;
					}
				}
				q.push(make_pair(val, it->first));
				curvals.insert(make_pair(it->first,val));
	}
	float newd;
	while(!q.empty())
	{
		p=q.top();
		q.pop();
		if(dtmap.find(p.second)==dtmap.end())
			dtmap.insert(make_pair(p.second,p.first));
		else
			continue;
		ind=p.second;
		//update the values of neighbors
		for(int k=-1; k<=1; k++)
			for(int j=-1; j<=1; j++)
				for(int i=-1; i<=1; i++)
				{
					int temp=ind+k*ysize*xsize+j*xsize+i;
					if(voxset.find(temp)!=voxset.end())
					{
						it=curvals.find(temp);
						if(abs(i)+abs(j)+abs(k)==1)
							valtemp=1;
						else
						{
							if(abs(i)+abs(j)+abs(k)==2)
								valtemp=sqr2;
							else
								valtemp=sqr3;
						}
						newd=p.first+valtemp;
						if(it->second>newd)
						{
							it->second=newd;
							q.push(make_pair(newd,it->first));
						}
					}
				}
	}
}

//bool greater_pair(pair<float,vector<float>> p1, pair<float,vector<float>> p2)
//{
//	if(p1.first>p2.first) return true;
//	else return false;
//}

//class compare_pair { // simple comparison function
//   public:
//	   bool operator()(const pair<float,vector<float>> p1, const pair<float,vector<float>> p2) { return (p1.first>p2.first); } // returns x>y
//};

void MedialCurve::getTotalLength(hash_map<int,float> &dtmap, float &len, float &vol, float &sa)
{
	float tl=0;
	float sqr2 = (float)sqrt(2.0);
	float sqr3 = (float)sqrt(3.0);
	float valtemp;
	float newd;
	hash_map<int,float>::iterator it;
	hash_map<int,float>::iterator it2;
	hash_map<int,float>::iterator itincr;
	hash_map<int,int>::iterator itp;
	hash_map<int,float> curvals;
	hash_map<int,int> parents;
	hash_map<int,float> increment;
	priority_queue<pair<float,int>, vector<pair<float,int>>, greater<pair<float,int>>> q;
	pair<float,int> p;
	int seed=voxset.begin()->first;
	it=voxset.begin();
	parents.insert(make_pair(seed,-1));
	increment.insert(make_pair(seed,0.0f));
	q.push(make_pair(0.0f,seed));
	curvals.insert(make_pair(seed,0.0f));
	it++;
	hash_set<int>visited;
	float r1, r2;

	while(!q.empty())
	{
		p=q.top();
		q.pop();
		//update neightbors
		if(visited.find(p.second)==visited.end())
		{
			visited.insert(p.second);
			float incr = increment.find(p.second)->second;
			tl=tl+incr;
			// add computation of volume here vol=vol+
			itp=parents.find(p.second);
			it=dtmap.find(p.second);
			it2=dtmap.find(itp->second);
			if(it!=dtmap.end() && it2!=dtmap.end())
			{
				r1=it->second;
				r2=it2->second;
				vol=vol+(0.5f*r1+0.5f*r2)*(0.5f*r1+0.5f*r2)*incr;
				sa=sa+(0.5f*r1+0.5f*r2)*incr;
			}
			else
			{
				printf("getTotalLength: Oops! no value for a voxel in distance map!");
			}

		}
		else
			continue;
		//update the values of neighbors
		for(int k=-1; k<=1; k++)
			for(int j=-1; j<=1; j++)
				for(int i=-1; i<=1; i++)
				{
					int temp=(int)p.second+k*ysize*xsize+j*xsize+i;
					if(voxset.find(temp)!=voxset.end() && visited.find(temp)==visited.end())
					{
						if(abs(i)+abs(j)+abs(k)==1)
							valtemp=1.0f;
						else
						{
							if(abs(i)+abs(j)+abs(k)==2)
								valtemp=sqr2;
							else
								valtemp=sqr3;
						}
						newd=p.first+valtemp;
						it=curvals.find(temp);
						if(it==curvals.end())
						{
							curvals.insert(make_pair(temp,newd));
							parents.insert(make_pair(temp,p.second));
							increment.insert(make_pair(temp,valtemp));
							q.push(make_pair(newd,temp));
						}
						else if(it->second>newd)
						{
							it->second=newd;
							itp=parents.find(temp);
							itp->second=p.second;
							itincr=increment.find(temp);
							itincr->second=valtemp;
							q.push(make_pair(newd,temp));
						}
					}
				}
	}

	vol=vol*(float)M_PI;
	sa=sa*2*(float)M_PI;
}