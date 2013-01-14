//============================================================================
// Name        : bdb_vfs.cpp
// Author      : h05t
// Version     :
// Copyright   : Your copyright notice
// Description : Test Berkeley DB mapping to VFS using fusekit
//============================================================================

// example:
// g++ test.cpp -ldb_cxx -o dbd_test
//
// g++ -Wall test.cpp `pkg-config --cflags --libs fuse` -DFUSE_USE_VERSION=27 -D_FILE_OFFSET_BITS=64 -ldb_cxx -o testfs
// ==
// g++ -Wall test.cpp -DFUSE_USE_VERSION=27 -D_FILE_OFFSET_BITS=64 -I/usr/include/fuse -pthread -lfuse -lrt -ldl -ldb_cxx -o testfs

#define FUSE_USE_VERSION 27

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


// data
Db db_s(NULL, 0);
Db db_p(NULL, 0);
Db db_o(NULL, 0);

vector<string> subj_vector;
vector<string> pred_vector;
vector<string> obj_vector;


int createDb();
int closeDb();
int insertDbData();
int printDbData();
int createDbVfs(int argc, char *argv[]);

int createDb()
{
    try
    {
        cout << "Open DB" << endl;

        u_int32_t flags = DB_CREATE | DB_TRUNCATE;

        //u_int32_t flags = DB_RDONLY;
        db_s.set_flags(DB_DUP);
        db_p.set_flags(DB_DUP);
        db_o.set_flags(DB_DUP);

        // only create
        db_s.open(NULL, "s.db", NULL, DB_BTREE, flags, 0);
        db_p.open(NULL, "p.db", NULL, DB_BTREE, flags, 0);
        db_o.open(NULL, "o.db", NULL, DB_BTREE, flags, 0);

        return 0;
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
}

int closeDb()
{
    try
    {
    	cout << "Closing DB ..." << endl;
    	//if (db != NULL) // if DB pointer
    	//{
    	db_s.close(0);
    	db_p.close(0);
    	db_o.close(0);
    	//}

    	cout << "DB closed!" << endl;
    	return 0;
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
}

int insertDbData()
{
    try
    {
		//if -- проверка на открытие БД

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

		//if (flags != DB_RDONLY)
		//{
		db_s.put(NULL, &subj_key, &subj1_value, 0);
		db_s.put(NULL, &subj_key, &subj2_value, 0);
		db_s.put(NULL, &subj_key, &subj3_value, 0);

		db_p.put(NULL, &pred_key, &pred1_value, 0);
		db_p.put(NULL, &pred_key, &pred2_value, 0);
		db_p.put(NULL, &pred_key, &pred3_value, 0);

		db_o.put(NULL, &obj_key, &obj1_value, 0);
		db_o.put(NULL, &obj_key, &obj2_value, 0);
		db_o.put(NULL, &obj_key, &obj3_value, 0);
		// }

		return 0;
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
}

// Output data of DB and Save DB data to vectors
int printDbData()
{
    try
    {
    	cout << "Output data ..." << endl;
    	// Acquire a cursor for the table.
    	Dbc *cur_s, *cur_p, *cur_o;
    	db_s.cursor(NULL, &cur_s, 0);
    	db_p.cursor(NULL, &cur_p, 0);
    	db_o.cursor(NULL, &cur_o, 0);

    	// Walk through the table, printing the key/data pairs.
    	Dbt key_s, key_p, key_o, data_s, data_p, data_o;
    	while ((cur_s->get(&key_s, &data_s, DB_NEXT) == 0) &&
    			(cur_p->get(&key_p, &data_p, DB_NEXT) == 0) &&
    			(cur_o->get(&key_o, &data_o, DB_NEXT) == 0))
    	{
    		cout << "S: " << (char *)key_s.get_data() << " : " << (char *)data_s.get_data() << endl;
    		subj_vector.push_back((char *)data_s.get_data());
    		cout << "P: " << (char *)key_p.get_data() << " : " << (char *)data_p.get_data() << endl;
    		pred_vector.push_back((char *)data_p.get_data());
    		cout << "O: " << (char *)key_o.get_data() << " : " << (char *)data_o.get_data() << endl;
    		obj_vector.push_back((char *)data_o.get_data());
    	}

        cur_s->close();
        cur_p->close();
        cur_o->close();

        return 0;
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
}

// TODO
// -- implement new structure of VFS
// --

// Create test VFS
int createDbVfs(int argc, char *argv[])
{
	// fusekit daemon
	fusekit::daemon<>& daemon = fusekit::daemon<>::instance();

	cout << "Creating test VFS ... " << endl;

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
	for (int i = 0; i < (int) predicates_dirs.size(); ++i)
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

	closeDb();

	return daemon.run(argc,argv);
}


int main(int argc, char *argv[])
{
	int key;

	while(true)
	{
		cout << "Menu:" << endl;
		cout << "1 -- Create DB" << endl;
		cout << "2 -- Load data to DB" << endl;
		cout << "3 -- Print data from DB" << endl;
		cout << "4 -- Close DB" << endl;
		cout << "5 -- Create DB VFS" << endl;

		cout << "Enter key: "; cin >> key;
		switch(key)
		{
		case 1:
			createDb();
			break;
		case 2:
			insertDbData();
			break;
		case 3:
			printDbData();
			break;
		case 4:
			closeDb();
			break;
		case 5:
			createDbVfs(argc, argv);
			break;
		default:
			cout << "Wrong key!!!" << endl;
			break;
		}
	}

	return 0;
}
