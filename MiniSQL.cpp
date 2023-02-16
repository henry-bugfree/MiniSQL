#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <map>
#include <unistd.h>
#include <filesystem>

#include "MiniSQL.h"
using namespace std;

extern string data_path;

void LoadData(map<string, Database*>& dbs){
	string db_name;
	for (auto& i : filesystem::directory_iterator(data_path)) {
		db_name = "";
		string tmp_path = i.path().string();
		for(int j=tmp_path.length()-1;;j--){
			if(tmp_path[j]=='/'){
				break;
			}
			db_name.insert(0,1,tmp_path[j]);
		}
		
		if(db_name != ".DS_Store"){
			auto db_ptr = new Database();
			db_ptr->db_name = db_name;
			db_ptr->db_path =  data_path + "/" + db_name;
			dbs[db_name] = db_ptr;
			cout << "database " << db_name << " loaded!" << endl;

			for (auto& j : filesystem::directory_iterator(db_ptr->db_path)){
				string data_file = j.path().string();
				ifstream inFile;
				inFile.open(data_file, ios::in);

				auto tb = new Table();

				string tmp;
				inFile >> tmp;
				tb->tb_name = tmp;
				inFile >> tmp;
				tb->tb_path = tmp;
				inFile >> tmp;
				tb->primary = tmp;

				int tmp1;
				inFile >> tmp1;
				tb->primary_pos = tmp1;

				inFile >> tmp;
				while(tmp != ";"){
					tb->colName.push_back(tmp);
					inFile >> tmp;
				}
				inFile >> tmp;
				while(tmp != ";"){
					tb->type.push_back(tmp);
					inFile >> tmp;
				}
				vector<string> tmp_vector;
				while(inFile >> tmp){
					while(tmp != ";"){
						tmp_vector.push_back(tmp);
						inFile >> tmp;
					}
					tb->data[tmp_vector[tb->primary_pos]] = tmp_vector;
					tmp_vector.clear();
				}

				db_ptr->tables[tb->tb_name] = tb;
				cout << "table " << tb->tb_name << " loaded!" << endl;
			}
		}
    }
}

Database* current_db;
map<string, Database*> dbs; //root dbs

int main() {
	string cmd;
	string name;
	vector<string> cols;
	vector<string> cond;
	LoadData(dbs);

	while (cin >> cmd) {
		if (cmd == "exit"){
			for (auto iter = dbs.begin(); iter != dbs.end(); iter++) {
				delete iter->second;
			}
			break;
		} 
		else if (cmd == "use"){
			cin >> name;
			if(dbs.count(name) == 0){
				cout << "database does not exist!" << endl;
				continue;
			}
			else{
				current_db = dbs[name];
				cout << name << " selected!" << endl;
			}
		}
		else if (cmd == "create") {
			cin >> cmd >> name;
			if (cmd == "database"){
				auto db_ptr = new Database();
				db_ptr->CreateDataBase(name);
				dbs[name] = db_ptr;
			}
			else if (cmd == "table"){
				if(current_db == nullptr){
					cout << "no database selected!" << endl;
					continue;
				}
				if(current_db->tables.count(name) == 1){
					cout << "there already exists table " << name << endl;
					continue;
				}
				auto tb = new Table();
				current_db->CreateTable(name, tb);
				current_db->tables[name] = tb;
			}
			else cout << "errors in create command!" << endl;
		}
        else if (cmd == "drop") {
			cin >> cmd >> name;
			if (cmd == "database"){
				if(dbs.count(name) == 0){
					cout << "database does not exist!" << endl;
					continue;
				}
				else{
					auto drop_db = dbs[name];
					if(current_db == drop_db)
						current_db = nullptr;
					drop_db->DropDataBase(name);
					auto iter = dbs.find(name);
					dbs.erase(iter);
				}
			}
			else if (cmd == "table"){
				if(current_db == nullptr){
					cout << "no database selected!" << endl;
					continue;
				}
				if(current_db->tables.count(name) == 0){
					cout << "there is no table " << name << endl;
					continue;
				}
				current_db->DropTable(name);
				auto iter = current_db->tables.find(name);
				current_db->tables.erase(iter);
			}
			else cout << "errors in drop command!" << endl;
		}
		// insert <table> values ( <const-value>, <const-value> ... ), ... );
		else if (cmd == "insert") {
			if(current_db == nullptr){
				cout << "no database selected!" << endl;
				continue;
			}
			cin >> name >> cmd;
			if(current_db->tables.count(name) == 0){
				cout << "table does not exist!" << endl;
				continue;
			}
			if (cmd != "values"){
				cout << "errors in insert command!" << endl;
				continue;
			}

			auto tb = current_db->tables[name];
			current_db->InsertTable(tb);
		}
		// delete <table> [ where <cond> ]
		else if (cmd == "delete") {
			cols.clear();
			cond.clear();
			if(current_db == nullptr){
				cout << "no database selected!" << endl;
				continue;
			}
			cin >> name;
			if(current_db->tables.count(name) == 0){
				cout << "table does not exist!" << endl;
				continue;
			}

			cin >> cmd;
			if(cmd == "where"){
				for(int i=0;i<3;++i){
					cin >> cmd;
					cond.push_back(cmd);
				}
				cin >> cmd;
				if(cmd == ";"){
					auto tb = current_db->tables[name];
					current_db->DeleteTable(tb, cond);
				}
				else {
					cout << "errors in delete command!" << endl;
					continue;
				}
			}
			else {
				cout << "errors in delete command!" << endl;
				continue;
			}
		}
		// select <column> from <table> [ where <cond> ]
		else if (cmd == "select") {
			cols.clear();
			cond.clear();
			if(current_db == nullptr){
				cout << "no database selected!" << endl;
				continue;
			}
			cin >> cmd;
			while(cmd != "from"){
				cols.push_back(cmd);
				cin >> cmd;
			}
			cin >> name;
			if(current_db->tables.count(name) == 0){
				cout << "table does not exist!" << endl;
				continue;
			}

			cin >> cmd;
			if(cmd == "where"){
				for(int i=0;i<3;++i){
					cin >> cmd;
					cond.push_back(cmd);
				}
				cin >> cmd;
			}
			if(cmd == ";"){
				auto tb = current_db->tables[name];
				current_db->SelectTable(tb, cols, cond);
			}
			else {
				cout << "errors in select command!" << endl;
				continue;
			}
		}
		else {
			string tmp;
			getline(cin, tmp);
			cout << "errors in command!" << endl;
		}
	}
	return 0;
}

// Study/Modern\ C++\ Programming/MiniSQL/
// create database test
// drop database test
// use test
// create table person (id int primary, name string)
// insert person values ( 1 , Alice ), ( 2 , Bob ), ( 3 , Curry );
// select * from person ;
// select * from person where id > 1 ;
// select * from person where name = Bob ;
// select name from person where id < 3 ;
// delete person where id < 2 ;