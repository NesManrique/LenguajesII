#include <vector>
#include <iostream>
#include "symtable.h"

class NStatement;
class NExpression;
class NVariableDeclaration;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;


class Node{
	public:
		virtual TType* typeChk(Symtable& t,TType* expected = NULL){
#ifdef DEBUG
			cerr<<"TypeCHK:default"<<endl;
#endif
			return t.lookupType("void");
		}
		virtual ~Node() {}
};

class NExpression : public Node {

};

class NLRExpression : public NExpression{
};

class NStatement : public Node {

};


class NExpressionStatement : public NStatement {
	public:
		NExpression &expr;
		NExpressionStatement(NExpression& expr):expr(expr){}
		TType* typeChk(Symtable& t,TType* expected = NULL){
#ifdef DEBUG
			cerr<<"TypeCHK:ExprStament"<<endl;
#endif
			return expr.typeChk(t);
		}
};

class NInteger : public NExpression {
	public :
		long long value;
		NInteger(long long value) : value(value) {}
		TType* typeChk(Symtable& t,TType* expected = NULL){
#ifdef DEBUG
			cerr<<"TypeCHK:integer "<<value<<endl;
#endif
			return (TType*)t.lookupType("integer");
		}
};

class NDouble : public NExpression {
	public :
		double value;
		NDouble(double value) : value(value) {}
		TType* typeChk(Symtable& t,TType* expected = NULL){
#ifdef DEBUG
			cerr<<"TypeCHK:double "<<value<<endl;
#endif
			return (TType*)t.lookupType("float");
		}
};

class NString : public NExpression {
	public :
		std::string value;
		NString(std::string &value) : value(value) {}
};

class NArray : public NExpression {
	public :
		ExpressionList values;
		NArray() { }
		TType* typeChk(Symtable& t,TType* expected = NULL){
            bool err=false;
            TType* first=values[0]->typeChk(t);
            for(int i=1; i<values.size(); i++){
                if(first->name != values[i]->typeChk(t)->name)
                    err=true;
            }
            if(err){
                fprintf(stderr, "Array elements are not the same type.");
                return NULL;
            }else{
                return NULL;//arreglo de tipo first;
            }
        }
};

class NChar : public NExpression {
	public :
		char value;
		NChar(char value) : value(value){}
		TType* typeChk(Symtable& t,TType* expected = NULL){
#ifdef DEBUG
			cerr<<"TypeCHK:Char "<<value<<endl;
#endif
			return (TType*)t.lookupType("char");
		}
};

class NBool : public NExpression {
	public:
		bool value;
		NBool(bool value) :value(value){}
		TType* typeChk(Symtable& t,TType* expected = NULL){
#ifdef DEBUG
			cerr<<"TypeCHK:Bool "<<value<<endl;
#endif
			return (TType*)t.lookupType("boolean");
		}
};

class NIdentifier : public NLRExpression {
	public:
		std::string name;
		TElement* symbol;
		NIdentifier(std::string &name) : name(name){}
		NIdentifier(std::string &name,TType* type) : name(name),symbol(type){}
		TType* typeChk(Symtable& t,TType* expected = NULL){
			TVar* temp= (TVar*) t.lookup(name);
			return &temp->type;
		}
};

class NArrayAccess : public NLRExpression{
	public:
		NLRExpression &lexpr;
		NExpression &index;
		NArrayAccess(NLRExpression &lexpr, NExpression &index):lexpr(lexpr),index(index){}
		TType* typeChk(Symtable& t,TType* expected = NULL){
			if (index.typeChk(t)->name=="integer") return lexpr.typeChk(t);
			fprintf(stderr,"Array index must be an integer\n");
			return NULL;
		}
};

class NStructAccess : public NLRExpression{
	public:
		NLRExpression &lexpr;
		NIdentifier &name;
		NStructAccess(NLRExpression &lexpr,NIdentifier &name):lexpr(lexpr),name(name){}
		TType* typeChk(Symtable& t,TType* expected = NULL){
			TType* temp = lexpr.typeChk(t)
			if (temp == NULL) return NULL;
			if (!temp->struc) {
				cerr << lexpr.name<< " is not a struct or union"<<endl;
				return NULL
			}
		}
};

class NFunctionCall : public NExpression {
	public:
		const NIdentifier &id;
		ExpressionList arguments;
		NFunctionCall(const NIdentifier &id, ExpressionList &arguments) : id(id), arguments(arguments){}
		TType* typeChk(Symtable& t,TType* expected = NULL){
            TType* tip;
		    std::vector<TType*> args;
			for(int i=0;i<arguments.size();i++){
                tip = arguments[i]->typeChk(t);
				if(tip == NULL){
                    cerr << "Type of argument " << i << " doesn't exist" << endl;
                }else{
                    args.push_back(tip); 
                }
			}

            if(t.lookupFunc(id.name,args) == NULL){
                cerr << "Function "<<id.name<<" was not declared"<< endl;
			    return NULL;
            }else{
                #ifdef DEBUG
                cerr << "Function call type "<<(t.lookupFunc(id.name,args)->type)->name<< endl;
                #endif
                return t.lookupFunc(id.name,args)->type;
            }
		}
};


class  NBinaryOperator : public NExpression {
	public :
		string op;
		NExpression &lexp;
		NExpression &rexp;
		NBinaryOperator(NExpression& lexp,string op,NExpression& rexp):op(op),lexp(lexp),rexp(rexp){}
		TType* typeChk(Symtable& t,TType* expected = NULL){
			TType* t1=lexp.typeChk(t);
			TType* t2=rexp.typeChk(t);
			if(isalpha(op[0])){
				if(t1->name=="boolean" && t2->name=="boolean"){
					return t1;
				}else{
					fprintf(stderr,"%s expected a boolean types but received %s and %s\n",op.c_str(),t1->name.c_str(),t2->name.c_str());
					/*should be NULL*/
					return t1;
				}
			}else{
				if(t1->numeric && t2->numeric){
					if(t1->name=="float") return t1;
					if(t2->name=="float") return t2;
					return t1;
				}else{
					fprintf(stderr,"%s expected a numeric types but received %s and %s\n",op.c_str(),t1->name.c_str(),t2->name.c_str());

					/*should be NULL*/
					if(t1->name=="float") return t1;
					if(t2->name=="float") return t2;
					return t1;
				}
			}
		}
};

class NUnaryOperator : public NExpression {
	public :
		string op;
		NExpression &rexp;
		NUnaryOperator(string op,NExpression& rexp):op(op),rexp(rexp){}
		TType* typeChk(Symtable& t,TType* expected = NULL){
			TType* t2=rexp.typeChk(t);
			if(isalpha(op[0])){
				if(t2->name=="boolean"){
					return t2;
				}else{
					fprintf(stderr,"%s expected a boolean type but received a %s\n",op.c_str(),t2->name.c_str());
					return NULL;
				}
			}else{
				if(t2->numeric){
					return t2;
				}else{
					fprintf(stderr,"%s expected a numeric and received a %s\n",op.c_str(),t2->name.c_str());
					return NULL;
				}
			}
		}
};

class NBlock: public NStatement{
	public :
		StatementList statements;
		NBlock() {};
		TType* typeChk(Symtable& t,TType* expected = NULL){
			bool err=false;
			TType* s;
			for(int i=0;i<statements.size();i++){
				s=statements[i]->typeChk(t,expected);
				if(s==NULL && !err){
					err=true;
				}
			}
			if(err) {cerr << "error in block"<<endl;return NULL;}
            return t.lookupType("void");
		}
};


class NVariableDeclaration : public NStatement {
	public:
		const NIdentifier& type;
		NIdentifier& id;
		NExpression *assigment;
		NVariableDeclaration(const NIdentifier& type,NIdentifier& id,NExpression* assignment=NULL):type(type),id(id),assigment(assignment){}
		TType* typeChk(Symtable& t,TType* expected = NULL){
			TType* t1 = t.lookupType(type.name);
			if(t1==NULL) {
				cerr<<"Type "<<type.name<<" not defined"<<endl;	
				return NULL;
			}
			if(assigment!=NULL){
				TType* t2 = assigment->typeChk(t);
                #ifdef DEBUG
				cout<<"declsign"<<t1<<' '<<t2<<endl;
                #endif
				if(t2==NULL) {cerr <<"Type Error assignment\n" ;return NULL;}
				if (t1->name!=t2->name){
					fprintf(stderr,"%s declared as %s but inicialized with %s\n",id.name.c_str(),t1->name.c_str(),t2->name.c_str());
					return NULL;
				}
				
			}
			return t1;
		}
};


class NFunctionDeclaration : public NStatement {
	public:
		const NIdentifier& type;
		const NIdentifier& id;
		VariableList args;
		NBlock *block;
		NFunctionDeclaration(const NIdentifier& type,const NIdentifier& id,const VariableList& args, NBlock* block=NULL):type(type),id(id),args(args),block(block){}
		TType* typeChk(Symtable& t,TType* expected = NULL){
			bool err=false;
			TType* x=t.lookupType(type.name);
		    cout<<"Type Function test "<<type.name<<endl;
			if(x==NULL){
				cerr<<"Type "<<type.name<<" not defined"<<endl;
			}
			TType* c;
			for(int i=0;i<args.size();i++){
				c=args[i]->typeChk(t);
				if(c==NULL) err=true;
			}
			block->typeChk(t,x);
			//cerr<<x<<err<<endl;
			if(err) return NULL;
			return t.lookupType("void");
		}

        int addSymtable(Symtable& t){
            bool err=false;
            TType* ret = t.lookupType(type.name);
            if(ret==NULL){
                cerr << "Error in function declaration " << id.name << " type " << ret->name << " does not exist" << endl;
                err=true;
            }
            std::vector<TType*>* arguments = new vector<TType*>();
            int i;
            TType* c;
            for(i=0; i<args.size(); i++){
                c = t.lookupType(args[i]->type.name);
                if(c==NULL){
                    err=true;
                    cerr << "Error in function declaration " << id.name << " type " << args[i]->type.name << " does not exist" << endl;
                }
                arguments->push_back(c);
            }
            if(err){
             return 1;
            }
            TFunc* f = new TFunc(id.name,ret,arguments);
            if(!t.insertfun(f))
                return 2;
            else
                return 0;
        }
			
};


class NArrayDeclaration : public NStatement{
	public:
		NIdentifier& id;
		NIdentifier& type;
        NExpression& size;
		ExpressionList elements;
		NArrayDeclaration(NIdentifier& id, NIdentifier& type,
				NExpression& size) :
			id(id), type(type), size(size), elements(*(new ExpressionList())){}

		NArrayDeclaration(NIdentifier& id, NIdentifier& type,
				NExpression& size, ExpressionList& elements) :
			id(id), type(type), size(size), elements(elements){}

		TType* typeChk(Symtable& t,TType* expected = NULL){
            TType* s = size.typeChk(t);
            TType* x = type.typeChk(t);
            bool err=false;

            if(x == NULL){
                fprintf(stderr, "Array elements are not the same type.");
                return NULL;
            }else if(s->name!="integer"){
                fprintf(stderr, "Array size expression is not an integer.");
                return NULL;
            }

            for(int i=0; i<elements.size(); i++){
                if(x->name != elements[i]->typeChk(t)->name)
                    err=true;
            }

            if(err){
                fprintf(stderr, "Array elements are not the same type as array type declaration.");
                return NULL;
            }
            
        }
};

class NRegisterDeclaration : public NStatement{
	public:
		const NIdentifier& type;
		VariableList fields;
		NRegisterDeclaration(const NIdentifier& type, VariableList fields) :type(type), fields(fields){}
};

class NUnionDeclaration : public NStatement{
	public:
		const NIdentifier& type;
		VariableList fields;
		NUnionDeclaration(const NIdentifier& type, VariableList fields) : type(type), fields(fields){}
		TType* typeChk(Symtable& t,TType* expected = NULL){
		}
};


class NWhileDo : public NStatement{
	public:
		NExpression* cond;
		NBlock& block;
		NWhileDo(NExpression* cond, NBlock& block):cond(cond),block(block){}
		TType* typeChk(Symtable& t,TType* expected = NULL){
			TType *a,*b;
			a=cond->typeChk(t);	
			b=block.typeChk(t,expected);
			if(a==NULL || b==NULL) return NULL;
			return t.lookupType("void");
		}
};

class NDoWhile : public NStatement{
	public:
		NExpression* cond;
		NBlock& block;
		NDoWhile(NExpression* cond, NBlock& block):cond(cond),block(block){}
		TType* typeChk(Symtable& t,TType* expected = NULL){
			TType *a,*b;
			a=cond->typeChk(t);	
			b=block.typeChk(t,expected);
			if(a==NULL || b==NULL) return NULL;
			return t.lookupType("void");
		}
};

class NIf : public NStatement{
	public:
		NExpression& cond;
		NStatement& block;
		NStatement* elseBlock;
		NIf(NExpression& cond,NStatement& block):cond(cond),block(block){}
		NIf(NExpression& cond,NStatement& block, NStatement* elseBlock):cond(cond),block(block),elseBlock(elseBlock){}
		TType* typeChk(Symtable& t,TType* expected = NULL){
			TType *a,*b,*c;
			a=cond.typeChk(t);	
			b=block.typeChk(t,expected);
			if(elseBlock!=NULL) c=elseBlock->typeChk(t,expected);
			if(a==NULL || b==NULL ) return NULL;
			cond.typeChk(t);
			if( a=NULL) return NULL;
			return t.lookupType("void");
		}
};


class NFor : public NStatement{
	public:
		NIdentifier& id;
		NExpression* beg;
		NExpression* end;
		NIdentifier* array;
		NBlock& block;
		NFor(NIdentifier& id,NExpression* beg,NExpression* end,NBlock& block): id(id),beg(beg),end(end),block(block){};
		NFor(NIdentifier& id,NIdentifier* array,NBlock &block): id(id),array(array),block(block){};

};

class NStop : public NStatement{
};

class NNext : public NStatement{

};

class NReturn : public NStatement{
	public:
		NExpression* expr;
		NReturn(){}
		NReturn(NExpression *expr):expr(expr){}
		TType* typeChk(Symtable& t,TType* expected = NULL){
			TType* s=expr->typeChk(t);
			cerr<<s->name<<expected->name<<endl;
			if(s->name==expected->name){return s;}
			else return NULL;
		}
		
};

class NAssignment : public NStatement{
	public:
		NLRExpression* var;
		NExpression* assig;
		NAssignment (NLRExpression * var, NExpression *assigment):var(var),assig(assigment){}
		TType* typeChk(Symtable& t,TType* expected = NULL){
			TType* varT = var->typeChk(t);
			TType* assigT = assig->typeChk(t);
			if (varT == NULL || assigT == NULL){
				return NULL;
			} else if (varT->name!=assigT->name){
				fprintf(stderr,"The assignment  expects a %s but received a %s\n",varT->name.c_str(),assigT->name.c_str());
				return NULL;
			}
			return varT;
		}
};
