// example: 
// g++ test.cpp -ldb_cxx -o dbd_test
//


#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <cstdlib>

#include <iomanip>
#include <string>

#include <db_cxx.h>

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

int main(int argc, char **argv)
{

	// 1. открыть БД.
	// 2. заполнить её данными субъект, предикат, объект
	// S -- {subject, "I am"}
	// P -- {predicate, "has"}
	// O -- {object, "world"}
	// 3. вывести данные.
	// 4. сохранить данные БД, в три разные вектора (значения по id) или map. 
	
	// 1. Open DB
	cout << "Open DB" << endl;

	//DbEnv env(0);
	Db db_s(NULL, 0);
	Db db_p(NULL, 0);
	Db db_o(NULL, 0);
	
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

		// 3. Output data of DB 
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
			cout << "P: " << (char *)key_p.get_data() << " : " << (char *)data_p.get_data() << endl;
			cout << "O: " << (char *)key_o.get_data() << " : " << (char *)data_o.get_data() << endl;		
		}
		//*/
		//if (cursor != NULL)
		cur_s->close();
		cur_p->close();
		cur_o->close();	
	
		// 4. Save DB data to maps
		// map subj_values;			
		// map pred_values;
		// map obj_values;		

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
		cout << "Closed." << endl;
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
	
  	return 0;
}
