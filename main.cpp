#define NOMINMAX

#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>

#include "menu.h"
#include "util.h"

namespace po = boost::program_options;

using namespace std;

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
		bool wrl, iv;
		generic.add_options()
			("version,V", "show program's version number and exit")
			("help,h", "show this help message and exit")
			("wrl", po::bool_switch(&wrl)->default_value(true), "generate mesh(es) using WRL format (Default: enabled)")
			("iv", po::bool_switch(&iv)->default_value(false), "generate mesh(es) using IV format (Default: disabled)")
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

		map<string, bool> output_flags;
		output_flags.insert(pair<string, bool>("wrl", vm["wrl"].as<bool>()));
		output_flags.insert(pair<string, bool>("iv", vm["iv"].as<bool>()));

		cout << VERSION << endl;
		cout << "Executable Path: " << argv[0] << endl;
		cout << "Point Cloud Volume Filepath: " << fileinput << endl;
		cout << "Features Filepath: " << fileoutput << endl;
		cout << "Scale: " << scale << endl;
		cout << "WRL write-flag: " << output_flags["wrl"] << endl;
		cout << "IV write-flag: " << output_flags["iv"] << endl;

		skel_and_features_pipeline(fileinput, fileoutput, scale, output_flags);
	}
	catch (std::exception &e)
	{
		cout << e.what() << "\n";
		return 1;
	}
	return 0;
}