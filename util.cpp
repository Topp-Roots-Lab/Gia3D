# include "util.h"
# include <iostream>
#include <fstream>
#include <stdio.h>
#include <queue>
using __gnu_cxx::hash_set;

using namespace std;

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
	hash_map<int,int> coords;
	pair <hash_map<int,int>::iterator,bool> ins;

	//find boundary voxels
	//put visible faces and their vertices in the array
	vector<vector<int>> inds(0);
	vector<int> ind(3);
	int i,j,k;
	hash_map<int,float>::iterator it;
	hash_map<int,float>::iterator itf;
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
	vector<vector<int>> coordall(coords.size());
	for(int ii=0; ii<(int)coords.size(); ii++)
		coordall[ii].resize(3);
	hash_map<int,int>::iterator iter;
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
	for(int i=0; i<(int)coordall.size(); i++)
		fprintf(f, " %d %d %d\n", coordall[i][0],coordall[i][1], coordall[i][2]);
	fprintf(f, "]\n }\n");
	fprintf(f, "IndexedFaceSet { \ncoordIndex [\n");
	for(int i=0; i<(int)inds.size(); i++)
			fprintf(f, " %i %i %i -1", inds[i][0],inds[i][1], inds[i][2]);
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

