// example: 
// g++ test.cpp -ldb_cxx -o dbd_test
//
// g++ -Wall test.cpp `pkg-config --cflags --libs fuse` -DFUSE_USE_VERSION=27 -D_FILE_OFFSET_BITS=64 -ldb_cxx -o testfs
 

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>

#include <algorithm>
#include <string>
#include <map>
#include <vector>

// berkeley db
#include <db_cxx.h>

// fusekit
#include <fusekit/daemon.h>
#include <fusekit/default_directory.h>
#include <fusekit/stream_object_file.h>
#include <fusekit/new_creator.h>

#ifdef _WIN32
extern "C" {
  extern int getopt(int, char * const *, const char *);
  extern char *optarg;
}
#else
#include <unistd.h>
#endif

using namespace std;

int usage()
{
    	std::cout << "test -c [DB_CREATE]" << " -- [only create and insert data to DB]." << std::endl;
	std::cout << "test -r [DB_RDONLY]" << " -- [only read data from DB]." << std::endl;
    	
	return (-1);
}

int main(int argc, char *argv[])
{
	// 1. Open DB
	cout << "Open DB" << endl;

	//DbEnv env(0);
	Db db_s(NULL, 0);
	Db db_p(NULL, 0);
	Db db_o(NULL, 0);

	// fusekit daemon
	fusekit::daemon<>& daemon = fusekit::daemon<>::instance();
	
	//u_int32_t flags = DB_CREATE;
	int ch;
	u_int32_t flags = DB_RDONLY;	

	// Parse the command line arguments
    	while ((ch = getopt(argc, argv, "c:r:")) != EOF)
		switch (ch) {
		case 'c':
		    cout << "Creating and inserting data to DB ..." << endl;
		    flags = DB_CREATE | DB_TRUNCATE;
		    break;
		case 'r':
		    cout << "Reading data from DB ..." << endl;
		    flags = DB_RDONLY;
		    break;
		case '?':
		default:
		    return (usage());
		    break;
		}		

  	try
  	{
		db_s.set_flags(DB_DUP);
		db_p.set_flags(DB_DUP);
		db_o.set_flags(DB_DUP);		
		// only create
    		db_s.open(NULL, "s.db", NULL, DB_BTREE, flags, 0);
		db_p.open(NULL, "p.db", NULL, DB_BTREE, flags, 0);
		db_o.open(NULL, "o.db", NULL, DB_BTREE, flags, 0);

		// 2. Insert data
		cout << "Inserting data ..." << endl;
		string subject("subject");
	  	string subject_1("I");
	  	string subject_2("Kirill");
		string subject_3("Katya");

		string predicate("predicate");
		string predicate_1("am");
	  	string predicate_2("has");
		string predicate_3("sister of");
			
		string object("object");
		string object_1("Kirill.");
  		string object_2("a sister.");
		string object_3("Kirill");
	
		// subjects    		
		Dbt subj_key(const_cast<char*>(subject.data()), subject.size());
    		Dbt subj1_value(const_cast<char*>(subject_1.data()), subject_1.size()+1);
    		Dbt subj2_value(const_cast<char*>(subject_2.data()), subject_2.size()+1);
		Dbt subj3_value(const_cast<char*>(subject_3.data()), subject_3.size()+1);
    		// predicates    		
		Dbt pred_key(const_cast<char*>(predicate.data()), predicate.size());
    		Dbt pred1_value(const_cast<char*>(predicate_1.data()), predicate_1.size()+1);
    		Dbt pred2_value(const_cast<char*>(predicate_2.data()), predicate_2.size()+1);
		Dbt pred3_value(const_cast<char*>(predicate_3.data()), predicate_3.size()+1);
		// objects    		
		Dbt obj_key(const_cast<char*>(object.data()), object.size());
    		Dbt obj1_value(const_cast<char*>(object_1.data()), object_1.size()+1);
    		Dbt obj2_value(const_cast<char*>(object_2.data()), object_2.size()+1);
    		Dbt obj3_value(const_cast<char*>(object_3.data()), object_3.size()+1);

		if (flags != DB_RDONLY)
		{		
			db_s.put(NULL, &subj_key, &subj1_value, 0);
			db_s.put(NULL, &subj_key, &subj2_value, 0);
			db_s.put(NULL, &subj_key, &subj3_value, 0);

			db_p.put(NULL, &pred_key, &pred1_value, 0);
			db_p.put(NULL, &pred_key, &pred2_value, 0);
			db_p.put(NULL, &pred_key, &pred3_value, 0);

			db_o.put(NULL, &obj_key, &obj1_value, 0);
			db_o.put(NULL, &obj_key, &obj2_value, 0);
			db_o.put(NULL, &obj_key, &obj3_value, 0);
		}

		// 3. Output data of DB and Save DB data to vectors
		cout << "Output data ..." << endl;
		// Acquire a cursor for the table.
		Dbc *cur_s, *cur_p, *cur_o;
		db_s.cursor(NULL, &cur_s, 0);
		db_p.cursor(NULL, &cur_p, 0);
		db_o.cursor(NULL, &cur_o, 0);

		multimap<string, string> multi_subjs;
		multimap<string, string> multi_preds;
		multimap<string, string> multi_objs;

		vector<string> subj_vector;
		vector<string> pred_vector;
		vector<string> obj_vector;

		// Walk through the table, printing the key/data pairs.
		Dbt key_s, key_p, key_o, data_s, data_p, data_o;
		while ((cur_s->get(&key_s, &data_s, DB_NEXT) == 0) &&
		       (cur_p->get(&key_p, &data_p, DB_NEXT) == 0) &&
		       (cur_o->get(&key_o, &data_o, DB_NEXT) == 0)) 		
		{
		   	cout << "S: " << (char *)key_s.get_data() << " : " << (char *)data_s.get_data() << endl;
			multi_subjs.insert(pair<string, string>((char *)key_s.get_data(), (char *)data_s.get_data()));
			subj_vector.push_back((char *)data_s.get_data());
		   	cout << "P: " << (char *)key_p.get_data() << " : " << (char *)data_p.get_data() << endl;
			multi_preds.insert(pair<string, string>((char *)key_p.get_data(), (char *)data_p.get_data()));
			pred_vector.push_back((char *)data_p.get_data());
		   	cout << "O: " << (char *)key_o.get_data() << " : " << (char *)data_o.get_data() << endl;
			multi_objs.insert(pair<string, string>((char *)key_o.get_data(), (char *)data_o.get_data()));
			obj_vector.push_back((char *)data_o.get_data());
		}


		cout << "subj vector contains:\n";
		vector<string>::iterator it_subj;
  		for (it_subj = subj_vector.begin(); it_subj != subj_vector.end(); ++it_subj)
    			cout << (*it_subj) << ", ";

		cout << endl;
		cout << "pred vector contains:\n";
		vector<string>::iterator it_pred;
  		for (it_pred = pred_vector.begin(); it_pred != pred_vector.end(); ++it_pred)
    			cout << (*it_pred) << ", ";

		cout << endl;
		cout << "obj vector contains:\n";
		vector<string>::iterator it_obj;
  		for (it_obj = obj_vector.begin(); it_obj != obj_vector.end(); ++it_obj)
    			cout << (*it_obj) << ", ";
		cout << endl;

		cout << "multi_subj contains:\n";
		map<string,string>::iterator multisubj_it;
		for (multisubj_it = multi_subjs.begin(); multisubj_it != multi_subjs.end(); ++multisubj_it)
	       		cout << "  [" << (*multisubj_it).first << ", " << (*multisubj_it).second << "]" << endl;
		cout << "multi_pred contains:\n";
		map<string,string>::iterator multipred_it;
		for (multipred_it = multi_preds.begin(); multipred_it != multi_preds.end(); ++multipred_it)
	       		cout << "  [" << (*multipred_it).first << ", " << (*multipred_it).second << "]" << endl;
		cout << "multi_obj contains:\n";
		map<string,string>::iterator multiobj_it;
		for (multiobj_it = multi_objs.begin(); multiobj_it != multi_objs.end(); ++multiobj_it)
	       		cout << "  [" << (*multiobj_it).first << ", " << (*multiobj_it).second << "]" << endl;

		
		cur_s->close();
		cur_p->close();
		cur_o->close();	
	
		// 5. Close DB
		cout << "Closing DB ..." << endl;
		// закрываем БД
    		//if (db != NULL) // if DB pointer 
    		//{
      			db_s.close(0);
			db_p.close(0);
			db_o.close(0);
      			//delete pdb;
      		//}
    		
		//env.close(0);
		cout << "DB closed!" << endl;

		// -------------------------------------------------------------------------------------------	
		// 6. Create test VFS

		cout << "Creating test VFS ... " << endl;

		// TODO
		// 1. Create new FS structure using fusekit (look article)
		// 2. testing, testing, testing ....
		// 3. Check strange fusekit error (or write to author)
		// - open preds dir and after moving to root, dispay error window: The folder contents couldnt be displayed. Sorry, could not display all the contents of "testvfs": Transport endpoint is not connected 
		// что то с fuse!

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

		for(vector<string>::iterator it = subj_vector.begin(); it < subj_vector.end(); ++it) 
			cout << *it << endl;
		cout << "----------------------------" << endl;
	
		std::vector< fusekit::default_directory<>::type > predicates_dirs;

		// subjects --> p1_d, p2_d, p3_d ... --> o1_f, o2_f, o3_f ...
		for(vector<string>::iterator it = pred_vector.begin(); it < pred_vector.end(); ++it) 
		{
			subjects_dir.add_directory((*it).c_str(), fusekit::make_default_directory());
			predicates_dirs.push_back(subjects_dir.add_directory((*it).c_str(), fusekit::make_default_directory()));
		}

		cout << "predicates_dirs size: " << predicates_dirs.size() << endl;

		// p1_d, p2_d, p3_d ... --> o1_f, o2_f, o3_f ...
		for (int i = 0; i < predicates_dirs.size(); ++i) 
			predicates_dirs.at(i).add_file(obj_vector.at(i).c_str(), fusekit::make_iostream_object_file(obj_vector.at(i)));


		cout << "Predicates" << endl;
		cout << "----------------------------" << endl;
		// predicates --> p1_f, p2_f, p3_f ...
   		for(vector<string>::iterator it = pred_vector.begin(); it < pred_vector.end(); ++it) 
		{
      			cout << *it << endl;
      			predicates_dir.add_file((*it).c_str(), fusekit::make_iostream_object_file(*it));
   		}

		cout << "----------------------------" << endl;	
		cout << "Objects" << endl;
		cout << "----------------------------" << endl;

		for(vector<string>::iterator it = obj_vector.begin(); it < obj_vector.end(); ++it) 
			cout << *it << endl;
	
		cout << "----------------------------" << endl;
		// objects --> p1_d, p2_d, p3_d ... --> s1_f, s2_f, s3_f ...
		for(vector<string>::iterator it = pred_vector.begin(); it < pred_vector.end(); ++it)
			objects_dir.add_directory((*it).c_str(), fusekit::make_default_directory());

		cout << "VFS created!" << endl;

		// -------------------------------------------------------------------------------------------		

	} 
  	catch (DbException &e) 
  	{
    		cerr << "DbException: " << e.what() << endl;
    		return -1;
  	} 
  	catch (std::exception &e) 
  	{
    		cerr << e.what() << endl;
    		return -1;
  	}
	
  	return daemon.run(argc,argv);
}
