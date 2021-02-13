#include "menu.h"
#include "util.h"
#include "MedialCurve.h"
#include "RootGraph.h"

// 2012-09-20
//
// Vladimir Popov added, when adjusting this code to Linux
//
// newline at the end of this file

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

/*
 * Compute traits on point cloud data (.OUT)
 *
 * Converts point cloud data into mesh data (.WRL), generating an original mesh
 * and skeleton mesh, and then measures traits that are output in TSV format.
 *
 * @param fileinput Point cloud (.OUT) filepath
 * @param fileoutput Tab-separated (.TSV) filepath, must already exist.
 * @param scale Scaling factor used to determine overall root system complexity
 */
void skel_and_features_pipeline(string fileinput, string fileoutput, float scale, map<string, bool> output_flags)
{
	string filestorename;

	// create a root model from the file //rtvox format
	MedialCurve root(fileinput, 1, 1);
	// Checkpoint message: root repair
	cout << "Repairing voxel set" << endl;
	root.repair();
	__gnu_cxx::hash_map<int, float> dist_tr;
	root.dt(dist_tr);
	//get a copy for feature computation
	MedialCurve skel(root);

	// write output of the original model
	cout << "Visual output of the original object" << endl;
	filestorename = fileinput.substr(0, fileinput.rfind('.')) + "_or";

<<<<<<< Updated upstream
	// Generate file format if specified at execution time
		// iv
=======
	// Generate file format if specified at execution time 
	// iv
>>>>>>> Stashed changes
	if (output_flags["iv"]) {
		printf("Type file = iv\n");
		writeVisibleIvWrlEff(filestorename, root, 1);
	}

	// wrl
	if (output_flags["wrl"]) {
		printf("Type file = wrl\n");
		writeVisibleIvWrlEff(filestorename, root, 2);
	}

	//create skeleton by thinning and filtering
	skel.createSkeleton(scale);
	//output skeleton to the file
	filestorename = fileinput.substr(0, fileinput.rfind('.')) + "_sk";
	cout << "Visual output of the skeleton" << endl;

	// iv
	if (output_flags["iv"]) {
		printf("Type file = iv\n");
		writeVisibleIvWrlEff(filestorename, skel, 1);
	}
	// wrl
	if (output_flags["wrl"]) {
		printf("Type file = wrl\n");
		writeVisibleIvWrlEff(filestorename, skel, 2);
	}

	//compute features
	vector<pair<string, double>> features;
	vector<pair<string, double>> bif_features;
	vector<pair<string, double>> e_features;
	vector<pair<string, double>> sk_features;
	cout << "Compute root features" << endl;
	root.computeFeatures(features, skel);
	cout << "Compute bifurcation cluster features" << endl;
	skel.getBifClusterFeatures(bif_features);
	cout << "Compute edge features" << endl;
	skel.getEdgesFeatures(e_features);
	cout << "Compute skeleton estimated features" << endl;
	skel.computeSkeletonEstimatedFeatures(sk_features, dist_tr);

	//output features into the file
	cout << "Writing computed features to '" << fileoutput << "'" << endl;
	string ffname = fileoutput;

	FILE *ff = fopen(ffname.c_str(), "a");
	size_t i;
	fprintf(ff, "\n%s\t", fileinput.c_str());
	for (i = 0; i < features.size(); ++i)
		fprintf(ff, "%.4f\t", features.at(i).second);
	for (i = 0; i < bif_features.size(); ++i)
		fprintf(ff, "%.4f\t", bif_features.at(i).second);
	for (i = 0; i < e_features.size(); ++i)
		fprintf(ff, "%.4f\t", e_features.at(i).second);
	for (i = 0; i < sk_features.size(); ++i)
		fprintf(ff, "%.4f\t", sk_features.at(i).second);
	fclose(ff);

	/*if( remove( "temp.temp" ) != 0 )
		perror( "Error deleting file" );
	else
		puts( "File successfully deleted" );*/

	//remove("C:\Olga\development\root3D_clean\Release\temp.temp");

	cout << "\nDONE. Exiting." << endl;
}
