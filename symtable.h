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

class TElement {
	public:
		string name;
		bool istype;
		bool isarr;
		TElement(string name,bool type=false,bool arr=false):name(name),istype(type),isarr(arr){};
		virtual ~TElement(){};
};

class TType: public TElement {
	public:
		unsigned long size;
		bool numeric;
		bool basic;
		bool struc;

		TType(string name,unsigned long size,bool basic=false, bool numeric=false, bool struc=false,bool arr=false):TElement(name,true,arr),size(size),basic(basic),numeric(numeric),struc(struc){};

        bool operator==(const TType &t2)const{
            return name == t2.name;
        }
        
        bool operator!=(const TType &t2)const{
            return !(*this == t2);
        }

};

class Field{
	public:
		TType& type;
		string& name;
		Field(TType& type,string& name):type(type),name(name){}
};


class TStructured: public TType{
	public:
		hash_map<string,TType*> fields;
		bool uni;
		TStructured(string name,int size,bool uni=false):TType(name,size,true,false,true){}
		TType* accessType(string name){
			hash_map<string,TType*>::iterator it;
			it=fields.find(name);
			if(it==fields.end())return NULL;
			return it->second;
		}
		virtual void addField(TType* type,string name){
			fields[name]=type;
			size+=type->size;
		}
};

class TRegister: public TStructured{
	public:
		TRegister(string name):TStructured(name,0){};
};
class TUnion: public TStructured{
	public:
		TUnion(string name,int size):TStructured(name,size,true){}
		void addField(TType* type,string name){
			fields[name]=type;
			if(size<type->size)size=type->size;
		}
};


class TVar: public TElement{
	public:
		TType& type;
		TVar(string name,TType& type):type(type),TElement(name){}
};

class TFunc: public TElement{
	public:
		TType* type;
		std::vector<TType*>* args;
		TFunc(string name, TType* type, std::vector<TType*>* args):TElement(name),type(type),args(args){}
};

class TArray: public TType{
	public:
		TType& type;
		int length;
		TArray(TType& type, int length):TType("array",type.size*length,false,false,false,true),type(type),length(length){}


        /*bool operator==(const TType &t2)const{
            if(t2.arr){
                return this->type == (const_cast<TArray*>(t2)).type && this->length == (const_cast<TArray>(t2)).length;
            }else
                return false;
        }*/

        bool operator==(const TArray &t2)const{
            return type == t2.type && length == t2.length;
        }

        
        bool operator!=(const TArray &t2)const{
            return !(*this == t2);
        }
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
		
		int insertType(string& name,TType* type){
			table[tuple(name,0)]=type;
		}

		int insert(string& name,TElement* elem){
#ifdef DEBUG
			cerr<<"inserted "<<name<<scope<<endl;
#endif
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

        int insertnextscope(string& name, TElement* elem){
            table[tuple(name,scope+1)]=elem;
        }

		TElement* lookup(const string name){
			tuple t(name,scope);
			hash_map<tuple,TElement*>::iterator it;
#ifdef DEBUG
			cerr<<"searching "<<name <<scope<<endl;
#endif
			it=table.find(t);
			if(it==table.end()){
				list<int>::iterator lit;
				for(lit = scopeStack.begin();lit!=scopeStack.end();lit++){
#ifdef DEBUG
					cerr<<"searching "<<name <<*lit<<endl;
#endif
					t=tuple(name,*lit);
					it=table.find(t);
					if(!(it==table.end())){
						break;
					}
				}
			}
			if(it==table.end()){
#ifdef DEBUG
						cerr<<"NOT FOUND!"<<endl;
#endif
				return NULL;
			}
			return it->second;
		}
		
        TElement* lookupScope(const string name){
			tuple t(name,scope);
			hash_map<tuple,TElement*>::iterator it;
#ifdef DEBUG
			cerr<<"searching "<<name <<scope<<endl;
#endif
			it=table.find(t);
			
            if(it==table.end()){
#ifdef DEBUG
						cerr<<"NOT FOUND!"<<endl;
#endif
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
		int resetScope(){scope=0;nextscope=1;scopeStack.clear();}
};
#define SYMTABLE
#endif
