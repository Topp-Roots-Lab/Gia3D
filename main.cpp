#define NOMINMAX

#include "menu.h"
#include "util.h"

using namespace std;

// this main call is to run the code in batch and output features in the same file
int main(int argc, char ** argv)
{
	string filename=argv[1];
	float scale=(float)atof(argv[2]);
	skel_and_features(filename, scale);
    return 0;
}


/*int main(int argc, char ** argv)
{
	//if (argc<2)
	//{
	//	printf("Inputfile.out and outputfile are needed!.. Exitting.\n");
	//	return 0;
	//}
	//string filename=argv[1];
	//string filestorename=argv[2];
	//// create a root model from the file //rtvox format
	//MedialCurve skel(filename,1,true);
	//writeVisibleIVEff(filestorename,skel);
	//return 1;



	if (argc<2)
	{
		printf("This program should be called with RTVOX file as an argument... Exitting.\n");
		return 0;
	}
	string filename=argv[1];

	printf("Chose what you want to do:\n");
	printf("\t [1] - Read root file, extract the skeleton and compute root features\n");
	printf("\t [2] - Read root file, extract and output the skeleton \n");
	printf("\t [3] - Compute range of features based on the scaling factor\n");
	// read root data, thin and output skeleton as adjacency matrix

	char cmd[10];
	scanf("%s", cmd);
	string strcmd=cmd;
	int i;

	char c;

	for(i=0; i<(int)strcmd.length();i++)
	{
		switch(c=strcmd[i])
		{
			case '1':
			// read voxelization from file and perform thinning, skeleton prunning, and feature computation
			{
				skel_and_features(filename);
				break;
			}
			case '2':
			// read root data, thin and output skeleton as adjacency matrix
			{
				root_graph_output(filename);
				break;
			}
			case '3':
			// read root data, thin and output skeleton as adjacency matrix
			{
				root_features_range(filename);
				break;
			}
			case '4':
			// read root data, thin and output skeleton as adjacency matrix
			{
				printf("You chose to print info about the root:");
				root_repair(filename);
				break;
			}
			default:
			{
				printf("Nothing was chosen... exitting. Ciao!");
			}
		}
	}
    return 0;
  }*/















