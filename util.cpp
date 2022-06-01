# include "util.h"
# include <iostream>
#include <fstream>
#include <stdio.h>
#include <queue>

using namespace std;

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

/**
* this funtion is used to tokenize a string by a number
* of given delimeters. The extracted string pieces are
* stored in the vector tokens. This function was found on the Internet.
*/
void tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}
/**
* write the object MedialCurve into IV and VRML(WRL) file
* @param filename input path
* @param m medial curve representation of object
* @param type output file format, '1' saves object as .iv; '2' saves object
 *			  as .wrl
*/
void writeVisibleIvWrlEff(string filename, MedialCurve &m, int type)
{
	string fileout;
	if (type == 1)
	{
		fileout = filename + ".iv";
	}
	else if (type == 2)
	{
		fileout = filename + ".wrl";
	}
	else
	{
		cerr << "Type file not supported:\n"
				 << endl;
		cerr << "Supported file types: 1 (uv) and 2 (wrl)\n"
				 << endl;
	}
	cout << "file: " << fileout << endl;

	int si, sj, sk;
	si = m.xsize;
	sj = m.ysize;
	sk = m.zsize;
	//these are vertices of a voxel
	int v1, v2, v3, v4, v5, v6, v7, v8;
	//indices per square face
	int ind1, ind2, ind3, ind4;
	__gnu_cxx::hash_map<int, int> coords;
	pair<__gnu_cxx::hash_map<int, int>::iterator, bool> ins;

	//find boundary voxels
	//put visible faces and their vertices in the array
	vector<vector<int>> inds(0);
	vector<int> ind(3);
	int i, j, k;
	__gnu_cxx::hash_map<int, float>::iterator it;
	__gnu_cxx::hash_map<int, float>::iterator itf;
	int X = m.xsize * m.ysize * m.zsize;

	coords.clear();
	int num = 0;
	for (it = m.voxset.begin(); it != m.voxset.end(); it++)
	{
		v1 = it->first;
		m.linind2subind(it->first, i, j, k);
		v2 = m.subind2linind(i + 1, j, k);
		v3 = m.subind2linind(i + 1, j, k + 1);
		v4 = m.subind2linind(i, j, k + 1);
		v5 = m.subind2linind(i, j + 1, k);
		v6 = m.subind2linind(i + 1, j + 1, k);
		v7 = m.subind2linind(i + 1, j + 1, k + 1);
		v8 = m.subind2linind(i, j + 1, k + 1);

		//buttom face is visible
		itf = m.voxset.find(it->first - si);
		if (itf == m.voxset.end())
		{
			ins = coords.insert(pair<int, int>(v1, num));
			if (ins.second == true)
				num++;
			ind1 = ins.first->second;

			ins = coords.insert(pair<int, int>(v2, num));
			if (ins.second == true)
				num++;
			ind2 = ins.first->second;

			ins = coords.insert(pair<int, int>(v3, num));
			if (ins.second == true)
				num++;
			ind3 = ins.first->second;

			ins = coords.insert(pair<int, int>(v4, num));
			if (ins.second == true)
				num++;
			ind4 = ins.first->second;

			ind[0] = ind1;
			ind[1] = ind2;
			ind[2] = ind3;
			inds.push_back(ind);
			ind[0] = ind1;
			ind[1] = ind3;
			ind[2] = ind4;
			inds.push_back(ind);
		}
		itf = m.voxset.find(it->first + si);
		//above face is visible
		if (itf == m.voxset.end())
		{
			ins = coords.insert(pair<int, int>(v5, num));
			if (ins.second == true)
				num++;
			ind1 = ins.first->second;

			ins = coords.insert(pair<int, int>(v6, num));
			if (ins.second == true)
				num++;
			ind2 = ins.first->second;

			ins = coords.insert(pair<int, int>(v7, num));
			if (ins.second == true)
				num++;
			ind3 = ins.first->second;

			ins = coords.insert(pair<int, int>(v8, num));
			if (ins.second == true)
				num++;
			ind4 = ins.first->second;

			ind[0] = ind1;
			ind[1] = ind4;
			ind[2] = ind3;
			inds.push_back(ind);
			ind[0] = ind1;
			ind[1] = ind3;
			ind[2] = ind2;
			inds.push_back(ind);
		}
		//right face is visible
		itf = m.voxset.find(it->first + 1);
		if (itf == m.voxset.end())
		{
			ins = coords.insert(pair<int, int>(v2, num));
			if (ins.second == true)
				num++;
			ind1 = ins.first->second;

			ins = coords.insert(pair<int, int>(v3, num));
			if (ins.second == true)
				num++;
			ind2 = ins.first->second;

			ins = coords.insert(pair<int, int>(v6, num));
			if (ins.second == true)
				num++;
			ind3 = ins.first->second;

			ins = coords.insert(pair<int, int>(v7, num));
			if (ins.second == true)
				num++;
			ind4 = ins.first->second;

			ind[0] = ind4;
			ind[1] = ind2;
			ind[2] = ind1;
			inds.push_back(ind);
			ind[0] = ind4;
			ind[1] = ind1;
			ind[2] = ind3;
			inds.push_back(ind);
		}
		//left face is visible
		itf = m.voxset.find(it->first - 1);
		if (itf == m.voxset.end())
		{
			ins = coords.insert(pair<int, int>(v1, num));
			if (ins.second == true)
				num++;
			ind1 = ins.first->second;

			ins = coords.insert(pair<int, int>(v4, num));
			if (ins.second == true)
				num++;
			ind2 = ins.first->second;

			ins = coords.insert(pair<int, int>(v5, num));
			if (ins.second == true)
				num++;
			ind3 = ins.first->second;

			ins = coords.insert(pair<int, int>(v8, num));
			if (ins.second == true)
				num++;
			ind4 = ins.first->second;

			ind[0] = ind4;
			ind[1] = ind1;
			ind[2] = ind2;
			inds.push_back(ind);
			ind[0] = ind4;
			ind[1] = ind3;
			ind[2] = ind1;
			inds.push_back(ind);
		}
		//back face is visible
		itf = m.voxset.find(it->first - si * sj);
		if (itf == m.voxset.end())
		{
			ins = coords.insert(pair<int, int>(v1, num));
			if (ins.second == true)
				num++;
			ind1 = ins.first->second;

			ins = coords.insert(pair<int, int>(v2, num));
			if (ins.second == true)
				num++;
			ind2 = ins.first->second;

			ins = coords.insert(pair<int, int>(v5, num));
			if (ins.second == true)
				num++;
			ind3 = ins.first->second;

			ins = coords.insert(pair<int, int>(v6, num));
			if (ins.second == true)
				num++;
			ind4 = ins.first->second;

			ind[0] = ind3;
			ind[1] = ind2;
			ind[2] = ind1;
			inds.push_back(ind);
			ind[0] = ind3;
			ind[1] = ind4;
			ind[2] = ind2;
			inds.push_back(ind);
		}
		//front face is visible
		itf = m.voxset.find(it->first + si * sj);
		if (itf == m.voxset.end())
		{
			ins = coords.insert(pair<int, int>(v3, num));
			if (ins.second == true)
				num++;
			ind1 = ins.first->second;

			ins = coords.insert(pair<int, int>(v4, num));
			if (ins.second == true)
				num++;
			ind2 = ins.first->second;

			ins = coords.insert(pair<int, int>(v7, num));
			if (ins.second == true)
				num++;
			ind3 = ins.first->second;

			ins = coords.insert(pair<int, int>(v8, num));
			if (ins.second == true)
				num++;
			ind4 = ins.first->second;

			ind[0] = ind3;
			ind[1] = ind4;
			ind[2] = ind2;
			inds.push_back(ind);
			ind[0] = ind3;
			ind[1] = ind2;
			ind[2] = ind1;
			inds.push_back(ind);
		}
	}
	vector<vector<int>> coordall(coords.size());
	for (int ii = 0; ii < (int)coords.size(); ii++)
		coordall[ii].resize(3);
	__gnu_cxx::hash_map<int, int>::iterator iter;
	for (iter = coords.begin(); iter != coords.end(); iter++)
	{
		m.linind2subind(iter->first, i, j, k);
		//printf("Coord index %i\n", iter->second);
		coordall[iter->second][0] = i;
		coordall[iter->second][1] = j;
		coordall[iter->second][2] = k;
	}

	//string fileout;
	/*int pp=filename.find('.');
	fileout=filename.substr(0,pp);
	fileout=fileout+".iv";*/

	//char strsc[100];
	float r=0.75f;
	float g=0.35f;
	float b=0.01f;
	float tr=0.0f;
	// float r = 0.60f;
	// float g = 0.60f;
	// float b = 0.60f;
	// float tr = 0.0f;
	cout << "Mesh color set to rgba(" << r << ", " << g << ", " << b << ", " << tr << ")" << endl;
	//Uncomment this part if you want a user input on color choice
	/*printf("Enter the color of the object for visualization (r g b format)] :\n");
	printf("red: ");
	scanf("%s", &strsc);
	float r =(float)atof(strsc);
	printf("green: ");
	scanf("%s", &strsc);
	float g =(float)atof(strsc);
	printf("blue: ");
	scanf("%s", &strsc);
	float b =(float)atof(strsc);
	printf("transparancy( [0..1] where 1 is completely transparent) : ");
	scanf("%s", &strsc);
	float tr =(float)atof(strsc);*/

	FILE *f = fopen(fileout.c_str(), "w");
	if (type == 1)
	{
		////////////////////////////////////////////////////
		// IV
		////////////////////////////////////////////////////
		fprintf(f, "#Inventor V2.1 ascii\n Separator {\n");
		fprintf(f, "Material { \n diffuseColor %.3f %.3f %.3f\n transparency %.3f\n }\n", r, g, b, tr);
		fprintf(f, "Coordinate3 { \n point [\n");
		////////////////////////////////////////////////////
	}
	if (type == 2)
	{
		////////////////////////////////////////////////////
		// VRML
		////////////////////////////////////////////////////
		fprintf(f, "#VRML V2.0 utf8\n");
		fprintf(f, "#%s\n", VERSION.c_str());
		fprintf(f, "Shape {\n");
		fprintf(f, "    appearance Appearance {\n");
		//fprintf(f, "        material DEF BaseMaterial Material {\n");
		fprintf(f, "        material Material {\n");
		fprintf(f, "        diffuseColor %.3f %.3f %.3f\n", r, g, b);
		fprintf(f, "        transparency %.3f\n", tr);
		fprintf(f, "        }\n");
		fprintf(f, "    }\n");
		fprintf(f, "    geometry IndexedFaceSet {\n");
		fprintf(f, "        coord Coordinate {\n");
		fprintf(f, "point [\n");
		////////////////////////////////////////////////////
	}
	// Checkpoint message: Create WRL output file
	int linesToProcess = (int)coordall.size() + (int)inds.size();
	cout << "Writing WRL '" << filename << "' (line count: " << linesToProcess << ")" << endl;

	// add comma after every point, except for the last one
	// to be able to view IV file with IV Viewer (installed on BioRoss Linux server) otherwise it does not work -
	// gives error when reading file.
	// (For Windows IV Viewer, there is no need for comma: viewer shows file without commas)
	int last = (int)coordall.size() - 1;
	int ptCounter = 0;
	for (int i = 0; i < last; i++)
	{
		ptCounter++;
		// if ((ptCounter - 1) % int(floor(last/200)) == 0 || ptCounter == last)
		if (ptCounter == last)
		{
			cout << "\rWrite point " << ptCounter << " of " << last << " from " << filename << " to mesh" << flush;
		}
		fprintf(f, "%d %d %d,\n", coordall[i][0], coordall[i][1], coordall[i][2]);
	}
	fprintf(f, "%d %d %d\n", coordall[last][0], coordall[last][1], coordall[last][2]);
	cout << endl;

	fprintf(f, "]\n }\n");
	if (type == 1)
	{
		////////////////////////////////////////////////////
		// IV
		////////////////////////////////////////////////////
		fprintf(f, "IndexedFaceSet { \ncoordIndex [\n");
		////////////////////////////////////////////////////
	}
	if (type == 2)
	{
		////////////////////////////////////////////////////
		// VRML
		////////////////////////////////////////////////////
		fprintf(f, "coordIndex [\n");
		////////////////////////////////////////////////////
	}

	// add comma after every number, except for the last one
	// to be able to view IV file with IV Viewer (installed on BioRoss Linux server) otherwise it does not work -
	// gives error when reading file.
	// (For Windows IV Viewer, there is no need for commas: viewer shows file without commas)
	int last2 = (int)inds.size() - 1;
	ptCounter = 0;
	for (int i = 0; i < last2; i++)
	{
			ptCounter++;
		// if ((ptCounter - 1) % int(floor(last2/200)) == 0 || ptCounter == last2)
		if (ptCounter == last2)
		{
			cout << "\rWrite face " << ptCounter << " of " << last2 << " from " << filename << " to mesh" << flush;
		}
		fprintf(f, "%i,%i,%i,-1,", inds[i][0], inds[i][1], inds[i][2]);
	}
	cout << endl;
	fprintf(f, "%i,%i,%i,-1", inds[last2][0], inds[last2][1], inds[last2][2]);

	fprintf(f, "]\n }\n");
	fprintf(f, "}\n");
	fclose(f);
}

/**
* write the object MedialCurve into IV file
*/
void writeVisibleIVEff(string filename, MedialCurve &m)
{
	int si, sj, sk;
	si=m.xsize;
	sj=m.ysize;
	sk=m.zsize;
	//these are vertices of a voxel
	int v1,v2,v3,v4,v5,v6,v7,v8;
	//indices per square face
	int ind1,ind2,ind3,ind4;
	__gnu_cxx::hash_map<int,int> coords;
	pair <__gnu_cxx::hash_map<int,int>::iterator,bool> ins;

	//find boundary voxels
	//put visible faces and their vertices in the array
	vector<vector<int> > inds(0);
	vector<int> ind(3);
	int i,j,k;
	__gnu_cxx::hash_map<int,float>::iterator it;
	__gnu_cxx::hash_map<int,float>::iterator itf;
	int X=m.xsize*m.ysize*m.zsize;

	coords.clear();
	int num=0;
	for(it=m.voxset.begin(); it!=m.voxset.end(); it++)
	{
		v1=it->first;
		m.linind2subind(it->first,i,j,k);
		v2=m.subind2linind(i+1,j  ,k  );
		v3=m.subind2linind(i+1,j  ,k+1);
		v4=m.subind2linind(i  ,j  ,k+1);
		v5=m.subind2linind(i  ,j+1,k  );
		v6=m.subind2linind(i+1,j+1,k  );
		v7=m.subind2linind(i+1,j+1,k+1);
		v8=m.subind2linind(i  ,j+1,k+1);

		//buttom face is visible
		itf = m.voxset.find(it->first-si);
		if(itf==m.voxset.end())
		{
			ins=coords.insert(pair<int,int>(v1,num));
			if(ins.second==true) num++;
			ind1=ins.first->second;

			ins=coords.insert(pair<int,int>(v2,num));
			if(ins.second==true) num++;
			ind2=ins.first->second;

			ins=coords.insert(pair<int,int>(v3,num));
			if(ins.second==true) num++;
			ind3=ins.first->second;

			ins=coords.insert(pair<int,int>(v4,num));
			if(ins.second==true) num++;
			ind4=ins.first->second;

			ind[0]=ind1; ind[1]=ind2; ind[2]=ind3;
			inds.push_back(ind);
			ind[0]=ind1; ind[1]=ind3; ind[2]=ind4;
			inds.push_back(ind);
		}
		itf = m.voxset.find(it->first+si);
		//above face is visible
		if(itf==m.voxset.end())
		{
			ins=coords.insert(pair<int,int>(v5,num));
			if(ins.second==true) num++;
			ind1=ins.first->second;

			ins=coords.insert(pair<int,int>(v6,num));
			if(ins.second==true) num++;
			ind2=ins.first->second;

			ins=coords.insert(pair<int,int>(v7,num));
			if(ins.second==true) num++;
			ind3=ins.first->second;

			ins=coords.insert(pair<int,int>(v8,num));
			if(ins.second==true) num++;
			ind4=ins.first->second;

			ind[0]=ind1; ind[1]=ind4; ind[2]=ind3;
			inds.push_back(ind);
			ind[0]=ind1; ind[1]=ind3; ind[2]=ind2;
			inds.push_back(ind);
		}
		//right face is visible
		itf = m.voxset.find(it->first+1);
		if(itf==m.voxset.end())
		{
			ins=coords.insert(pair<int,int>(v2,num));
			if(ins.second==true) num++;
			ind1=ins.first->second;

			ins=coords.insert(pair<int,int>(v3,num));
			if(ins.second==true) num++;
			ind2=ins.first->second;

			ins=coords.insert(pair<int,int>(v6,num));
			if(ins.second==true) num++;
			ind3=ins.first->second;

			ins=coords.insert(pair<int,int>(v7,num));
			if(ins.second==true) num++;
			ind4=ins.first->second;

			ind[0]=ind4; ind[1]=ind2; ind[2]=ind1;
			inds.push_back(ind);
			ind[0]=ind4; ind[1]=ind1; ind[2]=ind3;
			inds.push_back(ind);
		}
		//left face is visible
		itf = m.voxset.find(it->first-1);
		if(itf==m.voxset.end())
		{
			ins=coords.insert(pair<int,int>(v1,num));
			if(ins.second==true) num++;
			ind1=ins.first->second;

			ins=coords.insert(pair<int,int>(v4,num));
			if(ins.second==true) num++;
			ind2=ins.first->second;

			ins=coords.insert(pair<int,int>(v5,num));
			if(ins.second==true) num++;
			ind3=ins.first->second;

			ins=coords.insert(pair<int,int>(v8,num));
			if(ins.second==true) num++;
			ind4=ins.first->second;

			ind[0]=ind4; ind[1]=ind1; ind[2]=ind2;
			inds.push_back(ind);
			ind[0]=ind4; ind[1]=ind3; ind[2]=ind1;
			inds.push_back(ind);
		}
		//back face is visible
		itf = m.voxset.find(it->first-si*sj);
		if(itf==m.voxset.end())
		{
			ins=coords.insert(pair<int,int>(v1,num));
			if(ins.second==true) num++;
			ind1=ins.first->second;

			ins=coords.insert(pair<int,int>(v2,num));
			if(ins.second==true) num++;
			ind2=ins.first->second;

			ins=coords.insert(pair<int,int>(v5,num));
			if(ins.second==true) num++;
			ind3=ins.first->second;

			ins=coords.insert(pair<int,int>(v6,num));
			if(ins.second==true) num++;
			ind4=ins.first->second;

			ind[0]=ind3; ind[1]=ind2; ind[2]=ind1;
			inds.push_back(ind);
			ind[0]=ind3; ind[1]=ind4; ind[2]=ind2;
			inds.push_back(ind);
		}
		//front face is visible
		itf = m.voxset.find(it->first+si*sj);
		if(itf==m.voxset.end())
		{
			ins=coords.insert(pair<int,int>(v3,num));
			if(ins.second==true) num++;
			ind1=ins.first->second;

			ins=coords.insert(pair<int,int>(v4,num));
			if(ins.second==true) num++;
			ind2=ins.first->second;

			ins=coords.insert(pair<int,int>(v7,num));
			if(ins.second==true) num++;
			ind3=ins.first->second;

			ins=coords.insert(pair<int,int>(v8,num));
			if(ins.second==true) num++;
			ind4=ins.first->second;

			ind[0]=ind3; ind[1]=ind4; ind[2]=ind2;
			inds.push_back(ind);
			ind[0]=ind3; ind[1]=ind2; ind[2]=ind1;
			inds.push_back(ind);
		}
	}
	vector<vector<int> > coordall(coords.size());
	for(int ii=0; ii<(int)coords.size(); ii++)
		coordall[ii].resize(3);
	__gnu_cxx::hash_map<int,int>::iterator iter;
	for(iter=coords.begin(); iter!=coords.end(); iter++)
	{
		m.linind2subind(iter->first,i,j,k);
		//printf("Coord index %i\n", iter->second);
		coordall[iter->second][0]=i;
		coordall[iter->second][1]=j;
		coordall[iter->second][2]=k;
	}

	string fileout;
	/*int pp=filename.find('.');
	fileout=filename.substr(0,pp);
	fileout=fileout+".iv";*/
	fileout=filename+".iv";

	//char strsc[100];
	float r=0.75f;
	float g=0.35f;
	float b=0.01f;
	float tr=0.0f;
	// float r = 0.60f;
	// float g = 0.60f;
	// float b = 0.60f;
	// float tr = 0.0f;
	cout << "Mesh color set to rgba(" << r << ", " << g << ", " << b << ", " << tr << ")" << endl;
	//Uncomment this part if you want a user input on color choice
	/*printf("Enter the color of the object for visualization (r g b format)] :\n");
	printf("red: ");
	scanf("%s", &strsc);
	float r =(float)atof(strsc);
	printf("green: ");
	scanf("%s", &strsc);
	float g =(float)atof(strsc);
	printf("blue: ");
	scanf("%s", &strsc);
	float b =(float)atof(strsc);
	printf("transparancy( [0..1] where 1 is completely transparent) : ");
	scanf("%s", &strsc);
	float tr =(float)atof(strsc);*/

	FILE *f = fopen(fileout.c_str(),"w");
	fprintf(f, "#Inventor V2.1 ascii\n Separator {\n");
	fprintf(f, "Material { \n diffuseColor %.3f %.3f %.3f\n transparency %.3f\n }\n", r,g,b,tr);
	fprintf(f, "Coordinate3 { \n point [\n");

	// add comma after every point, except for the last one
	// to be able to view IV file with IV Viewer (installed on BioRoss Linux server) otherwise it does not work -
	// gives error when reading file.
	// (For Windows IV Viewer, there is no need for comma: viewer shows file without commas)
        int last = (int)coordall.size()-1;
	for(int i=0; i<last; i++)
        {
	    fprintf(f, "%d %d %d,\n", coordall[i][0],coordall[i][1], coordall[i][2]);
        }
        fprintf(f, "%d %d %d\n", coordall[last][0],coordall[last][1], coordall[last][2]);

	fprintf(f, "]\n }\n");
	fprintf(f, "IndexedFaceSet { \ncoordIndex [\n");
	// add comma after every number, except for the last one
	// to be able to view IV file with IV Viewer (installed on BioRoss Linux server) otherwise it does not work -
	// gives error when reading file.
	// (For Windows IV Viewer, there is no need for commas: viewer shows file without commas)
        int last2 = (int)inds.size()-1;
	for(int i=0; i<last2; i++){
	    fprintf(f, "%i,%i,%i,-1,", inds[i][0],inds[i][1], inds[i][2]);
        }
        fprintf(f, "%i,%i,%i,-1", inds[last2][0],inds[last2][1], inds[last2][2]);

	fprintf(f, "]\n }\n");
	fprintf(f, "}\n");
	fclose(f);
}


/**
* colormap of four intervals:
* returns the values RGB given a float value x
*/
void getRGBcolor4Interals(float x, float &r, float &g, float &b)
{
	// heat map with 4 intervals
	if(x<0)
	{ r=0; g=0; b=1;}
	else if(x<=0.25)
	{r=0; g=4*x; b=1; }
	else if(x<=0.5)
	{ r=0; g=1; b=2-4*x; }
	else if(x<=0.75)
	{ r=4*(x-0.5f); g=1; b=0; }
	else if(x<=1)
	{ r=1; g=2-4*(x-0.5f); b=0; }
	else
	{ r=1; g=0; b=0;}
}

/**
* output IV file of a sphere in a given position with a given radius
*/
void drawSphereIV(string filename, float x, float y, float z, float r)
{
	//store to the file
	FILE *f = fopen(filename.c_str(),"w");

	fprintf(f,"#Inventor V2.0 ascii\n");
	fprintf(f,"Separator {\n");
    fprintf(f,"Material { diffuseColor 1.0 0.0 0.0}\n");
    fprintf(f,"Transform { translation %.3f %.3f %.3f}\n",x,y,z);
	fprintf(f,"Sphere { radius %.3f }\n",r);
	fprintf(f,"}");
	fclose(f);
}

