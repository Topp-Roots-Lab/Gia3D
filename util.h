#ifndef UTIL_H
	#define UTIL_H

#include <string>
#include <vector>
#include "MedialCurve.h"

const string VERSION = "skeletonization-and-features-pipeline 2.1.2";
// 2012-09-20
//
// Vladimir Popov added, when adjusting this code to Linux
//
// newline at the end of this file

using namespace std;

void tokenize(const string& str, vector<string>& tokens, const string& delimiters = " ");
void writeVisibleIVEff(string filename, MedialCurve &m);
void writeVisibleIvWrlEff(string filename, MedialCurve &m, int type);
void getRGBcolor4Interals(float x, float &r, float &g, float &b);
void drawSphereIV(string filename, float x, float y, float z, float r);
//convert back and force between linear index and xyz coordinates
//inline void linind2subind(int ind, int &x, int &y, int &z, int Xsize, int Ysize, int Zsize){x=(int)floor((double)ind/(double)(Ysize*Zsize)); y=(int)floor((double)(ind-x*Ysize*Zsize)/(double)Zsize); z=ind-x*Ysize*Zsize-y*Zsize; return;}
//inline void linind2subind(int ind, float &x, float &y, float &z, int Xsize, int Ysize, int Zsize){x=(float)floor((double)ind/(double)(Ysize*Zsize)); y=(float)floor((double)(ind-x*Ysize*Zsize)/(double)Zsize); z=(float)(ind-x*Ysize*Zsize-y*Zsize); return;}
//inline int subind2linind(int i, int j, int k, int Xsize, int Ysize, int Zsize){return i*Ysize*Zsize+j*Zsize+k;};
inline void linind2subind(int ind, int &x, int &y, int &z, int Xsize, int Ysize, int Zsize){z=(int)floor((double)ind/(double)(Ysize*Xsize)); y=(int)floor((double)(ind-z*Ysize*Xsize)/(double)Xsize); x=ind-z*Ysize*Xsize-y*Xsize; return;}
inline void linind2subind(int ind, float &x, float &y, float &z, int Xsize, int Ysize, int Zsize){z=(float)floor((double)ind/(double)(Ysize*Xsize)); y=(float)floor((double)(ind-z*Ysize*Xsize)/(double)Xsize); x=(float)(ind-z*Ysize*Xsize-y*Xsize); return;}
inline int subind2linind(int i, int j, int k, int Xsize, int Ysize, int Zsize){return k*Ysize*Xsize+j*Xsize+i;};
inline float eucl_dist(float x1, float y1, float z1, float x2, float y2, float z2) {return (float)sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));};

#endif

