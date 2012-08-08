/*

Compile: 
   make
else:
   g++ -Wall main.cpp db.cpp `pkg-config --cflags --libs fuse` -DFUSE_USE_VERSION=27 -D_FILE_OFFSET_BITS=64 -lsqlite3 -o sibfs

*/


#include <iostream>
#include <string.h>
#include <algorithm>
#include "db.h"

#include <fusekit/daemon.h>
#include <fusekit/default_directory.h>
#include <fusekit/stream_object_file.h>
#include <fusekit/new_creator.h>

using namespace std;



int main(int argc, char *argv[])
{
	DB *db = new DB("X");
   	db->getAllTriplesText(db);

   	fusekit::daemon<>& daemon = fusekit::daemon<>::instance();

 
   	fusekit::default_directory<>::type& objects_dir = 
		daemon.root().add_directory("objects", fusekit::make_default_directory()); // link to objects dir
   	fusekit::default_directory<>::type& subjects_dir = 
		daemon.root().add_directory("subjects", fusekit::make_default_directory()); // link to subjects dir
   	fusekit::default_directory<>::type& predicates_dir = 
		daemon.root().add_directory("predicates", fusekit::make_default_directory()); // link to predicates dir
	
 
	// root/
	cout << "----------------------------" << endl;
	cout << "Subjects" << endl;
	cout << "----------------------------" << endl;
	
	for(vector<string>::iterator it = db->S.begin(); it < db->S.end(); ++it) cout << *it << endl;
	cout << "----------------------------" << endl;
	
	std::vector< fusekit::default_directory<>::type > predicates_dirs;

	// subjects --> p1_d, p2_d, p3_d ... --> o1_f, o2_f, o3_f ...
	for(vector<string>::iterator it = db->P.begin(); it < db->P.end(); ++it) {

		subjects_dir.add_directory((*it).c_str(), fusekit::make_default_directory());


		predicates_dirs.push_back(subjects_dir.add_directory((*it).c_str(), fusekit::make_default_directory()));
	}

	cout << "predicates_dirs size: " << predicates_dirs.size() << endl;

	//*
	// 1. Выявить что записывается в predicates_dirs, То ли нам нужно и туда ли оно указывает???
	// 2. Как сохранить ссылки на созданные папки предикатов??? 
	// p1_d, p2_d, p3_d ... --> o1_f, o2_f, o3_f ...
	for (int i = 0; i < predicates_dirs.size(); ++i) {
		//for(vector<string>::iterator it_o = db->O.begin(); it_o < db->O.end(); ++it_o) {
		//for(vector< fusekit::default_directory<>::type >::iterator it = predicates_dirs.begin(); 
			//it < predicates_dirs.end(); ++it) {
				//cout << *it << endl;
				//predicates_dirs.at(i).add_file((*it_o).c_str(), fusekit::make_iostream_object_file(*it_o));
				predicates_dirs.at(i).add_file(db->O.at(i).c_str(), fusekit::make_iostream_object_file(db->O.at(i)));
		//}
	}
	//*/


	cout << "Predicates" << endl;
	cout << "----------------------------" << endl;

	// predicates --> p1_f, p2_f, p3_f ...
   	for(vector<string>::iterator it = db->P.begin(); it < db->P.end(); ++it) {
      		cout << *it << endl;
      		predicates_dir.add_file((*it).c_str(), fusekit::make_iostream_object_file(*it));
   	}

	cout << "----------------------------" << endl;	
	cout << "Objects" << endl;
	cout << "----------------------------" << endl;

	for(vector<string>::iterator it = db->O.begin(); it < db->O.end(); ++it) cout << *it << endl;
	
	cout << "----------------------------" << endl;
	// objects --> p1_d, p2_d, p3_d ... --> s1_f, s2_f, s3_f ...
	for(vector<string>::iterator it = db->P.begin(); it < db->P.end(); ++it) {

		objects_dir.add_directory((*it).c_str(), fusekit::make_default_directory());
	}


   	return daemon.run(argc,argv);
}
