#include <hash_map>
#include <iostream>
#include <string>
#include <cmath>
#include "ast.h"

using namespace __gnu_cxx;
using namespace std;

typedef std::vector<NVariableDeclaration*> VariableList;

class tuple{
    public:
        const NIdentifier &id;
        int scope;
    tuple(const NIdentifier &id, int scope):id(id),scope(scope){}
    bool operator==(const tuple a) const{
        return id.name==a.id.name && scope==a.scope;
    }
};

class idinfo{
    public:
        string attribute;
        string type;
        int size;
        union{
            float valuef;
            int valuei;
            char valuec;
            string * values;
            bool valueb;
        };
        struct{
            VariableList * args;
        };

    idinfo(){};
    idinfo(string attribute, string type, int size):attribute(attribute),type(type),size(size){}
    idinfo(string attribute, string type, int size, float valuef):attribute(attribute),type(type),size(size),valuef(valuef){}
    idinfo(string attribute, string type, int size, int valuei):attribute(attribute),type(type),size(size),valuef(valuei){}
    idinfo(string attribute, string type, int size, char valuec):attribute(attribute),type(type),size(size),valuef(valuec){}
    idinfo(string attribute, string type, int size, string * values):attribute(attribute),type(type),size(size),values(values){}
    idinfo(string attribute, string type, int size, bool valueb):attribute(attribute),type(type),size(size),valueb(valueb){}
    idinfo(string attribute, string type, int size, VariableList *args):attribute(attribute),type(type),size(size),args(args){}
    
};

namespace __gnu_cxx{
    template<> class hash<tuple>{
        public:
            size_t operator()(const tuple &s) const { 
                int i;
                int sum=0;
                for(i=0; i<s.id.name.size(); i++){
                    sum=sum+s.id.name[i]*(int)(pow(31,s.id.name.size()-i+1));
                }
                cout << sum+s.scope << endl;
                return sum+s.scope;
            }
    };
};

/*int main(int argc, char *argv[]){
    
    hash_map<tuple,idinfo> symtable;
    NIdentifier r("hola");
    NIdentifier p("hola");
    NIdentifier q("holas");
    idinfo i(std::string("const"),std::string("integer"),54);
    cout << (r.name==p.name)<<(r.name==q.name)<<endl;
    tuple t(r,3);
    tuple h(q,7);
    symtable[t]=i;
    cout << symtable[t].attribute << symtable[h].attribute << t.id.name << t.scope << endl;
    return 0;
}*/
