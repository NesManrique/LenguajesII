
#ifndef SYMTABLE
#include <cstdio>
#include <hash_map>
#include <iostream>
#include <string>
#include <cmath>
#include <list>


using namespace __gnu_cxx;
using namespace std;

class tuple{
    public:
         string id;
         int scope;
    tuple(string id,int scope):id(id),scope(scope){}
    bool operator==(const tuple a) const{
        return id==a.id && scope==a.scope;
    }
	/*tuple operator=(const tuple& a){
		return tuple(a.id,a.scope);
	}*/
	
};

class TElement {
	public:
		virtual ~TElement(){};
};

class TType: public TElement {
	public:
		const string name;
		const unsigned long size;
		const bool basic;
		const bool arr;
		const bool struc;
		const bool numeric;
		TType(string name,unsigned long size,bool basic=false, bool numeric=false, bool struc=false, bool arr=false):name(name),size(size),basic(basic),numeric(numeric),struc(struc),arr(arr){};
};

class Field{
	public:
		TType* type;
		string& name;
		Field(TType* type,string& name):type(type),name(name){}
};

class TRegister: public TType{
	public:
		std::vector<Field*> fields;
		TRegister(string name,unsigned long size,std::vector<Field*> fields):TType(name,size),fields(fields){}

};

class TUnion: public TType{
	public:
		std::vector<Field*> fields;
		TUnion(string name,unsigned long size,std::vector<Field*> fields):TType(name,size),fields(fields){}

};


class TVar: public TElement{
	public:
		const TType& type;
		TVar(const TType& type):type(type){}
};

class TFunc: public TElement{
	public:
		TType& type;
		const std::vector<TType*> args;
		TFunc(TType& type, std::vector<TType*> args):type(type),args(args){}
};

class TArray: public TElement{
	public:
		const TType& type;
		const int length;
		TArray(TType& type, int length):type(type),length(length){}
};

namespace __gnu_cxx{
    template<> class hash<tuple>{
        public:
            size_t operator()(const tuple &s) const { 
                int i;
                int sum=0;
                for(i=0; i<s.id.size(); i++){
                    sum=sum+s.id[i]*(int)(pow(31,s.id.size()-i+1));
                }
               // cout << sum+s.scope << endl;
                return sum+s.scope;
            }
    };
};


class Symtable {
	hash_map<tuple,TElement*> table;
	list<int> scopeStack;
	int scope;
	int nextscope;
	public:
		Symtable():scope(0),nextscope(1){
			table[tuple(string("char"),scope)]=new TType("char",sizeof(char),true,true);
			table[tuple(string("integer"),scope)]=new TType("integer",sizeof(int),true,true);
			table[tuple(string("float"),scope)]=new TType("float",sizeof(float),true,true);
			table[tuple(string("boolean"),scope)]=new TType("boolean",sizeof(bool),true);
			table[tuple(string("void"),scope)]=new TType("void",0,true);
		//	table[tuple(string("ezequiel"),scope)]=new TType("boolean",sizeof(bool),true);
		}
		
		
		int insert(string& name,TElement* elem){
			table[tuple(name,scope)]=elem;
		}

        int insrtnextscope(string& name, TElement* elem){
            table[tuple(name,scope+1)]=elem;
        }

		TElement* lookup(const string name){
			tuple t(name,scope);
			hash_map<tuple,TElement*>::iterator it;
			it=table.find(t);
			if(it==table.end()){
				list<int>::iterator lit;
				for(lit = scopeStack.begin();lit!=scopeStack.end();lit++){
					t=tuple(name,*lit);
					it=table.find(t);
					if(!(it==table.end())){
						break;
					}
				}
			}
			if(it==table.end()){
				return NULL;
			}
			return it->second;
		}
		
		TFunc* lookupFunc(const string name){
			tuple t(name,scope);
			hash_map<tuple,TElement*>::iterator it;
			it=table.find(t);
			if(it==table.end()){
				list<int>::iterator lit;
				for(lit = scopeStack.begin();lit!=scopeStack.end();lit++){
					t=tuple(name,*lit);
					it=table.find(t);
					if(!(it==table.end())){
						break;
					}
				}
			}
			if(it==table.end()){
				return NULL;
			}
			return (TFunc*)it->second;
		}
        TType* lookupType(const string name){
            tuple t(name,0);
            hash_map<tuple,TElement*>::iterator it;
            it=table.find(t);
            if(it==table.end()){
                return NULL;
            }

            return (TType *)(it->second);
        }

        int begScope(){scopeStack.push_front(scope);scope=nextscope;nextscope++;}
        int endScope(){scope=scopeStack.front();scopeStack.pop_front();}
};
#define SYMTABLE
#endif
