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

using namespace std;

struct Tables {
	string name;
	string path;
	vector<string>colName;//列名称
	vector<string>type;
	vector<int>size;
	FILE* fp;
};

class Database {
    bool is_open;
	vector<Tables*> tables;
public:
	string cmd;
	string path;

	Database() {
		cmd = "";
		path = "";
		is_open = false;
	}
	~Database() {
        cmd = "";
		path = "";
        is_open = false;
		for (auto& tab : tables) {
			if (tab->fp != NULL)
				fclose(tab->fp);
			delete tab;
		}
		tables.resize(0);
	}
    void CreateDataBase(string);
    void DropDataBase(string);
    void CreateTable(string);
    void DropTable(string);
};

void Database::CreateDataBase(string name){
    string path = "./" + name;
    if(opendir(path.c_str())==nullptr){
        if (mkdir(name.c_str(),0755)==0)
            cout << "database created!" << endl;
        else
            cout << "database fail to create!" << endl;
    }
    else cout << "database already existed!" << endl;
    return;
}

void Database::DropDataBase(string name){
    string path = "./" + name;
    string command = "rm -r " + path;
    if(opendir(path.c_str())!=nullptr){
        if (system(command.c_str())==0)
            cout << name << " dropped" << endl;
        else
            cout << "fail to drop database" << name << endl;
    }
    else cout << "database does not exist!" << endl;
    return;
}

void Database::CreateTable(string name){
    return;
}

void Database::DropTable(string name){
    return;
}

int main() {
	Database db;
	while (cin >> db.cmd) {
		if (db.cmd == "exit") 
            break;
		else if (db.cmd == "create") {
			string name;
			cin >> db.cmd >> name;
			if (db.cmd == "database")
				db.CreateDataBase(name);
			else if (db.cmd == "table")
				db.CreateTable(name);
			else cout << "errors in create command!" << endl;
		}
        else if (db.cmd == "drop") {
			string name;
			cin >> db.cmd >> name;
			if (db.cmd == "database")
				db.DropDataBase(name);
			else if (db.cmd == "table")
				db.DropTable(name);
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

// create database test
// drop database test