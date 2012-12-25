// example: 
// g++ c_example.c -ldb_cxx -o с_example
//


#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <iomanip>
#include <string>

#define HAVE_CXX_STDHEADERS
#include <db_cxx.h>

using namespace std;

const char* kDatabaseName = "access.db";

int main() 
{
  DB *dbp;
  DBT key, data;
  int ret, t_ret;

  if ((ret = db_create(&dbp, NULL, 0)) != 0) 
  {
    cerr << "db_create:" << db_strerror(ret) << endl;
    exit (1);
  }

  if ((ret = dbp->open(dbp, NULL, kDatabaseName, "",
                       DB_BTREE, DB_CREATE, 0664)) != 0) 
  {
    dbp->err(dbp, ret, "%s", kDatabaseName);
    goto err;
  }

  memset(&key, 0, sizeof(key));
  memset(&data, 0, sizeof(data));
  key.data = (char*)"fruit";
  key.size = sizeof("fruit");
  data.data = (char*)"apple";
  data.size = sizeof("apple");

  if ((ret = dbp->put(dbp, NULL, &key, &data, 0)) == 0)
    cout << "db: " << (char*)key.data << ": key stored.\n";
  else 
  {
    dbp->err(dbp, ret, "DB->put");
    goto err;
  }

  if ((ret = dbp->get(dbp, NULL, &key, &data, 0)) == 0)
    cout << "db: " << (char*)key.data
         << ": key retrieved: data was " << (char *)data.data << endl;
  else 
  {
    dbp->err(dbp, ret, "DB->get");
    goto err;
  }

  if ((ret = dbp->del(dbp, NULL, &key, 0)) == 0)
    cout << "db: " << (char*)key.data << " key was deleted.\n";
  else 
  {
    dbp->err(dbp, ret, "DB->del");
    goto err;
  }

  if ((ret = dbp->get(dbp, NULL, &key, &data, 0)) == 0)
    cout << "db: " << (char*)key.data << ": key retrieved: data was "
         << (char *)data.data << endl;
  else
    dbp->err(dbp, ret, "DB->get");

err:
  if ((t_ret = dbp->close(dbp, 0)) != 0 && ret == 0)
    ret = t_ret;

  return ret;
}
