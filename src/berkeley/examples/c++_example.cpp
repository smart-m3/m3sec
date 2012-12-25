// example: 
// g++ c++_example.cpp -ldb_cxx -o c++_example
//


#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <iomanip>
#include <string>

#include <db_cxx.h>

using namespace std;

// вспомогательная БД
const char* kDatabaseName = "access.db";

int main() 
{
  string fruit("fruit");
  string apple("apple");
  string orange("orange");

  DbEnv env(0);
  Db* pdb;

  try 
  {
    env.set_error_stream(&cerr);
    // look flags
    env.open("/home/host/thesis/redland/berkeley/src", DB_CREATE | DB_INIT_MPOOL, 0);

    pdb = new Db(&env, 0);
    // If you want to support duplicated records and make duplicated
    // records sorted by data, you need to call:
    //   pdb->set_flags(DB_DUPSORT);
    // Note that only Btree-typed database supports sorted duplicated
    // records

    // If the database does not exist, create it.  If it exists, clear
    // its content after openning.
    pdb->open(NULL, "access.db", NULL, DB_BTREE, DB_CREATE | DB_TRUNCATE, 0);

    // DB data
    Dbt key(const_cast<char*>(fruit.data()), fruit.size());
    Dbt value(const_cast<char*>(apple.data()), apple.size()+1);
    // insert data to DB
    pdb->put(NULL, &key, &value, 0);

    Dbt value_orange(const_cast<char*>(orange.data()), orange.size()+1);
    pdb->put(NULL, &key, &value_orange, 0);

    // You need to set ulen and flags=DB_DBT_USERMEM to prevent Dbt
    // from allocate its own memory but use the memory provided by you.
    char buffer[1024];
    Dbt data;
    data.set_data(buffer);
    data.set_ulen(1024);
    data.set_flags(DB_DBT_USERMEM);
    if (pdb->get(NULL, &key, &data, 0) == DB_NOTFOUND) 
    {
      cerr << "Not found" << endl;
    } 
    else 
    {
      cout << "Found: " << buffer << endl;
    }

    // закрываем старую БД, создаем новую и удаляем БД
    if (pdb != NULL) 
    {
      pdb->close(0);
      delete pdb;
      // You have to close and delete an exisiting handle, then create
      // a new one before you can use it to remove a database (file).
      pdb = new Db(NULL, 0);
      pdb->remove("/home/host/thesis/redland/berkeley/src/access.db", NULL, 0);
      delete pdb;
    }
    env.close(0);
  } 
  catch (DbException& e) 
  {
    cerr << "DbException: " << e.what() << endl;
    return -1;
  } 
  catch (std::exception& e) 
  {
    cerr << e.what() << endl;
    return -1;
  }

  return 0;
}
