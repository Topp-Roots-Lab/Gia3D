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

void root_graph_output(string filename)
{	
	string filestorename;

	float scale;
	char strsc[100];
	printf("Enter scale factor (suggested value appr. 2): ");
	scanf("%s", strsc);
	scale=(float)atof(strsc);	

	// create a root model from the file //rtvox format
	MedialCurve skel(filename,1,true);	
	skel.repair();
	writeVisibleIVEff("root.iv",skel);	
	skel.createSkeleton(scale);
	writeVisibleIVEff("root_skel.iv",skel);	
}


void skel_and_features(string filename, float scale)
{
	//printf("\nHello skel_and_features\n");
	
	string filestorename;	
	//float scale;
	//char strsc[100];
	//printf("Enter the scale factor value (suggested value is 2): ");
	//scanf("%s", &strsc);
	//scale=atof(strsc);	
	//scale=atof(strsc);	
	//scale=2.25f;

	// create a root model from the file //rtvox format
	MedialCurve root(filename,1,1);
	root.repair();
	__gnu_cxx::hash_map<int, float> dist_tr;
	root.dt(dist_tr);
	//get a copy for feature computation
	MedialCurve skel(root);

	filestorename = filename.substr(0, filename.rfind('.'))+"_or";	
	//string foldername="D:\\time_analysis\\QTL\\";	
	//int nlen=filename.find('.')-filename.find_last_of('\\')-1;
	//filestorename = foldername+filename.substr(filename.find_last_of('\\')+1,nlen)+"_or";		

	// write output of the original model
	//printf("\nVisual output of the original object...\n");
	//filestorename = filename.substr(0, filename.find('.'))+"_or";	
	writeVisibleIVEff(filestorename, root);
	//create skeleton by thinning and filtering
	skel.createSkeleton(scale);
	//output skeleton to the file
	filestorename = filename.substr(0, filename.rfind('.'))+"_sk";	
	//filestorename = foldername+filename.substr(filename.find_last_of('\\')+1,nlen)+"_sk";	
	// write output of the original model
	//printf("\nVisual output of the skeleton...\n");
	writeVisibleIVEff(filestorename, skel);
	
	//compute features	
	vector<pair<string,double> > features;
	vector<pair<string,double> > bif_features;
	vector<pair<string,double> > e_features;
	vector<pair<string,double> > sk_features;
	root.computeFeatures(features, skel);
	skel.getBifClusterFeatures(bif_features);
	skel.getEdgesFeatures(e_features);
	skel.computeSkeletonEstimatedFeatures(sk_features, dist_tr);
	//output features into the file
	//string ffname=filename.substr(0, filename.find('.'))+"_features.txt";	
	//string ffname="D:\\time_analysis\\QTL\\features.cvs";	
	string ffname="features.cvs";	
	FILE *ff=fopen(ffname.c_str(), "a");
	//map<string,double>::iterator itmap;
	size_t i;
	fprintf(ff,"\n%s\t",filename.c_str());
	for(i=0; i<features.size(); ++i)
		//fprintf(ff,"%s: \t%.4f\n",itmap->first.c_str(), itmap->second);
		fprintf(ff,"%.4f\t", features.at(i).second);
	for(i=0; i<bif_features.size(); ++i)	
		//fprintf(ff,"%s: \t%.4f\n",itmap->first.c_str(), itmap->second);
		fprintf(ff,"%.4f\t", bif_features.at(i).second);	
	for(i=0; i<e_features.size(); ++i)
		//fprintf(ff,"%s: \t%.4f\n",itmap->first.c_str(), itmap->second);
		fprintf(ff,"%.4f\t", e_features.at(i).second);	
	for(i=0; i<sk_features.size(); ++i)
		//fprintf(ff,"%s: \t%.4f\n",itmap->first.c_str(), itmap->second);
		fprintf(ff,"%.4f\t", sk_features.at(i).second);
	fclose(ff);
	
	/*if( remove( "temp.temp" ) != 0 )
		perror( "Error deleting file" );
	else
		puts( "File successfully deleted" );*/

	//remove("C:\Olga\development\root3D_clean\Release\temp.temp");
}

void skel_and_features_pipeline(string fileinput, string fileoutput, float scale)
{
	printf("\nCall skel_and_features_pipeline\n");
	
	string filestorename;	

	// create a root model from the file //rtvox format
	MedialCurve root(fileinput,1,1);
	root.repair();
	__gnu_cxx::hash_map<int, float> dist_tr;
	root.dt(dist_tr);
	//get a copy for feature computation
	MedialCurve skel(root);		

	// write output of the original model
	printf("\nVisual output of the original object ...\n");
	filestorename = fileinput.substr(0, fileinput.rfind('.'))+"_or";

	//writeVisibleIVEff(filestorename, root);
        // iv
        printf("Type file = iv\n");
        writeVisibleIvWrlEff(filestorename, root, 1); 
        // wrl
        printf("Type file = wrl\n");
        writeVisibleIvWrlEff(filestorename, root, 2); 

	//create skeleton by thinning and filtering
	skel.createSkeleton(scale);
	//output skeleton to the file
	filestorename = fileinput.substr(0, fileinput.rfind('.'))+"_sk";	
	printf("\nVisual output of the skeleton ...\n");

	//writeVisibleIVEff(filestorename, skel);
        // iv
        printf("Type file = iv\n");
        writeVisibleIvWrlEff(filestorename, skel, 1); 
        // wrl
        printf("Type file = wrl\n");
        writeVisibleIvWrlEff(filestorename, skel, 2); 	

	//compute features	
	vector<pair<string,double> > features;
	vector<pair<string,double> > bif_features;
	vector<pair<string,double> > e_features;
	vector<pair<string,double> > sk_features;
	root.computeFeatures(features, skel);
	skel.getBifClusterFeatures(bif_features);
	skel.getEdgesFeatures(e_features);
	skel.computeSkeletonEstimatedFeatures(sk_features, dist_tr);

	//output features into the file
        printf("\nWriting features into the file ...\n");
        string ffname=fileoutput;
	
	FILE *ff=fopen(ffname.c_str(), "a");
	size_t i;
	fprintf(ff,"\n%s\t",fileinput.c_str());
	for(i=0; i<features.size(); ++i)
		//fprintf(ff,"%s: \t%.4f\n",itmap->first.c_str(), itmap->second);
		fprintf(ff,"%.4f\t", features.at(i).second);
	for(i=0; i<bif_features.size(); ++i)	
		//fprintf(ff,"%s: \t%.4f\n",itmap->first.c_str(), itmap->second);
		fprintf(ff,"%.4f\t", bif_features.at(i).second);	
	for(i=0; i<e_features.size(); ++i)
		//fprintf(ff,"%s: \t%.4f\n",itmap->first.c_str(), itmap->second);
		fprintf(ff,"%.4f\t", e_features.at(i).second);	
	for(i=0; i<sk_features.size(); ++i)
		//fprintf(ff,"%s: \t%.4f\n",itmap->first.c_str(), itmap->second);
		fprintf(ff,"%.4f\t", sk_features.at(i).second);
	fclose(ff);
	
	/*if( remove( "temp.temp" ) != 0 )
		perror( "Error deleting file" );
	else
		puts( "File successfully deleted" );*/

	//remove("C:\Olga\development\root3D_clean\Release\temp.temp");

	cout << "\nDONE" << endl;
}



void root_features_range(string filename)
{
	printf("Feature range will be printed to feature.txt file.\n");

	string filestorename;		
	

	// create a root model from the file //rtvox format
	MedialCurve root(filename,1);
	root.repair();
	__gnu_cxx::hash_map<int,float> dtmap;
	root.dt(dtmap);
	string ffname=filename.substr(0, filename.find('.'))+"_features.txt";	
	FILE *ff=fopen(ffname.c_str(), "w");
	
	MedialCurve skel(root);
	__gnu_cxx::hash_map<int, float> distR;	
	skel.thinning(distR);			
	//filter the skeleton
	//writeVisibleIVEff("root_skel_thin.iv",skel);	
	
	for(float scale=0.0f; scale<=4; scale=scale+0.1f)
	{
		//get a copy for skeleton computation
		MedialCurve skel_sc(skel);	
		skel_sc.setScale();
		skel_sc.scaleAxis(scale);	
		//output skeleton to the file
		//filestorename = filename.substr(0, filename.find('.'))+"_sk";	
		// write output of the original model
		//printf("\nVisual output of the skeleton...\n");
		//writeVisibleIVEff(filestorename, skel);
	
		//compute features	
		vector<pair<string,double> > skel_features;		
		vector<pair<string,double> > e_features;
		
		skel_sc.getEdgesFeatures(e_features);
		skel_sc.computeSkeletonEstimatedFeatures(skel_features,dtmap);
		//output features into the file
				
		size_t i;
		if(scale==0.0)
		{
			fprintf(ff,"Scale\t\t");
			for(i=0; i<e_features.size(); ++i)			
				fprintf(ff,"%s\t\t", e_features.at(i).first.c_str());
			for(i=0; i<skel_features.size(); ++i)			
				fprintf(ff,"%s\t\t", skel_features.at(i).first.c_str());
			fprintf(ff,"\n");
		}
		fprintf(ff,"%.3f\t\t",scale);		
		for(i=0; i<e_features.size(); ++i)		
			fprintf(ff,"%.3f\t\t", e_features.at(i).second);	
		for(i=0; i<skel_features.size(); ++i)			
				fprintf(ff,"%s\t\t", skel_features.at(i).first.c_str());
		fprintf(ff,"\n");		
	}
	fclose(ff);
}


void root_repair(string filename)
{
	// create a root model from the file //rtvox format
	MedialCurve root(filename,1,1);	
	writeVisibleIVEff("root_mesh_out.iv",root);	
	root.repair();
}

