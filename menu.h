#ifndef MENU_H 
	#define MENU_H

#include<string>

// 2012-09-20
//
// Vladimir Popov added, when adjusting this code to Linux
//
// newline at the end of this file

using namespace std;

void skel_and_features(string filename, float scale);
void skel_and_features_pipeline(string fileinput, string fileoutput, float scale);
void root_graph_output(string filename);
void root_features_range(string filename);
void root_repair(string filename);
#endif
