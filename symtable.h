
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
		string name;
		bool type;
		bool arr;
		virtual ~TElement(){};
};

class TType: public TElement {
	public:
		string name;
		unsigned long size;
		bool numeric;
		bool basic;
		bool struc;
		TType(string name,unsigned long size,bool basic=false, bool numeric=false, bool struc=false):name(name),size(size),basic(basic),numeric(numeric),struc(struc){};
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
		TType& type;
		TVar(TType& type):type(type){}
};

class TFunc: public TElement{
	public:
        string name;
		TType* type;
		std::vector<TType*>* args;
		TFunc(string name, TType* type, std::vector<TType*>* args):name(name),type(type),args(args){}
};

class TArray: public TElement{
	public:
		TType& type;
		int length;
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

    template<> class hash<string>{
        public:
            size_t operator()(const string &s) const { 
                int i;
                int sum=0;
                for(i=0; i<s.size(); i++){
                    sum=sum+s[i]*(int)(pow(31,s.size()-i+1));
                }
                return sum;
            }
    };
};


class Symtable {
	hash_map<tuple,TElement*> table;
	hash_map<string,TFunc*> funtable;
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
			table[tuple(string("string"),scope)]=new TType("string",0,true);
            
		}
		
		
		int insert(string& name,TElement* elem){
			table[tuple(name,scope)]=elem;
		}

		bool insertfun(TFunc* fun){
			hash_map<string,TFunc*>::iterator it;
            string key = string(fun->name);
            int i;
            for(i=0; i<fun->args->size(); i++){
                key += fun->args->at(i)->name;
            }

            it=funtable.find(key);
			if(it==funtable.end()){
			    funtable[key]=fun;
			}else{
                //cerr << "Function `" <<key<< "` was declare before." << endl;
                return false;
            }
           
            #ifdef DEBUG
            cout << "inserting key " << key << endl; 
            #endif
            funtable[key]=fun;
            return true;

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
		
		TFunc* lookupFunc(const string name, const std::vector<TType*> args){
			hash_map<string,TFunc*>::iterator it;
            string key = string(name);
            int i;
            for(i=0; i<args.size(); i++){
                key += args[i]->name;
            }
            #ifdef DEBUG
            cout << "key buscando " << key << endl; 
            #endif
			it=funtable.find(key);
			if(it==funtable.end()){
			    return NULL;	
			}
			
			return it->second;
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
