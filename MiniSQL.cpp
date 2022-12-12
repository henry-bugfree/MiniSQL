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

using namespace std;

string data_path = "./data";

struct Table {
	string tb_name;
	string tb_path;
	vector<string>colName;
	vector<string>type;
	string primary;
};

class Database {
public:
	string db_name;
	string db_path;
	map<string, Table*> tables;

	Database() {
		db_name = "";
		db_path = "";
	}
	~Database() {
		for (auto iter = tables.begin(); iter != tables.end(); iter++) {
			delete iter->second;
		}
	}
    void CreateDataBase(string);
    void DropDataBase(string);
    void CreateTable(string, Table*);
    void DropTable(string);
};

void Database::CreateDataBase(string name){
    string path = data_path + "/" + name;
    if(opendir(path.c_str())==nullptr){
        if (mkdir(path.c_str(),0755)==0){
			db_path = path;
			db_name = name;
			cout << "database created!" << endl;
		}
        else
            cout << "database fail to create!" << endl;
    }
    else cout << "database already existed!" << endl;
    return;
}

void Database::DropDataBase(string name){
    string path = data_path + "/" + name;
    string command = "rm -r " + path;
    if(opendir(path.c_str())!=nullptr){
        if (system(command.c_str())==0){
			cout << name << " dropped" << endl;
		}
        else
            cout << "fail to drop database" << name << endl;
    }
    else cout << "database does not exist!" << endl;
    return;
}

// create table person (id int primary, name string)
void Database::CreateTable(string name, Table* tb){
	string tmp;
	while(1){
		cin >> tmp;
		if(tmp[0]=='('){
			tmp.erase(tmp.begin());
			tb->colName.push_back(tmp);
		}
		else{
			tb->colName.push_back(tmp);
		}
		cin >> tmp;
		auto back = tmp.back();
		if(back==','){
			tmp.pop_back();
			tb->type.push_back(tmp);
			continue;
		}
		else if(back==')'){
			tmp.pop_back();
			tb->type.push_back(tmp);
			break;
		}
		else{
			tb->type.push_back(tmp);
			cin >> tmp;
			back = tmp.back();
			tmp.pop_back();
			if(back==',' && tmp == "primary"){
				tb->primary=tb->colName.back();
				continue;
			}
			else if(back==')' && tmp == "primary"){
				tb->primary=tb->colName.back();
				break;
			}
			else{
				cout << "error in syntax when creating table!" << endl;
				break;				
			}		
		}
	}
	tb->tb_name=name;
	tb->tb_path = data_path + "/" + db_name + "/" + name + ".dat";
	ofstream outFile;
	outFile.open(tb->tb_path);
	outFile << tb->tb_name << endl;
	outFile << tb->tb_path << endl;
	outFile << tb->primary << endl;
	for(auto& col : tb->colName){
		outFile << col << " ";
	}
	outFile << endl;
	for(auto& tp : tb->type){
		outFile << tp << " ";
	}
	outFile << endl;
	outFile.close();
	cout << "table created!" << endl;
	return;
}

void Database::DropTable(string name){
	string path = db_path + "/" + name + ".dat";
    string command = "rm " + path;
    if(access(path.c_str(), F_OK) == 0){
        if (system(command.c_str()) == 0){
			cout << name << " dropped" << endl;
		}
        else
            cout << "fail to drop table" << name << endl;
    }
    else cout << "table does not exist!" << endl;
	return;
}

Database* current_db;
map<string, Database*> dbs;

int main() {
	string cmd;
	string name;
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
				break;
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
					break;
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
				current_db->DropTable(name);
				auto iter = current_db->tables.find(name);
				current_db->tables.erase(iter);
			}
			else cout << "errors in drop command!" << endl;
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
// create table person (id int primary, name string)
