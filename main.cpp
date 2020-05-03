#define NOMINMAX

#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>

using namespace boost;
namespace po = boost::program_options;

#include "menu.h"
#include "util.h"

using namespace std;
const string VERSION = "2.0.4";

// this main call is to run the code in batch and output features in the same file
int main(int argc, char **argv)
{
	try
	{
		// Declare the supported options.
		// Declare a group of options that will be
		// allowed only on command line
		po::options_description generic("Allowed options");
		float scale;
		generic.add_options()
			("version,V", "show program's version number and exit")
			("help,h", "show this help message and exit")
			;

		po::options_description hidden("Hidden options");
		hidden.add_options()
			("input-file","input filepath, point cloud file (.out)")
			("output-file","output filepath, tab-delimited text file (.tsv)")
			("scale", po::value<float>(&scale)->default_value(2.25f), "scale level. increase to reduce skeleton complexity.")
		;

		po::positional_options_description p;
		p.add("input-file", 1).add("output-file", 1).add("scale", 1);

		po::options_description cmdline_options;
		cmdline_options.add(generic).add(hidden);
		auto args = po::command_line_parser(argc, argv)
									.options(cmdline_options)
									.positional(p)
									.run();
		po::variables_map vm;
		po::store(args, vm);
		po::notify(vm);

		if (vm.count("version"))
		{
			cout << argv[0] << " " << VERSION << endl;
			return 0;
		}

		if (vm.count("help") || !(vm.count("input-file") && vm.count("output-file") && vm.count("scale")))
		{
			cout << "Usage: " << argv[0] << " [fileinput] [fileoutput] [scale]" << endl << endl;
			cout << generic << endl;;
			return 1;
		}

		string fileinput = vm["input-file"].as<string>();
		string fileoutput = vm["output-file"].as<string>();

		cout << "Executing: " << argv[0] << endl;
		cout << "fileinput: " << fileinput << endl;
		cout << "fileoutput: " << fileoutput << endl;
		cout << "scale: " << scale << endl;
		cout << "\nProcessing ... " << endl;

		skel_and_features_pipeline(fileinput, fileoutput, scale);

	}
	catch (std::exception &e)
	{
		cout << e.what() << "\n";
		return 1;
	}
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
