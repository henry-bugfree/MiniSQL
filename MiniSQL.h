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
#include <iomanip>
using namespace std;

string data_path = "./data";

struct Table {
	string tb_name;
	string tb_path;
	string primary;
    int primary_pos;
    vector<string>colName;
	vector<string>type;
    map<string, vector<string> > data;
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
    void InsertTable(Table*);
    void SelectTable(Table*, vector<string>&, vector<string>& );
    void DeleteTable(Table*, vector<string>& );
    void PrintAssistant(int, int);
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
	outFile.open(tb->tb_path, ios::out);
	outFile << tb->tb_name << endl;
	outFile << tb->tb_path << endl;
	outFile << tb->primary << endl;

    for(int i=0;i<tb->colName.size();++i){
        if(tb->colName[i] == tb->primary){
            tb->primary_pos = i;
            break;
        }
    }
    outFile << tb->primary_pos << endl;

	for(auto& col : tb->colName){
		outFile << col << " ";
	}
	outFile << ";" << endl;
	for(auto& tp : tb->type){
		outFile << tp << " ";
	}
	outFile << ";" << endl;
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

// insert person values ( 1 , Alice ), ( 2 , Bob ), ( 3 , Curry );
void Database::InsertTable(Table* tb){
    string punc, tmp;
    vector<string> tmp_vector;
	while(1){
        cin >> punc;
        if(punc == "(" || punc == ","){
            cin >> tmp;
            tmp_vector.push_back(tmp);
        }
        else if(punc == "),"){
            if(tb->data.count(tmp_vector[tb->primary_pos]) == 1){
                cout << "duplicate element when inserting table!" << endl;
                break;	
            }
            tb->data[tmp_vector[tb->primary_pos]] = tmp_vector;

            ofstream outFile;
	        outFile.open(tb->tb_path, ios::app);
            for(auto& da : tmp_vector){
                outFile << da << " ";
            }
            outFile << ";" << endl;
            outFile.close();
            tmp_vector.clear();
        }
        else if(punc == ");"){
            if(tb->data.count(tmp_vector[tb->primary_pos]) == 1){
                cout << "duplicate element when inserting table!" << endl;
                break;
            }
            tb->data[tmp_vector[tb->primary_pos]] = tmp_vector;
            
            ofstream outFile;
	        outFile.open(tb->tb_path, ios::app);
            for(auto& da : tmp_vector){
                outFile << da << " ";
            }
            outFile << ";" << endl;
            outFile.close();
            tmp_vector.clear();
            cout << "inserted!" << endl;  
            break;
        }
        else{
            cout << "error in syntax when inserting table!" << endl;
            break;				
        }	
	}
    return ;
}

void Database::SelectTable(Table* tb, vector<string>& cols, vector<string>& cond){
    vector<int> pos;
    if(cols[0] == "*"){
        for(int i=0;i<tb->colName.size();++i){
            pos.push_back(i);
        }
    }
    else{
        for(auto& c:cols){
            pos.push_back(find(tb->colName.begin(),tb->colName.end(), c)-tb->colName.begin());
        }
    }

    //print header
    int setw_num = 15;
    PrintAssistant(tb->colName.size(), setw_num);

    for(auto& col: tb->colName){
        cout << "|" << setw(setw_num) << col ;
    }
    cout << "|" << endl;

    PrintAssistant(tb->colName.size(), setw_num);
    int row_count = 0;

    if(cond.empty()){
        for(auto& i:tb->data){
            for(auto& j:pos){
                cout << "|" << setw(setw_num) << i.second[j];
            }
            ++row_count;
            cout << "|" << endl;
        }
    }
    else {
        int cond_pos = 0;
        for(int i=0;i<tb->colName.size();++i){
            if(tb->colName[i] == cond[0]){
                cond_pos = i;
                break;
            }
        }
        multimap<string, vector<string>& > slt;
        for(auto& i:tb->data){
            slt.insert({i.second[cond_pos],i.second});
        }

        if(cond[1] == "="){
            for(auto iter=slt.equal_range(cond[2]).first;
                iter!=slt.equal_range(cond[2]).second;
                ++iter){
                for(auto& j:pos){
                    cout << "|" << setw(setw_num) << iter->second[j];
                }
                ++row_count;
                cout << "|" << endl;
            }
        }

        if(cond[1] == ">"){
            auto iter = slt.upper_bound(cond[2]);
            for(;iter!=slt.end();++iter){
                for(auto& j:pos){
                    cout << "|" << setw(setw_num) << iter->second[j];
                }
                ++row_count;
                cout << "|" << endl;
            }
        }

        if(cond[1] == "<"){
            for(auto iter=slt.begin();iter!=slt.lower_bound(cond[2]);++iter){
                for(auto& j:pos){
                    cout << "|" << setw(setw_num) << iter->second[j];
                }
                ++row_count;
                cout << "|" << endl;
            }
        }
    }

    PrintAssistant(tb->colName.size(), setw_num);
    cout << row_count << " rows in " << tb->tb_name << ";" << endl;
    return ;
}

void Database::DeleteTable(Table* tb, vector<string>& cond){
    multimap<string, vector<string>& > slt;
    int cond_pos = 0;
    for(int i=0;i<tb->colName.size();++i){
        if(tb->colName[i] == cond[0]){
            cond_pos = i;
            break;
        }
    }
    for(auto& i:tb->data){
        slt.insert({i.second[cond_pos],i.second});
    }

    if(cond[1] == "="){
        for(auto iter=slt.equal_range(cond[2]).first;
            iter!=slt.equal_range(cond[2]).second;
            ++iter){
            tb->data.erase(iter->second[tb->primary_pos]);
        }
    }

    if(cond[1] == ">"){
        auto iter = slt.upper_bound(cond[2]);
        for(;iter!=slt.end();++iter){
            tb->data.erase(iter->second[tb->primary_pos]);
        }
    }

    if(cond[1] == "<"){
        for(auto iter=slt.begin();iter!=slt.lower_bound(cond[2]);++iter){
            tb->data.erase(iter->second[tb->primary_pos]);
        }
    }

    ofstream outFile;
	outFile.open(tb->tb_path, ios::out);
	outFile << tb->tb_name << endl;
	outFile << tb->tb_path << endl;
	outFile << tb->primary << endl;
    outFile << tb->primary_pos << endl;

	for(auto& col : tb->colName){
		outFile << col << " ";
	}
	outFile << ";" << endl;
	for(auto& tp : tb->type){
		outFile << tp << " ";
	}
	outFile << ";" << endl;

    for(auto& da : tb->data){
        for(auto& da1 : da.second){
            outFile << da1 << " ";
        }
        outFile << ";" << endl;
    }
    outFile.close();

    cout << "deleted! " << endl;
    return ;
}

void Database::PrintAssistant(int num, int setw_num){
    for(int i=0;i<num;++i){
        cout << "|";
        for(int j=0;j<setw_num;++j){
            cout << "-"; 
        }
    }
    cout << "|" << endl;
    return ;
}