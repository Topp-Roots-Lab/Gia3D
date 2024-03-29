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
// $Id: PalagyiFilter.h,v 1.2 2001/01/20 02:38:28 min Exp $
//
// comment: functions apply_fast, applyWdist, apply_template_set_fast, and
// apply_template_set_std_fast were added by Olga Symonova (olga.symonova "at" gmail "dot" com) in 2010.

#ifndef __PalagyiFilter_h
#define __PalagyiFilter_h

// 2012-09-20
//
// Vladimir Popov commented these includes, when adjusting this code to Linux
//
//#include <hash_map>
//#include <hash_set>

#include "VoxelFilter.h"
//#include "VoxelDT.h"
#include "Templates.h"

// 2012-09-20
//
// Vladimir Popov added these includes, when adjusting this code to Linux
//
#include <ext/hash_map>
#include <ext/hash_set>

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


class PalagyiControl;




class PalagyiFilter : public VoxelFilter
{

public:

  PalagyiFilter(Voxels& voxels_ref);
  ~PalagyiFilter();

  void apply();
  void apply_fast();
  void apply_once();
  void applyWdist(__gnu_cxx::hash_map<int, float> &distR);
  
  friend class PalagyiControl;

  string get_name() { return "PalagyiFilter"; }
  
  
private:

  static int order[8];

  int apply_template_set(int direction);
  int apply_template_set_fast(int direction, __gnu_cxx::hash_set<int> &bd);
  int apply_template_set_std_fast(int direction, __gnu_cxx::hash_set<int> &er, __gnu_cxx::hash_set<int> &bd);
  int additional_erosion(__gnu_cxx::hash_set<int> &todel, __gnu_cxx::hash_set<int> &bdind);
  
  //  VoxelDT& voxel_dt;
  
  Templates template_sets[8];
  int set_index;
  
  
};  // PalagyiFilter class

#endif


