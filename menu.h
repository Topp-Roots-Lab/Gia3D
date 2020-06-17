#ifndef MENU_H
	#define MENU_H

#include<string>
#include<map>

// 2012-09-20
//
// Vladimir Popov added, when adjusting this code to Linux
//
// newline at the end of this file

using namespace std;

void skel_and_features_pipeline(string fileinput, string fileoutput, float scale, map<string, bool> output_flags);
#endif
