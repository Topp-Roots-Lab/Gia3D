//
// thinvox, a binary voxel thinning program
// Copyright (c) 2004-2007 by Patrick Min, patrick.n.min "at" gmail "dot" com
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// 
//
// $Id: PalagyiFilter.cc,v 1.5 2006/02/18 11:15:43 min Exp min $
//
// comment: functions apply_fast, applyWdist, apply_template_set_fast, apply_template_set_std_fast, and
// additional_erosion were added by Olga Symonova (olga.symonova "at" gmail "dot" com) in 2010-2011.

#include "PalagyiFilter.h"
#include <iostream>
#include <vector>
#include <math.h>

// 2012-09-20
//
// Vladimir Popov added this include, when adjusting this code to Linux
//
//#include <hash_set>

// 2012-09-20
//
// Vladimir Popov changed (if matches were found) below, when adjusting this code to Linux
//
// >> substituted by > >  in all generic types (templates)   
// for instance,map<int,pair<int,int>>  changed to map<int,pair<int,int> >
//
// stdext:: substituted by __gnu_cxx::
//
// hash_map substituted by __gnu_cxx::hash_map
//
// hash_set substituted by __gnu_cxx::hash_set


using namespace std;
// 2012-09-20
//
// Vladimir Popov commented these namespaces, when adjusting this code to Linux
//using stdext::hash_set;
//using stdext::hash_map;

#define DEBUG
int PalagyiFilter::order[8] = {2, 5, 3, 4, 1, 6, 0, 7};


PalagyiFilter::PalagyiFilter(Voxels& voxels_ref) :
  VoxelFilter(voxels_ref)
  //,  voxel_dt(voxel_dt_ref)
{
  set_index = 0;

  //
  // templates initialization (USW is already there)
  //
  for(int i=0; i < 8; i++) template_sets[i].convert_type(i);

}  // constructor



PalagyiFilter::~PalagyiFilter()
{

}  // destructor



void
PalagyiFilter::apply()
{
  //cout << "PalagyiFilter::apply" << endl;

  int nr_deleted;
  int total_deleted = 0;
  int nr_steps = 0;
  set_index = 0;
  
  do {
    nr_deleted = 0;
    for(int i=0; i < 8; i++) {
      nr_deleted += apply_template_set(set_index);

      set_index = (set_index + 1) % 8;
      //cout << "\rStep " << nr_steps << ", direction " << i << ", deleted " << nr_deleted << " voxels        " << flush;
      total_deleted += nr_deleted;
    }
    nr_steps++;
  } while (nr_deleted);
  //cout << "\rDeleted a total of " << total_deleted << " voxels in " << nr_steps << " steps" << endl;

  voxels.init_types(SKELETON);
  //  voxel_dt.compute_skeleton_dt();  // is this necessary? check code below
  // maybe just step_boundary in VoxelBoundary is sufficient

}  // PalagyiFilter::apply



static vector<int>to_be_deleted;

int
PalagyiFilter::apply_template_set(int direction)
{
  Templates& ts = template_sets[order[direction]];
  
  VoxelType nb[27];
  int nr_templates = ts.get_nr_templates();
  int size = voxels.get_size();
  
  for(int i=0; i < size; i++) {
    if (voxels[i]) {
      // get neighbours
      for(int j=0; j < 27; j++) nb[j] = voxels.get_neighbour(i, j);

      int match = 0;
      int template_index = 0;
      while (!match && (template_index < nr_templates)) {
	match = ts[template_index].match(nb);
	if (!match) template_index++;
      }  // while

      if (match) to_be_deleted.push_back(i);
    }  // if voxel
  }  // for each voxel

  //
  // delete marked voxels, update dt
  //
//   ChamferMetric& cm = voxel_dt.get_chamfer_metric();

//   int nr_to_be_deleted = to_be_deleted.size();
//   for(int i=0; i < nr_to_be_deleted; i++) {
//     //    cout << i << "/" << nr_to_be_deleted << endl;
    
//     //
//     // update distance transform here
//     //
//     // check neighbours
//     // choose minimum of neighbour distance tranform + 3/4/5
//     //
//     int index = to_be_deleted[i];
//     int minimum = MaxInt;
//     for(int j=0; j < 6; j++) {
//       int nb_index = voxels.get_face_neighbour_index(index, j);
//       if (!voxels[nb_index]) {
// 	if ((voxel_dt[nb_index] + cm.face_distance()) < minimum)
// 	  minimum = voxel_dt[nb_index] + cm.face_distance();
//       }
//     }
//     for(int j=0; j < 12; j++) {
//       int nb_index = voxels.get_edge_neighbour_index(index, j);
//       if (!voxels[nb_index]) {
// 	if ((voxel_dt[nb_index] + cm.edge_distance()) < minimum)
// 	  minimum = voxel_dt[nb_index] + cm.edge_distance();
//       }
//     }
//     for(int j=0; j < 8; j++) {
//       int nb_index = voxels.get_vertex_neighbour_index(index, j);
//       if (!voxels[nb_index]) {
// 	if ((voxel_dt[nb_index] + cm.vertex_distance()) < minimum)
// 	  minimum = voxel_dt[nb_index] + cm.vertex_distance();
//       }
//     }
//     voxel_dt[index] = minimum;
//     voxels[index] = 0;
//   }  // for, delete marked voxels

  int nr_to_be_deleted = to_be_deleted.size();
  for(int i=0; i < nr_to_be_deleted; i++) voxels[to_be_deleted[i]] = 0;
  to_be_deleted.clear();
    
  return nr_to_be_deleted;
  
}  // apply_template_set



void
PalagyiFilter::apply_once()
{
  cout << "PalagyiFilter::apply_once, set_index: " << set_index << endl;
  apply_template_set(set_index);
  set_index = (set_index + 1) % 8;

}  // PalagyiFilter::apply_once


/*
This function is the faster version of the function apply() above.
It implements thinning algorihtm for skeleton extraction 
by considering only boundary voxels for potential removal. 
Boundary list is updated at each iteration.
author: Olga Symonova
*/
void
PalagyiFilter::apply_fast()
{
#ifdef DEBUG
  //cout << "PalagyiFilter::apply" << endl;
#endif

  int nr_deleted;
  int total_deleted = 0;
  int nr_steps = 0;
  set_index = 0;
 
  __gnu_cxx::hash_set<int> bd;    
  int vsize=voxels.get_size();
  int n; 
  VoxelType nb[27];  

  //first get boundary voxels
  for(int i=0; i<vsize; i++)
  {
	  if(voxels[i]==0) continue;		  
	  
	  voxels.get_neighbours(i,&nb[0]);
	  n=0;	  
	  for(int k=0; k<27; k++)
	  {		  
		  if(k==13) continue;
		  else n=n+nb[k];		  
	  }
	  if(n<26)
		  bd.insert(i);	  
  }
  //cout << "\nApply_fast(): number of bd voxels:" << bd.size();
 
  // now perform thinning  
  do {
    nr_deleted = 0;		
    for(int i=0; i < 8; i++) {
		nr_deleted += apply_template_set_fast(set_index, bd);	 		

      set_index = (set_index + 1) % 8;
      //cout << "\rStep " << nr_steps << ", direction " << i << ", deleted " << nr_deleted << " voxels        " << flush;
      total_deleted += nr_deleted;
    }
    nr_steps++;
  } while (nr_deleted);
#ifdef DEBUG
  //cout << "\rDeleted a total of " << total_deleted << " voxels in " << nr_steps << " steps" << endl;
#endif

  voxels.init_types(SKELETON);
}  // PalagyiFilter::apply


/*
This function is anologous to the previous + erosion distance computation.
Erosion distance is estimated as a number of the thinning iteration 
at which the 3x3x3 neighborhood of a voxel was changed last time. 
It approximates the distance it takes to erode from an original shape 
to a one-voxel-wide skeleton.
The orginal thinning algorithm by Palagiy can leave some configuration of 
voxels that can be eroded even further. By checking if the Euler characteristics 
remains the same we can erode the volume till one-voxel-wide representation.
The values of the erosion distance are stored in the hash_map distR
author: Olga Symonova
*/
void PalagyiFilter::applyWdist(__gnu_cxx::hash_map<int, float> &distR)
{
#ifdef DEBUG
			//cout << "PalagyiFilter::apply" << endl;
#endif

  int nr_deleted;
  int total_deleted = 0;
  int nr_steps = 0;
  set_index = 0;
  __gnu_cxx::hash_set<int> bdind;  
  __gnu_cxx::hash_set<int> eroded;
  __gnu_cxx::hash_set<int>::iterator itset;
  __gnu_cxx::hash_map<int,pair<int,float> > ndmap;
  __gnu_cxx::hash_map<int,pair<int,float> >::iterator ndit; 
  
  voxels.init_euler_characteristics();
  int vsize=voxels.get_size();
  int n;
  VoxelType nb[27];  

  //first get bd voxels and initialize distances
  //the map ndmap stores linear indices of voxels together 
  //with number of current neighbors and erosion distance value.
  for(int i=0; i<vsize; i++)
  {
	  if(voxels[i]==0) continue;		  
	  
	  voxels.get_neighbours(i,&nb[0]);
	  n=0;	  
	  for(int k=0; k<27; k++)
	  {		  
		  if(k==13) continue;
		  else n=n+nb[k];		  
	  }
	  if(n<26)
		  bdind.insert(i);
	   ndmap.insert(make_pair(i,make_pair(n,0.f)));	  
  }

  //now perform thinning
  do {
    nr_deleted = 0;	
	eroded.clear();	
    for(int i=0; i < 8; i++) {
		//remove some voxels in the list bdind, and return removed voxels in the set eroded
		nr_deleted += apply_template_set_std_fast(set_index,eroded,bdind);		
		set_index = (set_index + 1) % 8;
#ifdef DEBUG
		//cout << "\rStep " << nr_steps << ", direction " << i << ", deleted " << nr_deleted << " voxels        " << flush;
#endif
		total_deleted += nr_deleted;
    }
	  __gnu_cxx::hash_set<int> add_todel;
	  __gnu_cxx::hash_set<int>::iterator ait;
	  int add_del=additional_erosion(add_todel,bdind);
	  total_deleted += add_del;
	  //printf("Additianl erosion: %d\n", add_del);
	  for(ait=add_todel.begin(); ait!=add_todel.end(); ait++) 	  		 
		  eroded.insert(*ait);
	  //update set of boundary voxels
	  for(ait=eroded.begin(); ait!=eroded.end(); ait++) 	 
	  {
		  voxels.get_neighbours(*ait,&nb[0]);
		  for(int j=0; j < 27; j++) 
			  if(nb[j]!=0)	  
				  bdind.insert(voxels.get_neighbour_index(*ait, j));
	  }	

	//cout << "\n";
    nr_steps++;
	if(nr_deleted==0)
		continue;
	//update erosion distances based on the voxels removed at the current iteration
	for(itset=eroded.begin(); itset!=eroded.end(); itset++)
	{	  
		  //get all neighbors of the eroded voxel
		  voxels.get_neighbours(*itset,&nb[0]);
		  //update erosion distance for the neghbors
		  for(int j=0; j<27; j++)
		  {
			  if(nb[j]==0) continue;			  
			  int nbind=voxels.get_neighbour_index(*itset,j);
			  
			  ndit=ndmap.find(nbind);
			  int nb_old=ndit->second.first;
			  //get new number of neighbors
			  VoxelType nb1[27];
			  voxels.get_neighbours(nbind,&nb1[0]);
			  n=0;
			  for(int k=0; k<27; k++)	  
				  n=n+nb1[k];
			  // if local neighborhood was changed update 
			  //the value of the erosion distance with the current iteration
			  if(n!=nb_old)
			  {
				  ndit->second.second=(float)(nr_steps+1);			  
				  ndit->second.first=n;			  				  
			  }
		  }
	}	   


  } while (nr_deleted);
#ifdef DEBUG
 // cout << "\rDeleted a total of " << total_deleted << " voxels in " << nr_steps << " steps" << endl;
#endif
  //move the values of the erosion distance from ndmap to distR
  for(int i=0; i<vsize; i++)
  {
	  if(voxels[i]==0) continue;
	  ndit=ndmap.find(i);
	  distR.insert(make_pair(i,ndit->second.second));
  }
  voxels.init_types(SKELETON);
}  // PalagyiFilter::apply


/*
This function is similar to apply_template_set(int direction).
It is faster than the original function because it considers 
only voxels in the set of boundary voxels bd. 
The bd set is concurrently as a voxels is removed.
All removed voxels are returned in the list er.
author: Olga Symonova
*/
int
PalagyiFilter::apply_template_set_std_fast(int direction, __gnu_cxx::hash_set<int> &er, __gnu_cxx::hash_set<int> &bd)
{
  Templates& ts = template_sets[order[direction]];
  
  VoxelType nb[27];
  int nr_templates = ts.get_nr_templates();
  int size = voxels.get_size();
  __gnu_cxx::hash_set<int>::iterator it;
  
  for(it=bd.begin(); it!=bd.end(); it++)
  {
	  int i = *it;
  //for(int i=0; i < size; i++) {
    //if (voxels[i]) {		
      // get neighbours
      for(int j=0; j < 27; j++) 
	  {
		  nb[j] = voxels.get_neighbour(i, j);		  
	  }
      int match = 0;
      int template_index = 0;
      while (!match && (template_index < nr_templates)) {
	match = ts[template_index].match(nb);
	if (!match) template_index++;
      }  // while
      if (match) to_be_deleted.push_back(i);
    //}  // if voxel
  }  // for each voxel

 
  int nr_to_be_deleted = to_be_deleted.size();
  for(int i=0; i < nr_to_be_deleted; i++) 	  
  {
	  voxels[to_be_deleted[i]] = 0;
	  er.insert(to_be_deleted[i]);
  }

  for(int i=0; i < nr_to_be_deleted; i++) 	  
	  bd.erase(to_be_deleted[i]); 
    
  to_be_deleted.clear();
    
  return nr_to_be_deleted;  
}  // apply_template_set

/*
This function is similar to apply_template_set(int direction).
It is faster than the original function because it considers 
only voxels in the set of boundary voxels bd. 
The bd set is concurrently as a voxels is removed.
author: Olga Symonova
*/
int
PalagyiFilter::apply_template_set_fast(int direction, __gnu_cxx::hash_set<int> &bd)
{
	Templates& ts = template_sets[order[direction]];
  
  VoxelType nb[27];
  int nr_templates = ts.get_nr_templates();
  int size = voxels.get_size();
  __gnu_cxx::hash_set<int>::iterator it;
  
  for(it=bd.begin(); it!=bd.end(); it++)
  {
	  int i = *it;
  //for(int i=0; i < size; i++) {
    //if (voxels[i]) {		
      // get neighbours
      voxels.get_neighbours(i, &nb[0]);
      int match = 0;
      int template_index = 0;
      while (!match && (template_index < nr_templates)) {
	match = ts[template_index].match(nb);
	if (!match) template_index++;
      }  // while
      if (match) to_be_deleted.push_back(i);
    //}  // if voxel
  }  // for each voxel
  
  int nr_to_be_deleted = to_be_deleted.size();
  for(int i=0; i < nr_to_be_deleted; i++) 	  
  {
	  voxels[to_be_deleted[i]] = 0;	  
  }
  //update bd set
  for(int i=0; i < nr_to_be_deleted; i++) 	  
  {
	  voxels.get_neighbours(to_be_deleted[i],&nb[0]);
	  for(int j=0; j < 27; j++) 
		  if(nb[j]!=0)	  
			  bd.insert(voxels.get_neighbour_index(to_be_deleted[i], j));
  }
  for(int i=0; i < nr_to_be_deleted; i++) 	  
	  bd.erase(to_be_deleted[i]);  
  to_be_deleted.clear();
    
  return nr_to_be_deleted;  
  
}  // apply_template_set

/**
* This function tries to erode voxels in todel set by checking if their 
* removal will change euler characteristic of the whole shape represended 
* by voxels[] array.
*/
int PalagyiFilter::additional_erosion(__gnu_cxx::hash_set<int> &todel, __gnu_cxx::hash_set<int> &bdind)
{
	int Euler=voxels.get_euler_characteristics();
	//printf("Euler characteristics: %d\n",Euler);
	__gnu_cxx::hash_set<int>::iterator bdit;
	VoxelType nb[27];
	int nnb;
	int deleted=0;

	for(bdit=bdind.begin(); bdit!=bdind.end(); bdit++)
	{
		//check if the euler characteristics remain the same when removing the voxel
		nnb=0;
		voxels.get_neighbours(*bdit,nb);
		for(int i=0; i<27; i++)
			nnb=nnb+nb[i];
		if(nnb<3)
			continue;
		int prem=1;
		int frem=voxels.get_nr_nonshared_faces(nb);
		int erem=voxels.get_nr_nonshared_edges(nb);
		int vrem=voxels.get_nr_nonshared_vertices(nb);		
		int newEuler=Euler-vrem+erem-frem+prem;
		// if Euler characteristics does not change, then remove the voxel 
		//and update the number of faces, edges, vertices and voxels.
		if(Euler==newEuler)
		{
			//printf("nb: %d, frem: %d, erem: %d, vrem %d\n",nnb,frem,erem,vrem);
			todel.insert(*bdit);
			voxels[*bdit]=0;
			deleted++;
		}
	}
	return deleted;
}




