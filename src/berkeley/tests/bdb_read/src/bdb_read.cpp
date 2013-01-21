//============================================================================
// Name        : bdb_read.cpp
// Author      : h05t
// Version     :
// Copyright   : Your copyright notice
// Description : Berkeley DB reading
//============================================================================

// making:
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

using namespace std;

//
/*
 *
 *
 */


int main()
{
    u_int32_t env_flags = DB_CREATE |       // If the environment does not exist, create it.
                          DB_INIT_MPOOL;    // Initialize the in-memory cache.

    //std::string envHome("./");
    std::string envHome("/home/kirill");

    u_int32_t db_flags = DB_RDONLY;   // only read

    std::string dbName("X-po2s.db");
    //std::string dbName("X-so2p.db");
    //std::string dbName("X-sp2o.db");

    DbEnv myEnv(0);
    Db *myDb;       // Instantiate the Db object

    Dbc *cursorp;   // cursor

    try
    {
        cout << "X-po2s.db output:" << endl;

        myEnv.open(envHome.c_str(), env_flags, 0);

        myDb = new Db(&myEnv, 0);

        myDb->open(NULL, dbName.c_str(), NULL, DB_BTREE, db_flags, 0);

        // Get a cursor
        myDb->cursor(NULL, &cursorp, 0);

        Dbt key, data;
        // Position the cursor to the first record in the database whose
        // key and data begin with the correct strings.
        int ret = cursorp->get(&key, &data, DB_NEXT);
        while (ret != DB_NOTFOUND)
        {
            cout << "..." << endl;
            std::cout << "key: " << (char *)key.get_data()
                      << "data: " << (char *)data.get_data()<< std::endl;
            ret = cursorp->get(&key, &data, DB_NEXT);
        }

        // Cursors must be closed
        if (cursorp != NULL)
            cursorp->close();


        if (myDb != NULL) {
            myDb->close(0);
        }
        myEnv.close(0);

        cout << "Closing ..." << endl;
    }
    // Must catch both DbException and std::exception
    catch(DbException &e)
    {
        myDb->err(e.get_errno(), "Database open failed %s", dbName.c_str());
        throw e;
    }
    catch(std::exception &e)
    {
        // No DB error number available, so use errx
        myDb->errx("Error opening database: %s", e.what());
        throw e;
    }


	return 0;
}
















