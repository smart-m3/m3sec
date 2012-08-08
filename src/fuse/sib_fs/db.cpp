/*

	SQLite DB

*/


#include <iostream>
#include "db.h"



DB::DB(char* filename)
{
	database = NULL;
	open(filename);
}

DB::~DB()
{
}

bool DB::open(char* filename)
{
	if(sqlite3_open(filename, &database) == SQLITE_OK)
		return true;
		
	return false;   
}

vector<vector<string> > DB::query(char* query)
{
	sqlite3_stmt *statement;
	vector<vector<string> > results;

	if(sqlite3_prepare_v2(database, query, -1, &statement, 0) == SQLITE_OK)
	{
		int cols = sqlite3_column_count(statement);
		int result = 0;
		while(true)
		{
			result = sqlite3_step(statement);
			
			if(result == SQLITE_ROW)
			{
				vector<string> values;
				for(int col = 0; col < cols; col++)
				{
					values.push_back((char*)sqlite3_column_text(statement, col));
				}
				results.push_back(values);
			}
			else
			{
				break;   
			}
		}
	   
		sqlite3_finalize(statement);
	}
	
	string error = sqlite3_errmsg(database);
	if(error != "not an error") 
		cout << query << " " << error << endl;
	
	return results;  
}

void DB::getAllTriplesText(DB *db)
{
	//db = new DB("X");
	
	// select all triples and it's text data
	vector< vector<string> > triples = db->query("SELECT * FROM triple;");
	vector< vector<string> > nodes_id_str = db->query("SELECT id, str FROM node;");

	// get strs from IDs
	for(vector< vector<string> >::iterator it = triples.begin(); it < triples.end(); ++it) 
	{
		vector<string> triples = *it;
		for(vector< vector<string> >::iterator it_str = nodes_id_str.begin(); it_str < nodes_id_str.end(); ++it_str) 
		{
			vector<string> str = *it_str;
			if (triples.at(0) == str.at(0))	// S
				db->S.push_back(str.at(1));
			else if (triples.at(1) == str.at(0)) // P
				db->P.push_back(str.at(1));
			else if (triples.at(2) == str.at(0)) // O
				db->O.push_back(str.at(1));
		}
	}

	db->close();
}

void DB::close()
{
	sqlite3_close(database);   
}

