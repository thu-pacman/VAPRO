#include <iostream>  
#include <fstream>  
#include <stdio.h>
#include <vector>

#include "/home/jyh/1/VAPRO/include/json/json-forwards.h"
#include "/home/jyh/1/VAPRO/include/json/json.h"
using namespace std;
class c
{
    public:
    int time;
    int ins;
    c(int t,int i){time=t;ins=i;}
};


void openfile(const string& filename,const string& body)
{
    ofstream ofile(filename);
    ofile<<body;
    ofile.close();
}

int main() 
{  
    std::vector<c> vec;
    for(int i=0;i<10;i++)
    {
        vec.push_back(c(i,i*i));
    }

    ifstream ifs;
    ifs.open("testjson.json");
    if(!ifs.is_open())
    {
    cout<<"open file error!"<<endl;
    return 1;
    }

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(ifs, root, false))
    {
    return -1;
    }

    std::string name = root["name"].asString();
    int age = root["age"].asInt();

    std::cout<<name<<std::endl;
    std::cout<<age<<std::endl;

    return 0;
}