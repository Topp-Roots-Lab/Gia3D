#include "RootGraph.h"
#include <queue>
#include <hash_set>
#include <map>
#include <float.h>
#include "util.h"
#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif


RootGraph::RootGraph(void)
{
}

RootGraph::~RootGraph(void)
{
}

void RootGraph::init(void)
{
	E.clear();
	v_einc.clear();
	elength.clear();
	e_order.clear();
	v_coord.clear();
	arc_v.clear();
}
/**
* The function creates the minimum spanning tree by default
* (if the argument is true), otherwise if the argument is
* false the function creates maximum spanning tree.
* NB: the tree is created from the original object.
* The minimum spanning tree is constructed based on the edge length.
*/
void RootGraph::getSpanningTree(bool minsp)
{
	multimap<float,int> q;
	stdext::hash_set<int> vset;
	stdext::hash_set<int>::iterator itset1;
	stdext::hash_set<int>::iterator itset2;
	map<int,int>  spanv;
	map<int,int>::iterator hit;
	map<int,int>::iterator itv1;
	map<int,int>::iterator itv2;
	int vc1, vc2;
	int v1, v2;
	set<int> edel;
	set<int>::iterator eit;
	map<int,pair<int,int>>::iterator e;

	map<int,float>::iterator it;
	map<int, coord>::iterator vit;
	//constract a map ordered by edge length
	for(it=elength.begin(); it!=elength.end(); it++)
	{
		e=E.find(it->first);
		//remove self-edge
		if(e->second.first==e->second.second)
			edel.insert(e->first);
		else
			q.insert(make_pair(it->second,it->first));
	}
	//creat a set of vertices, they represent connceted components
	for(vit=v_coord.begin(); vit!=v_coord.end(); vit++)
		vset.insert(vit->first);

	pair<float,int> ep;
	int cc = (int)vset.size();
	int cid=1;
	//while not all edges were considered or there are more than one connected components
	while(!q.empty() && cc!=1)
	{
		//retrieve the max or min edge from the queue
		if(minsp==true){
			ep=*(q.begin());
			q.erase(q.begin());}
		else
		{
			ep=*(--q.end());
			q.erase(--q.end());
		}
		//q.erase(ep);
		e=E.find(ep.second);
		v1=e->second.first;
		v2=e->second.second;

		//try to find the id of the connected components of incident vertices v1 and v2
		itset1=vset.find(v1);
		if(itset1 == vset.end())
		{
			itv1=spanv.find(v1);
			vc1=itv1->second;
		}
		itset2=vset.find(v2);
		if(itset2 == vset.end())
		{
			itv2=spanv.find(v2);
			vc2=itv2->second;
		}
		// if no conncted component was created for v1
		if(itset1!=vset.end())
		{
			//if the two nodes were not connected before to anything, they form a new connected component
			if(itset2!=vset.end())
			{
				spanv.insert(make_pair(v1,cid));
				spanv.insert(make_pair(v2,cid));
				cid++;
				cc--;
				vset.erase(v2);
			}
			//if one of the nodes is already inside another conn component, then the 1st vertex joins it
			else
			{
				spanv.insert(make_pair(v1,vc2));
				cc--;
			}
			vset.erase(v1);
		}
		else
		{
			//now the first vetex has been already connected
			//if the second was not connected to anything, it joins the first
			if(itset2!=vset.end())
			{
				spanv.insert(make_pair(v2,vc1));
				cc--;
				vset.erase(v2);
			}
			//both vertex belong to different connected components, check if components different -> reduce cc, other wise through away the edge
			else
			{
				if(vc1==vc2)
					edel.insert(e->first);
				else
				{
					cc--;
					//rename components
					for(hit=spanv.begin(); hit!=spanv.end(); hit++)
						if(hit->second==vc1)
							hit->second=vc2;
				}
			}

		}
	}
	//remove edges in the list "to delete" from the graph
	//maps to be updated:  E, v_inc, v_arc
	for(eit=edel.begin(); eit!=edel.end(); eit++)
		E.erase(*eit);
	//update vertex incidence map
	updateVertexIncidenceMap();
}

/**
* Create a map between the edges of the graph and average root radius.
* The created map is used later together with construction of
* the max Spanning Tree to maximize the conductivity. Conductivity is
* proportional to R^2 where R is the acerage radius of the edge
* (The average is computed over all voxels of the edge).
*/
void RootGraph::createDistEdgeMap(map<int,float> &cond, hash_map<int,float> &dmap)
{
	map<int,pair<int,int>>::iterator e;
	map<int,set<int>>::iterator ev;
	hash_map<int,float>::iterator itmap;
	set<int>::iterator it;
	int v;
	float avd;
	int nv;

	for(e=E.begin(); e!=E.end();e++)
	{
		avd=0;
		ev=v_einc.find(e->first);
		if(ev!=v_einc.end())
			nv=ev->second.size();
		else
		{
			cond.insert(make_pair(e->first,avd));
			continue;
		}
		for(it=ev->second.begin(); it!=ev->second.end(); it++)
		{
			v=*it;
			itmap=dmap.find(v);
			if(itmap==dmap.end())
				continue;
			else
			avd=avd+itmap->second;
		}
		avd=avd/nv;
		cond.insert(make_pair(e->first,avd));
	}
}

void RootGraph::orientArcs()
{
	map<int, vector<int>>::iterator ait;
	map<int, pair<int,int>>::iterator eit;
	for(ait=arc_v.begin(); ait!=arc_v.end(); ait++)
	{
		eit=E.find(ait->first);
		if(eit->second.first==ait->second[0])
			continue;
		else
		{
			int temp=eit->second.first;
			eit->second.first=eit->second.second;
			eit->second.second=temp;
		}
	}
}

void RootGraph::removeSelfLoops()
{
	map<int,pair<int,int>>::iterator e;
	set<int> edel;
	set<int>::iterator edelit;
	updateVertexIncidenceMap();
	map<int,set<int>>::iterator incit;

	for(e=E.begin(); e!=E.end();e++)
	{
		if(e->second.first==e->second.second)
			edel.insert(e->first);
	}

	for(edelit=edel.begin(); edelit!=edel.end();edelit++)
	{
		e=E.find(*edelit);
		incit=v_einc.find(e->second.first);
		incit->second.erase(*edelit);
		if(incit->second.size()==2)
		{
			//remove the node and merge adjacent arcs
			int e1=*incit->second.begin();
			int e2=*(++incit->second.begin());
			merge_edges(e1, e2);
		}
		E.erase(*edelit);
		arc_v.erase(*edelit);
		elength.erase(*edelit);
	}
}

void RootGraph::merge_edges(int e1, int e2)
{
	map<int,pair<int,int>>::iterator eit1;
	map<int,pair<int,int>>::iterator eit2;
	map<int,vector<int>>::iterator va1;
	map<int,vector<int>>::iterator va2;
	map<int,float>::iterator el;
	map<int,set<int>>::iterator vincit;

	eit1=E.find(e1);
	va1=arc_v.find(e1);
	va2=arc_v.find(e2);
	int v1=eit1->second.first;
	int v2=eit1->second.second;
	eit2=E.find(e2);
	int v3=eit2->second.first;
	int v4=eit2->second.second;
	int vn1, vn2;
	bool res=false;
	vector<int> newarc(0);

	if(v1==v2 || v3==v4)
		return;

	if(v1==v3)
	{
		newarc.insert(newarc.end(),va1->second.rbegin(),va1->second.rend());
		newarc.insert(newarc.end(),++va2->second.begin(),va2->second.end());
		res=true;
	}
	if(v1==v4)
	{
		newarc.insert(newarc.end(),va1->second.rbegin(),va1->second.rend());
		newarc.insert(newarc.end(),++va2->second.rbegin(),va2->second.rend());
		res=true;
	}
	if(v2==v3)
	{
		newarc.insert(newarc.end(),va1->second.begin(),va1->second.end());
		newarc.insert(newarc.end(),++va2->second.begin(),va2->second.end());
		res=true;
	}
	if(v2==v4)
	{
		newarc.insert(newarc.end(),va1->second.begin(),va1->second.end());
		newarc.insert(newarc.end(),++va2->second.rbegin(),va2->second.rend());
		res=true;
	}

	vn1=newarc[0];
	vn2=newarc[newarc.size()-1];
	eit1->second.first=vn1;
	eit1->second.second=vn2;
	va1->second.swap(newarc);

	E.erase(e2);
	float l=getEdgeLength(eit1->first);
	el=elength.find(e1);
	if(el==elength.end())
		elength.insert(make_pair(e1,l));
	else
		el->second=l;
	elength.erase(e2);
	//updateVertexIncidenceMap();
	vincit=v_einc.find(v3);
	if(vincit!=v_einc.end())
		vincit->second.erase(e2);
	vincit=v_einc.find(v4);
	if(vincit!=v_einc.end())
		vincit->second.erase(e2);
	vincit=v_einc.find(vn1);
	if(vincit!=v_einc.end())
		vincit->second.insert(e1);
	vincit=v_einc.find(vn2);
	if(vincit!=v_einc.end())
		vincit->second.insert(e1);
}

/**
* The function creates the minimum spanning tree by default
* (if the argument is true), otherwise if the argument is
* false the function creates maximum spanning tree.
* NB: the tree is created from the original object.
* The minimum spanning tree is constructed based on the edge length.
*/
void RootGraph::getSpanningTree(bool minsp, map<int,float> feature)
{
	multimap<float,int> q;
	stdext::hash_set<int> vset;
	stdext::hash_set<int>::iterator itset1;
	stdext::hash_set<int>::iterator itset2;
	map<int,int>  spanv;
	map<int,int>::iterator hit;
	map<int,int>::iterator itv1;
	map<int,int>::iterator itv2;
	int vc1, vc2;
	int v1, v2;
	set<int> edel;
	set<int>::iterator eit;
	map<int,pair<int,int>>::iterator e;
	map<int,float>::iterator mapit;

	map<int,float>::iterator it;
	map<int, coord>::iterator vit;
	//construct a map ordered by edge length
	for(e=E.begin(); e!=E.end(); e++)
	{
		//remove self-edge
		if(e->second.first==e->second.second)
			edel.insert(e->first);
		else
		{
			mapit=feature.find(e->first);
			if(mapit==feature.end())
			{
				printf("getSpanningTree: No feature is associated with the edge %i\n",e->first);
				continue;
			}
			float fval=mapit->second;
			q.insert(make_pair(fval,e->first));
		}
	}
	//creat a set of vertices, they represent connceted components
	for(vit=v_coord.begin(); vit!=v_coord.end(); vit++)
		vset.insert(vit->first);

	pair<float,int> ep;
	int cc = (int)vset.size();
	int cid=1;
	//while not all edges were considered or there are more than one connected components
	while(!q.empty() && cc!=1)
	{
		//retrieve the max or min edge from the queue
		if(minsp==true){
			ep=*(q.begin());
			q.erase(q.begin());}
		else
		{
			ep=*(--q.end());
			q.erase(--q.end());
		}
		//q.erase(ep);
		e=E.find(ep.second);
		v1=e->second.first;
		v2=e->second.second;

		//try to find the id of the connected components of incident vertices v1 and v2
		itset1=vset.find(v1);
		if(itset1 == vset.end())
		{
			itv1=spanv.find(v1);
			vc1=itv1->second;
		}
		itset2=vset.find(v2);
		if(itset2 == vset.end())
		{
			itv2=spanv.find(v2);
			vc2=itv2->second;
		}
		// if no conncted component was created for v1
		if(itset1!=vset.end())
		{
			//if the two nodes were not connected before to anything, they form a new connected component
			if(itset2!=vset.end())
			{
				spanv.insert(make_pair(v1,cid));
				spanv.insert(make_pair(v2,cid));
				cid++;
				cc--;
				vset.erase(v2);
			}
			//if one of the nodes is already inside another conn component, then the 1st vertex joins it
			else
			{
				spanv.insert(make_pair(v1,vc2));
				cc--;
			}
			vset.erase(v1);
		}
		else
		{
			//now the first vetex has been already connected
			//if the second was not connected to anything, it joins the first
			if(itset2!=vset.end())
			{
				spanv.insert(make_pair(v2,vc1));
				cc--;
				vset.erase(v2);
			}
			//both vertex belong to different connected components, check if components different -> reduce cc, other wise through away the edge
			else
			{
				if(vc1==vc2)
					edel.insert(e->first);
				else
				{
					cc--;
					//rename components
					for(hit=spanv.begin(); hit!=spanv.end(); hit++)
						if(hit->second==vc1)
							hit->second=vc2;
				}
			}

		}
	}
	//remove edges in the list "to delete" from the graph
	//maps to be updated:  E, v_inc, v_arc
	for(eit=edel.begin(); eit!=edel.end(); eit++)
		E.erase(*eit);
	//update vertex incidence map
	updateVertexIncidenceMap();
}

/**
* This function creates the map v_einc between
* vertex indices and a set of id-s of the incidend edges
*/
void RootGraph::updateVertexIncidenceMap()
{
	map<int,pair<int,int>>::iterator eit;
	map<int,set<int>>::iterator vincit;
	v_einc.clear();
	int v1, v2;
	//iterate through edges
	for(eit=E.begin(); eit!=E.end(); eit++)
	{
		v1=eit->second.first;
		v2=eit->second.second;

		//for the first node
		vincit=v_einc.find(v1);
		if(vincit==v_einc.end())
		{
			set<int> inc;
			inc.insert(eit->first);
			v_einc.insert(make_pair(v1,inc));
		}
		else
			vincit->second.insert(eit->first);
		//for the second node
		vincit=v_einc.find(v2);
		if(vincit==v_einc.end())
		{
			set<int> inc;
			inc.insert(eit->first);
			v_einc.insert(make_pair(v2,inc));
		}
		else
			vincit->second.insert(eit->first);
	}
}

/**
* returns the id of the node with the smallest y-value + it should be bif node
*/
int RootGraph::getHighestNode()
{
	map<int, coord>::iterator it;
	map<int, set<int>>::iterator itinc;
	float ymin = FLT_MAX;
	int vid=0;
	for(it=v_coord.begin(); it!=v_coord.end(); it++)
	{
		if(it->second.y<ymin)
		{
			itinc=v_einc.find(it->first);
			if(itinc==v_einc.end()) continue;
			if(itinc->second.size()>2)
			{
				ymin=it->second.y;
				vid=it->first;
			}
		}
	}
	return vid;
}

/**
* returns the id of the node with the biggest y-value + it should be bif node
*/
int RootGraph::getLowestNode()
{
	map<int, coord>::iterator it;
	map<int, set<int>>::iterator itinc;
	float ymax = -FLT_MAX;
	int vid=0;
	for(it=v_coord.begin(); it!=v_coord.end(); it++)
	{
		if(it->second.y>ymax)
		{
			itinc=v_einc.find(it->first);
			if(itinc==v_einc.end()) continue;
			if(itinc->second.size()>2)
			{
				ymax=it->second.y;
				vid=it->first;
			}
		}
	}
	return vid;
}
/**
* find Horton ordering of the network:
* assign values the order to each edge in the graph;
* the values are store in e_order map;
*/
void RootGraph::HortonOrder()
{
	// need to know the source node to start
	int vtop=getLowestNode();
	int vnum = (int)v_coord.size();

	vector<bool> visited(vnum);
	vector<int> vo(vnum);
	vector<vector<int>> ve(vnum);
	for(int i=0; i<vnum; i++)
		ve[i].resize(vnum);
	for(int i=0; i<vnum; i++)
		for(int j=0; j<vnum; j++)
			ve[i][j]=-1;

	//recursive procedure to find Horton order stored in the matrix vo
	findHortonOrder(visited,vtop,vo,ve);

	//transfer edge order from matrix representation to the map e_order
	map<int,pair<int,int>>::iterator eit;
	int v1, v2;
	for(eit=E.begin(); eit!=E.end(); eit++)
	{
		v1=eit->second.first;
		v2=eit->second.second;
		int ho=ve[v1][v2];
		if(ho>=0)
			e_order.insert(make_pair(eit->first,ho));
	}
}

/**
* This is a recursice procedure to find the Horton order of an edge based on its dauter edges.
* The Horton order of pendant edges is 1. If the edges of different orders meet at a bifurcation point,
* the new successive edge will have an order of max of the incoming edges.
* If edges of the same order meet at the biffurcation point,
* the order of the new successive edge will encrease by 1.
*/
int RootGraph::findHortonOrder(vector<bool> &visited,int vid, vector<int> &vo, vector<vector<int>> &eo)
{
	int cur_order;
	set<int> vset;
	set<int>::iterator vit;
	int n_eq_o;
	if(!visited[vid])
	{
		n_eq_o=0;
		cur_order=0;
		visited[vid]=1;
		// find incident nodes
		getIncidentVertices(vset,vid);
		// iterate trough incident vertices which weren't visited and find their order
		for(vit=vset.begin(); vit!=vset.end(); vit++)
		{
			if(visited[*vit]==true) continue;
			int d_order=findHortonOrder(visited,*vit,vo,eo);
			//assign order to the edge based on the order of the incident vertex
			if (d_order>0)
			{
				eo[vid][*vit]=d_order;
				eo[*vit][vid]=d_order;
			}
			//current order is the maximum order of the incident vertices
			if (d_order>cur_order)
				cur_order=d_order;
			else
				// if there are several incident vertices of the same order count their number
				if (cur_order==d_order)
					n_eq_o++;
		}
		// if the number of incident non-visited vertices is  equal
		// to the number of the vertices of the same order,
		// the current order should be increased by 1
		if(vset.size()-1==n_eq_o+1 && cur_order!=0 && vset.size()>2)
			cur_order=cur_order+1;
		cur_order=max(1,cur_order);
		vo[vid]=cur_order;
	}
	else
	{
		cur_order=0;
	}
	return cur_order;
}

/**
* returns a set of the incident vertices to a given vertex with id vid
*/
void RootGraph::getIncidentVertices(set<int> &vset,int vid)
{
	vset.clear();
	map<int,pair<int,int>>::iterator eit;
	map<int, set<int>>::iterator it;
	set<int>::iterator itset;
	it=v_einc.find(vid);
	for(itset=it->second.begin(); itset!=it->second.end(); itset++)
	{
		eit=E.find(*itset);
		if(eit->second.first!=vid)
			vset.insert(eit->second.first);
		else
			vset.insert(eit->second.second);
	}
}

/**
* get the maximum edge order in the network
*/
int RootGraph::getMaxHortonOrder()
{
	int maxOrder=0;
	map<int,int>::iterator it;
	for(it=e_order.begin(); it!=e_order.end(); it++)
		if(it->second>maxOrder) maxOrder=it->second;
	return maxOrder;
}

/**
* This function creates an IV output file with the colored network.
* If the order is assigned to edges, the edges will be colored using heatmap colors
* from blue (low values) to red (high values).
* The output will be written to the file filename.iv which is viewable with ivview.exe viewer
* or any OpenInventor viewer. This format can be easily converted to VRML format as well.
* The arguments are: filename -  the name of the output file;
* xs,ys,zs - width, hight, and depth of the model volume.
* These values are used to convert linear indices to coordinates.
*/
void RootGraph::writeHortonNetworkToIV(string filename, int xs, int ys, int zs)
{
	//first find the maximum order in the network
	int maxOrder=getMaxHortonOrder();


	float x,y,z;
	map<int,pair<int,vector<float>>> vinds;
	//create the map of old to new vertex indices
	map<int,int> old_new;
	map<int,pair<int,vector<float>>>::iterator indit;
	//create the map of new vertex indices + their coordinates
	map<int, coord>::iterator vit;
	map<int, pair<int,int>>::iterator eit;
	int curi=0;
	// collect only verticies incident to edges
	for(eit=E.begin(); eit!=E.end(); eit++)
	{
		vit=v_coord.find(eit->second.first);
		//if this vertex is not in the list - add it to the 2 lists
		if(old_new.find(vit->first)==old_new.end())
		{
			vector<float> coord(3);
			coord[0]=vit->second.x;
			coord[1]=vit->second.y;
			coord[2]=vit->second.z;
			vinds.insert(make_pair(curi,make_pair(vit->first,coord)));
			old_new.insert(make_pair(vit->first,curi));
			curi++;
		}
		//same for the second vertex
		vit=v_coord.find(eit->second.second);
		//if this vertex is not in the list - add it to the 2 lists
		if(old_new.find(vit->first)==old_new.end())
		{
			vector<float> coord(3);
			coord[0]=vit->second.x;
			coord[1]=vit->second.y;
			coord[2]=vit->second.z;
			vinds.insert(make_pair(curi,make_pair(vit->first,coord)));
			old_new.insert(make_pair(vit->first,curi));
			curi++;
		}
	}
	//now add vertices of the arcs of the edges
	map<int,vector<int>>::iterator evit;
	for(eit=E.begin(); eit!=E.end(); eit++)
	{
		evit=arc_v.find(eit->first);
		if(evit==arc_v.end()) continue;
		int vn=evit->second.size();
		for(int i=1; i<vn-1; i++)
		{
			int oldi=evit->second[i];
			linind2subind(oldi,x,y,z,xs,ys,zs);
			vector<float> coord(3);
			coord[0]=x;
			coord[1]=y;
			coord[2]=z;
			vinds.insert(make_pair(curi,make_pair(oldi,coord)));
			old_new.insert(make_pair(oldi,curi));
			curi++;
		}
	}

	string fileout;
	int pp=filename.find('.');
	fileout=filename.substr(0,pp);
	fileout=fileout+"_nw.iv";

	// create the table of colors based on the available order of the network
	vector<vector<float>> colors(maxOrder);
	for(int i=0; i<maxOrder; i++)
	{
		colors[i].resize(3);
		getRGBcolor4Interals(((float)(i+1)/(float)maxOrder),colors[i][0],colors[i][1], colors[i][2]);
	}

	FILE *f = fopen(fileout.c_str(),"w");
	fprintf(f, "#Inventor V2.1 ascii\n Separator {\n");
	fprintf(f, "LightModel { \n model PHONG \n}");
	fprintf(f, "MaterialBinding { \n value PER_VERTEX_INDEXED \n}");



	fprintf(f, "Coordinate3 { \n point [\n");
	for(indit=vinds.begin(); indit!=vinds.end(); indit++)
		fprintf(f, " %.2f %.2f %.2f,\n", indit->second.second[0], indit->second.second[1], indit->second.second[2]);
	fprintf(f, "]\n }\n");

	fprintf(f, "BaseColor { \n rgb [\n");
	for(int i=0; i<maxOrder; i++)
		fprintf(f, " %.2f %.2f %.2f,\n ", colors[i][0], colors[i][1], colors[i][2]);
	fprintf(f, "]\n }\n");
	fprintf(f, "IndexedLineSet { \ncoordIndex [\n");

	map<int,int>::iterator mit;
	map<int,int>::iterator mito;
	int vnew;
	int no;
	vector<int> cind(0);
	for(eit=E.begin(); eit!=E.end(); eit++)
	{
		evit=arc_v.find(eit->first);
		if(evit==arc_v.end())
			printf("Debug:something is wrong!!!");

		mito=e_order.find(evit->first);
		if(mito==e_order.end())
			no=0;
		else
			no=mito->second-1;

		int n=evit->second.size();
		if(n<=1) continue;
		int v0=eit->second.first;
		mit=old_new.find(v0);
		if(mit==old_new.end())
		{
			printf("Debug:something is wrong - first vertex!!!");
			continue;
		}
		vnew=mit->second;
		fprintf(f, " %i, ", vnew);
		cind.push_back(no);
		for(int i=1; i<=n-2;i++ )
		{
			mit=old_new.find(evit->second[i]);
			if(mit==old_new.end())
			{
				printf("Debug:something is wrong - intermediate vertices!!!");
				continue;
			}
			vnew=mit->second;
			fprintf(f, " %i, ", vnew);
			cind.push_back(no);
		}
		int vn=eit->second.second;
		mit=old_new.find(vn);
		if(mit==old_new.end())
		{
			printf("Debug:something is wrong - last vertex!!!");
			continue;
		}
		vnew=mit->second;
		fprintf(f, " %i, ", vnew);
		cind.push_back(no);
		fprintf(f, " -1,\n");
		cind.push_back(-1);
	}
	fprintf(f, "]\n ");


	fprintf(f, "\nmaterialIndex [\n");
	for(int i=0; i<(int)cind.size(); i++)
		fprintf(f, " %i, ", cind[i]);
	fprintf(f, "]\n }\n");

	fprintf(f, "}\n");
	fclose(f);
}



/*
*
*this function is for visualization of the graph with correctly located edge-arcs
*/
void RootGraph::outputFullGraphToIV(string filename)
{
	map<int, coord>::iterator vit;
	map<int, pair<int,int>>::iterator eit;
	map<int, vector<int>>::iterator eait;
	vector<int>::iterator ait;
	vector<int> e_inds(0);
	set<int> nlist;
	set<int>::iterator sit;
	map<int,coord>::iterator mit;

	get_node_list(nlist);
	sit=nlist.begin();
	map<int,int> inds;
	map<int,int>::iterator indit;

	int id=0;
	for(vit=v_coord.begin(); vit!=v_coord.end(); vit++)
	{
		indit=inds.find(vit->first);
		if(indit==inds.end())
		{
			inds.insert(make_pair(vit->first,id));
			id++;
		}
	}

	for(eit=E.begin(); eit!=E.end(); eit++)
	{
		eait=arc_v.find(eit->first);
		if(eait==arc_v.end())
			continue;
		for(vector<int>::iterator vit=eait->second.begin(); vit!=eait->second.end(); vit++)
		{
			indit=inds.find(*vit);
			e_inds.insert(e_inds.end(), indit->second);
		}
		e_inds.insert(e_inds.end(),-1);
	}
////////////////////////
	FILE *f = fopen(filename.c_str(),"w");
	fprintf(f, "#VRML V2.0 utf8\n");
	fprintf(f, "Transform {\n\t children [\n");

	//first node
	mit=v_coord.find(*sit);
	if(mit==v_coord.end())
	{
		printf("Cannot find the node %d! No output.", *sit);
		fclose(f);
		return;
	}
	fprintf(f, "Transform {\n");
	fprintf(f, "\t translation %.3f %.3f %.3f\n", mit->second.x, mit->second.y, mit->second.z);
	fprintf(f, "\t children   DEF Joe Shape { geometry Sphere { radius .7 } }\n");
	fprintf(f, "}\n");
	sit++;

	for(;sit!=nlist.end(); sit++)
	{
		mit=v_coord.find(*sit);
		if(mit==v_coord.end())
			continue;
		fprintf(f, "Transform {\n");
		fprintf(f, "\t translation %.3f %.3f %.3f\n", mit->second.x, mit->second.y, mit->second.z);
		fprintf(f, "\t children USE Joe\n");
		fprintf(f, "}\n");
	}
	fprintf(f, "]\n}\n\n ");

	fprintf(f, "IndexedLineSet {\n");
	fprintf(f, "coord Coordinate {\n point [\n");
	//fprintf(f, "Coordinate3 { \n point [\n");
	for(vit=v_coord.begin(); vit!=v_coord.end(); vit++)
		fprintf(f, " %.2f %.2f %.2f,\n", vit->second.x, vit->second.y, vit->second.z);
	fprintf(f, "]\n }\n");
	//fprintf(f, "IndexedLineSet { \ncoordIndex [\n");
	fprintf(f, "coordIndex [\n");
	for(ait=e_inds.begin(); ait!=e_inds.end(); ait++)
		fprintf(f, " %i, ", *ait);
	fprintf(f, "]\n}\n ");

	fclose(f);

	////////////////////////////////
	/*FILE *f = fopen(filename.c_str(),"w");
	fprintf(f, "#Inventor V2.1 ascii\n Separator {\n");
	fprintf(f, "Coordinate3 { \n point [\n");
	for(vit=v_coord.begin(); vit!=v_coord.end(); vit++)
		fprintf(f, " %.2f %.2f %.2f,\n", vit->second.x, vit->second.y, vit->second.z);
	fprintf(f, "]\n }\n");

	fprintf(f, "IndexedLineSet { \ncoordIndex [\n");
	for(ait=e_inds.begin(); ait!=e_inds.end(); ait++)
		fprintf(f, " %i, ", *ait);
	fprintf(f, "]\n}\n ");

	fprintf(f, "}\n");
	fclose(f);	*/
}

/**
* outputs the graph as adjacency matrix in txt
*/
void RootGraph::graph_adjacency_output(string filename)
{

	hash_map<int,int> indexmap;
	hash_map<int,int>::iterator it;
	pair< hash_map<int,int>::iterator, bool > res;
	map<int,pair<int,int>>::iterator eit;
	int ind=0;
	for(eit=E.begin(); eit!=E.end(); eit++)
	{
		if(eit->second.first==eit->second.second)
		{
			//printf("Self edge for the node %d.\n", eit->second.first);
			continue;
		}

		res=indexmap.insert(make_pair(eit->second.first,ind));
		if(res.second==true)
			ind++;
		res=indexmap.insert(make_pair(eit->second.second,ind));
		if(res.second==true)
			ind++;
	}
	int n=indexmap.size();

	vector<vector<bool>> adj(n);
	for(int i=0; i<n; i++)
		adj[i].resize(n);

	int v1,v2;
	for(eit=E.begin(); eit!=E.end(); eit++)
	{
		if(eit->second.first==eit->second.second)
			continue;

		it=indexmap.find(eit->second.first);
		if(it==indexmap.end())
			continue;
		v1=it->second;

		it=indexmap.find(eit->second.second);
		if(it==indexmap.end())
			continue;
		v2=it->second;
		//if(v1==v2)
			//printf("Self edge for the node %d\n.", v1);
		adj[v1][v2]=1;
		adj[v2][v1]=1;
	}

	FILE * f = fopen(filename.c_str(), "w");
	fprintf(f, "P1\n");
	fprintf(f, "%d %d\n", n,n);
	for(int i=0; i<n; i++)
	{
		for(int j=0; j<n; j++)
			fprintf(f, "%s ", (adj[i][j])?"1":"0");

		/*if(adj[i][j]==1)
				fprintf(f," 1 ");
			else
				fprintf(f," 0 ");*/
		fprintf(f,"\n");
	}

	fclose(f);
}


/*
* returns the adjacency matrix
*/
//void RootGraph::get_adjacency(vector<vector<float>> &a, map<int,int> &indexmap)
//{
//	map<int,int>::iterator it;
//	pair< hash_map<int,int>::iterator, bool > res;
//	map<int,pair<int,int>>::iterator eit;
//	int ind=0;
//	for(eit=E.begin(); eit!=E.end(); eit++)
//	{
//		if(eit->second.first==eit->second.second)
//		{
//			//printf("Self edge for the node %d.\n", eit->second.first);
//			continue;
//		}
//
//		res=indexmap.insert(make_pair(eit->second.first,ind));
//		if(res.second==true)
//			ind++;
//		res=indexmap.insert(make_pair(eit->second.second,ind));
//		if(res.second==true)
//			ind++;
//	}
//	int n=indexmap.size();
//
//	a.resize(n);
//	for(int i=0; i<n; i++)
//		a[i].resize(n);
//
//	int v1,v2;
//	map<int,float>::iterator elenit;
//	for(eit=E.begin(); eit!=E.end(); eit++)
//	{
//		if(eit->second.first==eit->second.second)
//			continue;
//
//		it=indexmap.find(eit->second.first);
//		if(it==indexmap.end())
//			continue;
//		v1=it->second;
//
//		it=indexmap.find(eit->second.second);
//		if(it==indexmap.end())
//			continue;
//		v2=it->second;
//		//if(v1==v2)
//			//printf("Self edge for the node %d\n.", v1);
//		elenit=elength.find(eit->first);
//		if(elenit==elength.end())
//			continue;
//		a[v1][v2]=elenit->second;
//		a[v2][v1]=elenit->second;
//	}
//}

//void RootGraph::graph_edge_description_output(string filename)
//{
//	//map of current indices to new indices [0,n]
//	map<int,int> inds;
//	map<int,int>::iterator indit;
//	int id=0;
//
//	map<int, float>::iterator elen;
//	map<int, pair<int,int>>::iterator eit;
//	int id1, id2;
//	int ne=E.size();
//	vector<int> v1(ne);
//	vector<int> v2(ne);
//	vector<float> len(ne);
//	int i;
//
//	for(eit=E.begin(), i=0; eit!=E.end(); eit++, i++)
//	{
//		if(inds.find(eit->second.first)==inds.end())
//		{
//			inds.insert(make_pair(eit->second.first,id));
//			id1=id;
//			id++;
//		}
//		if(inds.find(eit->second.second)==inds.end())
//		{
//			inds.insert(make_pair(eit->second.second,id));
//			id2=id;
//			id++;
//		}
//
//		elen=elength.find(eit->first);
//		if(elen==elength.end())
//		{
//			len[i]=-1;
//			continue;
//		}
//		v1[i]=id1;
//		v2[i]=id1;
//		len[i]=elen->second;
//	}
//
//	FILE * f = fopen(filename.c_str(), "w");
//	for(indit=inds.begin(); indit!=inds.end(); indit++)
//		fprintf(f, "%d %d\n", indit->first, indit->second);
//
//	fprintf(f, "\n");
//
//	for(i=0; i<ne; i++)
//	{
//		if(len[i]==-1)
//			continue;
//		fprintf(f, "%d %d %.3f\n", v1[i], v2[i], len[i]);
//	}
//	fclose(f);
//}


void RootGraph::graph_description_output(string filename, map<int,vector<float>> &descr)
{
	map<int,vector<float>>::iterator dit;
	vector<float>::iterator vit;

	//map of current indices to new indices [0,n]
	map<int,int> inds;
	map<int,int>::iterator indit;
	map<int,coord>::iterator vcit;
	int id=0;
	for(vcit=v_coord.begin(); vcit!=v_coord.end(); vcit++)
	{
		inds.insert(make_pair(vcit->first,id));
		id++;
	}

	FILE * f = fopen(filename.c_str(), "w");

	fprintf(f, "%d\n",descr.size());
	fprintf(f, "%d\n",descr.begin()->second.size());
	for(dit=descr.begin(); dit!=descr.end(); dit++)
	{
		indit=inds.find(dit->first);
		fprintf(f, "%d\t",indit->second);
		for(vit=dit->second.begin(); vit!=dit->second.end(); vit++)
			fprintf(f, "%.3f\t",*vit);
		fprintf(f, "\n");
	}
	fprintf(f, "\n");
	map<int, float>::iterator elen;
	map<int, pair<int,int>>::iterator eit;
	int id1, id2;

	for(eit=E.begin(); eit!=E.end(); eit++)
	{
		if(descr.find(eit->second.first)==descr.end())
			continue;
		if(descr.find(eit->second.second)==descr.end())
			continue;
		elen=elength.find(eit->first);
		if(elen==elength.end())
			continue;
		indit=inds.find(eit->second.first);
		id1=indit->second;
		indit=inds.find(eit->second.second);
		id2=indit->second;
		fprintf(f, "%d %d %.3f\n", id1, id2, elen->second);
	}
	fclose(f);
}

/**
*
* This function with smooth the edges of the graph
* by moving each of the intermediate nodes to a new
* position defined by the average of the neighbors.
*/
void RootGraph::smoothEdges(int iter)
{
	map<int, coord> sm_arc;
	map<int,pair<int,int>>::iterator eit;
	map<int, vector<int>>::iterator ea_it;
	map<int, coord>::iterator cit;
	float tx1, ty1, tz1;
	float tx2, ty2, tz2;
	map<int,coord>::iterator tcit;

	//map<int, vector<int>> arc_v; //map between edges and sequence of voxels
	for(eit=E.begin(); eit!=E.end(); eit++)
	{
		if(eit->second.first==eit->second.second)
		{
			//printf("Self edge for the node %d.\n", eit->second.first);
			continue;
		}
		ea_it=arc_v.find(eit->first);
		if(ea_it==arc_v.end() || ea_it->second.size()==2)
			continue;
		for(int i=1; i<(int)ea_it->second.size()-1; i++)
		{
			coord c;
			cit=v_coord.find(ea_it->second[i-1]);
			tx1=cit->second.x;
			ty1=cit->second.y;
			tz1=cit->second.z;
			cit=v_coord.find(ea_it->second[i+1]);
			tx2=cit->second.x;
			ty2=cit->second.y;
			tz2=cit->second.z;
			c.x=(tx1+tx2)/2;
			c.y=(ty1+ty2)/2;
			c.z=(tz1+tz2)/2;
			sm_arc.insert(make_pair(ea_it->second[i],c));
		}
	}
	//insert in sm_arc the remaining nodes from v_coord
	for(cit=v_coord.begin(); cit!=v_coord.end(); cit++)
		sm_arc.insert(make_pair(cit->first,cit->second));

	map<int,coord> prev_map;
	prev_map.swap(sm_arc);

	for(int j=2; j<=iter; j++)
	{

		for(eit=E.begin(); eit!=E.end(); eit++)
		{
			if(eit->second.first==eit->second.second)
			{
				//printf("Self edge for the node %d.\n", eit->second.first);
				continue;
			}
			ea_it=arc_v.find(eit->first);
			if(ea_it==arc_v.end() || ea_it->second.size()==2)
				continue;
			for(int i=1; i<(int)ea_it->second.size()-1; i++)
			{
				coord c;
				cit=prev_map.find(ea_it->second[i-1]);
				tx1=cit->second.x; ty1=cit->second.y; tz1=cit->second.z;
				cit=prev_map.find(ea_it->second[i+1]);
				tx2=cit->second.x; ty2=cit->second.y; tz2=cit->second.z;
				c.x=(tx1+tx2)/2;
				c.y=(ty1+ty2)/2;
				c.z=(tz1+tz2)/2;
				sm_arc.insert(make_pair(ea_it->second[i],c));
			}
		}

		for(tcit=sm_arc.begin(); tcit!=sm_arc.end(); tcit++)
		{
			cit=prev_map.find(tcit->first);
			cit->second=tcit->second;
		}
		sm_arc.clear();

	}

	v_coord.swap(prev_map);
	////update values in the v_coord map
	//for(tcit=sm_arc.begin(); tcit!=sm_arc.end(); tcit++)
	//{
	//	cit=v_coord.find(tcit->first);
	//	cit->second=tcit->second;
	//}
}

float RootGraph::getEdgeLength(int eid)
{
	// if map of smoothed edges is empty than take original coordinates
	map<int,vector<int>>::iterator ea_it;
	map<int,pair<int,int>>::iterator eit;
	map<int, coord>::iterator vit;
	float tx1, ty1, tz1;
	float tx2, ty2, tz2;
	float l=0;
	ea_it=arc_v.find(eid);
	eit=E.find(eid);
	if(eit==E.end())
		return -1;
	if(ea_it==arc_v.end() || ea_it->second.size()<=2)
	{
		//if no intemediate nodes compute the distance between two end vertices
		vit= v_coord.find(eit->second.first);
		tx1=vit->second.x;
		ty1=vit->second.y;
		tz1=vit->second.z;
		vit= v_coord.find(eit->second.second);
		tx2=vit->second.x;
		ty2=vit->second.y;
		tz2=vit->second.z;
		return eucl_dist(tx1, ty1, tz1, tx2, ty2, tz2);
	}
	else
	{
		vit= v_coord.find(ea_it->second[0]);
		tx1=vit->second.x;
		ty1=vit->second.y;
		tz1=vit->second.z;

		for(int i=1; i<(int)ea_it->second.size(); i++)
		{
			vit= v_coord.find(ea_it->second[i]);
			tx2=vit->second.x;
			ty2=vit->second.y;
			tz2=vit->second.z;

			l=l+eucl_dist(tx1, ty1, tz1, tx2, ty2, tz2);

			tx1=tx2;
			ty1=ty2;
			tz1=tz2;
		}
		return l;
	}
}
/**
*
This function computes radial geodesic shape descriptors for each bifurcation node.
The descriptor represent the distribution of the geodesic distance around the node
included in the sperical neighborhood. the maximum value of the sperical
neighborhood as well as the number of bins is given,

    *
  * | *
*   o   *  geod_dist(r<1)=1+1=2;  geod_dist(1<r<2)=0.4;  descriptor = {2,0.4};
  *  \*
    *
*/
void RootGraph::radial_geod_distr_per_bifurcation(map<int,vector<float>> &descr, float maxval, int nbin)
{
	set<int> blist;
	set<int>::iterator it;

	get_bif_node_list(blist);

	set<int> nb;
	set<int>::iterator nbit;

	map<int, set<int>>::iterator incit;
	map<int,pair<int,int>>::iterator eit;
	map<int, vector<int>>::iterator arc;
	map<int,coord>::iterator vcoord;
	set<int>::iterator sit;

	float tx,ty,tz;
	float x,y,z;
	int prevbin;
	float prevx,prevy,prevz;
	vector<float> descr_loc(nbin);
	float step=maxval/(float)nbin;
	set<int> e_visited;
	float len_r;
	float len;
	bool flag=0;
	int eid;
	float x1,y1,z1;
	float x2,y2,z2;
	int istart, ifin, idelta, coef;

	//update edge length map
	updateEdgeLengthMap();

	for(it=blist.begin(); it!=blist.end(); it++)
	{
		//printf("%d\n",*it);
	/*	if(*it==42)
			printf("Debug stop\n");*/

		vcoord=v_coord.find(*it);
		x=vcoord->second.x;
		y=vcoord->second.y;
		z=vcoord->second.z;
		prevx=x; prevy=y; prevz=z;
		//get all incident edges
		incit=v_einc.find(*it);
		nb.insert(incit->second.begin(), incit->second.end());
		e_visited.clear();

		while(!nb.empty())
		{
			//pick an edge from the list of adjacent edges
			//walk along the edge and compute geodesic distance
			//contribution to different bins until a voxel in
			//the edge is outside the max bin radius
			eid=*nb.begin();
			/*if(*nb.begin()==49)
				printf("Debug stop\n");*/
			nb.erase(eid);
			e_visited.insert(eid);
			arc=arc_v.find(eid);
			flag=0;

			vcoord=v_coord.find(arc->second[0]);
			x1=vcoord->second.x;
			y1=vcoord->second.y;
			z1=vcoord->second.z;
			vcoord=v_coord.find(arc->second[arc->second.size()-1]);
			x2=vcoord->second.x;
			y2=vcoord->second.y;
			z2=vcoord->second.z;
			if(eucl_dist(x,y,z,x1,y1,z1)<=eucl_dist(x,y,z,x2,y2,z2))
			{
				prevx=x1;
				prevy=y1;
				prevz=z1;
				istart=0;
				ifin=(int)arc->second.size()-1;
				idelta=1;
				coef=1;
			}
			else
			{
				prevx=x2;
				prevy=y2;
				prevz=z2;
				istart=(int)arc->second.size()-1;
				ifin=0;
				idelta=-1;
				coef=-1;
			}
			len_r=eucl_dist(x,y,z,prevx,prevy,prevz);
			if(len_r>maxval)
				continue;
			prevbin= (int)floor(len_r/step);

			for(int i=istart+idelta; i*coef<=coef*ifin; i=i+idelta)
			{
				vcoord=v_coord.find(arc->second[i]);
				tx=vcoord->second.x;
				ty=vcoord->second.y;
				tz=vcoord->second.z;
				len_r=eucl_dist(tx,ty,tz,x,y,z);
				len=eucl_dist(tx,ty,tz,prevx,prevy,prevz);
				if(len_r>=maxval)
				{
					flag=1;
					descr_loc[prevbin] = descr_loc[prevbin] + 0.5*len;
					break;
				}
				int bini = (int)floor(len_r/step);
				if(bini==prevbin)
					descr_loc[bini] = descr_loc[bini] + len;
				else
				{
					descr_loc[prevbin] = descr_loc[prevbin] + 0.5*len;
					descr_loc[bini] = descr_loc[bini] + 0.5*len;
					prevbin=bini;
				}
				prevx=tx; prevy=ty; prevz=tz;
			}
			if(flag==1)
				continue;
			//add all incident edges into nb list if not visited;
			eit=E.find(eid);
			incit=v_einc.find(eit->second.first);
			for(sit=incit->second.begin(); sit!=incit->second.end(); sit++)
			{
				if(e_visited.find(*sit)==e_visited.end())
					nb.insert(*sit);
			}
			incit=v_einc.find(eit->second.second);
			for(sit=incit->second.begin(); sit!=incit->second.end(); sit++)
			{
				if(e_visited.find(*sit)==e_visited.end())
					nb.insert(*sit);
			}

		}
		descr.insert(make_pair(*it, descr_loc));
		descr_loc.assign(nbin,0);
	}
}
/**
* Updates the map elength of the length of graph edges
*/
void RootGraph::updateEdgeLengthMap()
{
	elength.clear();
	map<int,pair<int,int>>::iterator eit;
	for(eit=E.begin(); eit!=E.end(); eit++)
	{
		float len = getEdgeLength(eit->first);
		elength.insert(make_pair(eit->first,len));
	}
}


/**
*
This function collects all bifurcation nodes in the list blist
*/
void RootGraph::get_bif_node_list(set<int> &blist)
{
	updateVertexIncidenceMap();
	map<int,set<int>>::iterator it;
	for(it=v_einc.begin(); it!=v_einc.end(); it++)
		if(it->second.size()>2)
			blist.insert(it->first);
}

/**
*
This function collects all bifurcation nodes in the list blist
*/
void RootGraph::get_node_list(set<int> &nlist)
{
	nlist.clear();
	updateVertexIncidenceMap();
	map<int,set<int>>::iterator it;
	for(it=v_einc.begin(); it!=v_einc.end(); it++)
		if(it->second.size()==1 || it->second.size()>2)
			nlist.insert(it->first);
}

/**
*
This function returns the number of nodes in the graph: bif nodes and tips
*/
int RootGraph::get_node_number()
{
	int n=0;
	updateVertexIncidenceMap();
	map<int,set<int>>::iterator it;
	for(it=v_einc.begin(); it!=v_einc.end(); it++)
		if(it->second.size()==1 || it->second.size()>2)
			n++;
	return n;
}

/**
* This function constructs the map between the edes
* and the list of incident edges. The curvature
* (angle between the edges) is stored together
* with each incident edge
*/
void RootGraph::curveEdgeMap(map<int,multimap<float,int>> &eemap)
{
	map<int,pair<int,int>>::iterator eit;
	set<int> einc;
	set<int>::iterator eit2;
	int v1,v2;
	map<int,set<int>>::iterator vincit;
	float a;
	multimap<float,int> emap;
	updateVertexIncidenceMap();

	for(eit=E.begin(); eit!=E.end(); eit++)
	{
		einc.clear();
		v1=eit->second.first;
		vincit=v_einc.find(v1);
		if(vincit!=v_einc.end())
			einc.insert(vincit->second.begin(), vincit->second.end());
		v2=eit->second.second;
		vincit=v_einc.find(v2);
		if(vincit!=v_einc.end())
			einc.insert(vincit->second.begin(), vincit->second.end());
		einc.erase(eit->first);
		emap.clear();
		for(eit2=einc.begin(); eit2!=einc.end(); eit2++)
		{
			a=getAngle(eit->first, *eit2);
			if(a==1000) continue;
			a=(float)abs(a-M_PI);
			emap.insert(make_pair(a,*eit2));
		}
		if(!emap.empty())
			eemap.insert(make_pair(eit->first,emap));
	}
}

/**
Computes an angle between two edges. The function
assumes that the wo edges share a vertex
(the angle at which is computed).
If not so, it returns the value of 1000.
*/
float RootGraph::getAngle(int e1, int e2)
{
	float a=1000;
	map<int,pair<int,int>>::iterator eit1;
	map<int,pair<int,int>>::iterator eit2;
	map<int, coord>::iterator vit;
	eit1=E.find(e1);
	if(eit1==E.end()) return a;
	eit2=E.find(e2);
	if(eit2==E.end()) return a;

	int v1,v2,vc=-1;
	int v11,v12,v21,v22;
	v11=eit1->second.first;
	v12=eit1->second.second;
	v21=eit2->second.first;
	v22=eit2->second.second;
	//try to find a common vertex
	getEndVertices(e1,e2,v1,v2,vc);
	if(vc==-1) return a;
	float x1,x2,y1,y2,z1,z2,xc,yc,zc;
	//vector coordinates
	float vx1,vx2,vy1,vy2,vz1,vz2;
	vit=v_coord.find(vc);
	if(vit==v_coord.end()) return a;
	xc=vit->second.x; yc=vit->second.y; zc=vit->second.z;
	vit=v_coord.find(v1);
	if(vit==v_coord.end()) return a;
	x1=vit->second.x; y1=vit->second.y; z1=vit->second.z;
	vit=v_coord.find(v2);
	if(vit==v_coord.end()) return a;
	x2=vit->second.x; y2=vit->second.y; z2=vit->second.z;
	vx1=x1-xc; vy1=y1-yc; vz1=z1-zc;
	vx2=x2-xc; vy2=y2-yc; vz2=z2-zc;
	//compute and angle between two vectors as acos(a.b/(||a||*||b||))
	a=(float)acos((vx1*vx2+vy1*vy2+vz1*vz2)/((float)sqrt((vx1*vx1+vy1*vy1+vz1*vz1)*(vx1*vx1+vy1*vy1+vz1*vz1))));
	return a;
}
/**
* this finction returns distinct vertices of the edges v1 and v2
* as well as the shared vertex vc. If the edges do not share a vertex,
* the value -1 is assigned to v1, v2, and vc.
*/
void RootGraph::getEndVertices(int e1, int e2, int &v1, int &v2, int &vc)
{
	v1=-1; v2=-1;
	map<int,pair<int,int>>::iterator eit1;
	map<int,pair<int,int>>::iterator eit2;
	eit1=E.find(e1);
	if(eit1==E.end()) return;
	eit2=E.find(e2);
	if(eit2==E.end()) return;

	int v11,v12,v21,v22;
	v11=eit1->second.first;
	v12=eit1->second.second;
	v21=eit2->second.first;
	v22=eit2->second.second;
	//try to find a common vertex
	if(v11==v21)
	{
		vc=v11;
		v1=v12;
		v2=v22;
		return;
	}
	if(v12==v21)
	{
		vc=v12;
		v1=v11;
		v2=v22;
		return;
	}
	if(v11==v22)
	{
		vc=v11;
		v1=v12;
		v2=v21;
		return;
	}
	if(v12==v22)
	{
		vc=v12;
		v1=v11;
		v2=v21;
		return;
	}
}

/**
* This function constructs the minimum spamming tree in breadth-first search manner.
* The process starts by choosing an initial edge (here longest edge).
* All incident edges go to the queue and an incident edge which
* forms the smallest angle with the current edge is chosen to be
* the new current edge. All connected vertices are pooled together.
* If a chosen edge connects vertices which have been already pooled,
* this edge will be deleted. The process continues until no more edges are in the queue.
*/
void RootGraph::getMinCurvSpanningTreeBreadthFirstSearch()
{
	map<int,multimap<float,int>> feature;
	map<int,multimap<float,int>>::iterator fit;
	multimap<float,int>::iterator eeit;

	set<int> vpool;
	int e;
	map<int,pair<int,int>>::iterator eit;
	multimap<float,int>  ee;
	set<int> todel;
	set<int>::iterator delit;
	hash_set<int> visited;

	curveEdgeMap(feature);
	e=getLongestEdge();
	ee.insert(make_pair(1.f,e));
	while(!ee.empty())
	{
		e=ee.begin()->second;
		ee.erase(ee.begin());
		if(visited.find(e)!=visited.end())
			continue;
		visited.insert(e);
		eit=E.find(e);
		if(eit==E.end()) continue;

		//if the edge creates a loop (two vertices are already connected)
		if(vpool.find(eit->second.first)!=vpool.end() && vpool.find(eit->second.second)!=vpool.end())
		{todel.insert(e); continue;}
		//else connect the vertices by the current edge
		vpool.insert(eit->second.first);
		vpool.insert(eit->second.second);
		fit=feature.find(e);
		if(fit==feature.end()) continue;
		for(eeit=fit->second.begin(); eeit!=fit->second.end(); eeit++)
		{
			if(visited.find(eeit->second)==visited.end())
				ee.insert(*eeit);
		}
	}

	//remove edges in the list "to delete" from the graph
	//maps to be updated:  E, v_inc, v_arc
	for(delit=todel.begin(); delit!=todel.end(); delit++)
		E.erase(*delit);
	//update vertex incidence map
	updateVertexIncidenceMap();
}

/**
* This function constructs a map cee for incident pairs of edges
* prioritiezed by the angle between them. It is just a different
* representation of what is computed in curveEdgeMap
*/
void RootGraph::getCurvEdgePairMap(map<int,multimap<float,int>> &feature, multimap<float,pair<int,int>> &cee)
{
	map<int,multimap<float,int>>::iterator fit;
	multimap<float,int>::iterator it;
	hash_map<int,hash_set<int>> visited;
	hash_map<int,hash_set<int>>::iterator vit;
	hash_map<int,hash_set<int>>::iterator vit2;
	int e1, e2;
	for(fit=feature.begin(); fit!=feature.end(); fit++)
	{
		e1=fit->first;
		for(it=fit->second.begin(); it!=fit->second.end(); it++)
		{
			e2=it->second;
			//check if this pair of edges was alredy considered
			if(visited.find(e1)==visited.end() && visited.find(e2)==visited.end())
			{
				cee.insert(make_pair(it->first,make_pair(e1,e2)));
				hash_set<int> newset;
				newset.insert(e2);
				visited.insert(make_pair(e1,newset));
				continue;
			}
			vit=visited.find(e1);
			vit2=visited.find(e2);
			if((vit!=visited.end() && vit->second.find(e2)==vit->second.end()) || (vit2!=visited.end() && vit2->second.find(e1)==vit2->second.end()) )
			{
				cee.insert(make_pair(it->first,make_pair(e1,e2)));
				if(vit!=visited.end())
					vit->second.insert(e2);
				else vit2->second.insert(e1);
			}
		}
	}
}

/**
* This function computes the minimum spanning
* tree of the graph. First all vertices are
* considered as separate connceted components.
* Pairs of incident edges are prioritized based
* on how small is the angle between them.
* The smallest angle is chosen first and adjacent
* vertices are collapsed into the same connected
* component. If an edge connects two vertices
* belonging to the same connected component,
* then this edge will be deleted.
*/
void RootGraph::getMinCurvSpanningTree()
{
	map<int,multimap<float,int>> feature;
	multimap<float,pair<int,int>> cee;
	multimap<float,pair<int,int>>::iterator ceeit;
	int e1,e2;
	int e1n, e2n;
	int v1,v2,vc,ve1, ve2;
	int c1,c2,c3;
	map<int,int> spanv;
	map<int,int>::iterator itcc;

	set<int> todel;
	set<int>::iterator delit;
	hash_set<int> visited;

	curveEdgeMap(feature);
	getCurvEdgePairMap(feature, cee);
	int cid=1;
	while(!cee.empty())
	{
		v1=-1;
		ceeit=cee.begin();
		e1=ceeit->second.first;
		e2=ceeit->second.second;
		cee.erase(cee.begin());
		getEndVertices(e1,e2,v1,v2,vc);
		if(v1==-1) continue;

		//if the edge creates a loop (two vertices are already connected)
		itcc=spanv.find(v1);
		if(itcc!=spanv.end())
			c1=itcc->second;
		else
			c1=-1;
		itcc=spanv.find(v2);
		if(itcc!=spanv.end())
			c2=itcc->second;
		else
			c2=-1;
		itcc=spanv.find(vc);
		if(itcc!=spanv.end())
			c3=itcc->second;
		else
			c3=-1;
		ve1=E.find(e1)->second.first;
		ve2=E.find(e1)->second.second;
		if(v1==ve1 || v1==ve2)
		{
			e1n=e1;
			e2n=e2;
		}
		else
		{
			e1n=e2;
			e2n=e1;
		}

		// if no conncted component was created for v1
		if(visited.find(e1n)==visited.end())
		{
			if(c1==-1)
			{

				//if the two nodes were not connected before to anything, they form a new connected component
				if(c3==-1)
				{
					spanv.insert(make_pair(v1,cid));
					spanv.insert(make_pair(vc,cid));
					c1=cid;
					c3=cid;
					cid++;
				}
				//if one of the nodes is already inside another conn component, then the 1st vertex joins it
				else
				{
					spanv.insert(make_pair(v1,c3));
					c1=c3;
				}//end if(c3==-1)
				visited.insert(e1n);
			}
			else
			{
				//now the first vetex has been already connected
				//if the second was not connected to anything, it joins the first
				if(c3==-1)
				{
					spanv.insert(make_pair(vc,c1));
					c3=c1;
					visited.insert(e1n);
				}
				//both vertex belong to different connected components, check if components different -> reduce cc, other wise through away the edge
				else
				{
					if(c1==c3)
					{
						todel.insert(e1n);
					}
					else
					{
						visited.insert(e1n);
						//rename components
						for(itcc=spanv.begin(); itcc!=spanv.end(); itcc++)
							if(itcc->second==c1)
								itcc->second=c3;
						c1=c3;
					}
				}//end else
			}//end if(c1==-1)
		}//end if(visited.find(e1n)==visited.end())

		//same but for the vertices vc and v2
		// if no conncted component was created for v1
		if(visited.find(e2n)==visited.end())
		{
			if(c2==-1)
			{
				//if the two nodes were not connected before to anything, they form a new connected component
				spanv.insert(make_pair(v2,c3));
				c2=c3;
				visited.insert(e2n);
			}
			else
			{
				//both vertex belong to different connected components, check if components different -> reduce cc, other wise through away the edge
				if(c2==c3)
				{
					todel.insert(e2n);
				}
				else
				{
					visited.insert(e2n);
					//rename components
					for(itcc=spanv.begin(); itcc!=spanv.end(); itcc++)
						if(itcc->second==c2)
							itcc->second=c3;
				}
			}//end if(c2==-1)
		}
	}//end while

	//remove edges in the list "to delete" from the graph
	//maps to be updated:  E, v_inc, v_arc
	for(delit=todel.begin(); delit!=todel.end(); delit++)
		E.erase(*delit);

	//update vertex incidence map
	updateVertexIncidenceMap();
}

/*
* Returns the id of the longest edge
*/
int RootGraph::getLongestEdge()
{
	map<int,float>::iterator el;
	float l=0;
	int eid;

	for(el=elength.begin();el!=elength.end(); el++)
	{
		if(el->second>l)
		{
			l=el->second;
			eid=el->first;
		}
	}
	return eid;
}