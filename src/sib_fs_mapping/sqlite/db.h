/*

SQLite DB
  - get data from DB and query to vector

*/

#ifndef __DB_H__
#define __DB_H__

#include <string>
#include <vector>
#include <sqlite3.h>

using namespace std;

class DB
{
public:
	DB(char* filename);
	~DB();
	
	bool open(char* filename);
	vector<vector<string> > query(char* query);
	void close();

	void getAllTriplesText(DB *db);
	
	vector<string> S, P, O;

private:
	sqlite3 *database;

	
};

#endif
