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
		virtual TType* typeChk(Symtable& t){return t.lookupType("void");}
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
		TType* typeChk(Symtable& t ){
			return expr.typeChk(t);
		}
};

class NInteger : public NExpression {
	public :
		long long value;
		NInteger(long long value) : value(value) {}
		TType* typeChk(Symtable& t){
			return (TType*)t.lookupType("integer");
		}
};

class NDouble : public NExpression {
	public :
		double value;
		NDouble(double value) : value(value) {}
		TType* typeChk(Symtable& t){
			return (TType*)t.lookupType("double");
		}
};

class NString : public NExpression {
	public :
		std::string value;
		NString(const std::string &value) : value(value) {
			
		}
};

class NArray : public NExpression {
	public :
		ExpressionList values;
		NArray() { }
        TType* typeChk(Symtable& t){
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
                return //arreglo de tipo first;
            }
        }
};

class NChar : public NExpression {
	public :
		char value;
		NChar(char value) : value(value){}
		TType* typeChk(Symtable& t){
			return (TType*)t.lookupType("char");
		}
};

class NBool : public NExpression {
	public:
		bool value;
		NBool(bool value) :value(value){}
		TType* typeChk(Symtable& t){
			return (TType*)t.lookupType("boolean");
		}
};

class NIdentifier : public NLRExpression {
	public:
		std::string name;
		TType* type;
		NIdentifier(const std::string &name) : name(name){}
		NIdentifier(const std::string &name,TType* type) : name(name),type(type){}
		TType* typeChk(Symtable& t){
			return type;
		}
};

class NArrayAccess : public NLRExpression{
	public:
		NLRExpression &lexpr;
		NExpression &index;
		NArrayAccess(NLRExpression &lexpr, NExpression &index):lexpr(lexpr),index(index){}
		TType* typeChk(Symtable &t){
			if (index.typeChk(t)->name=="integer") return lexpr.typeChk(t);
			fprintf(stderr,"Array index must be an integer\n");
			return NULL;
		}
};

class NStructAccess : public NLRExpression{
	public:
		const NLRExpression &lexpr;
		NIdentifier &name;
		NStructAccess(const NLRExpression &lexpr,NIdentifier &name):lexpr(lexpr),name(name){}
};

class NFunctionCall : public NExpression {
	public:
		const NIdentifier &id;
		ExpressionList arguments;
		NFunctionCall(const NIdentifier &id, ExpressionList &arguments) : id(id), arguments(arguments){}
		TType* typeChk(Symtable &t){
			for(int i=0;i<arguments.size();i++){
				arguments[i]->typeChk(t);
			}
			return &t.lookupFunc(id.name)->type;
		}
};


class  NBinaryOperator : public NExpression {
	public :
		string op;
		NExpression &lexp;
		NExpression &rexp;
		NBinaryOperator(NExpression& lexp,string op,NExpression& rexp):op(op),lexp(lexp),rexp(rexp){}
		TType* typeChk(Symtable& t){
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
				if(t1->typeception==1 && t2->typeception==1){
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
		TType* typeChk(Symtable& t){
			TType* t2=rexp.typeChk(t);
			if(isalpha(op[0])){
				if(t2->name=="boolean"){
					return t2;
				}else{
					fprintf(stderr,"%s expected a boolean type but received a %s\n",op.c_str(),t2->name.c_str());
					return NULL;
				}
			}else{
				if(t2->typeception==1){
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
		TType* typeChk(Symtable &t){
			for(int i=0;i<statements.size();i++){
				statements[i]->typeChk(t);
			}
            return t.lookupType("void");
		}
};


class NVariableDeclaration : public NStatement {
	public:
		const NIdentifier& type;
		NIdentifier& id;
		NExpression *assigment;
		NVariableDeclaration(const NIdentifier& type,NIdentifier& id,NExpression* assignment=NULL):type(type),id(id),assigment(assignment){}
		TType* typeChk(Symtable &t){
			TType* t1 = t.lookupType(type.name);
			if(t1==NULL) {
				cerr<<"Type "<<type.name<<" not defined"<<endl;	
				return NULL;
			}
			if(assigment!=NULL){
				TType* t2 = assigment->typeChk(t);
				//cout<<"declsign"<<t1<<' '<<t2<<endl;
				if(t2==NULL) return NULL;
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
		NBlock &block;
		NFunctionDeclaration(const NIdentifier& type,const NIdentifier& id,const VariableList& args, NBlock block):type(type),id(id),args(args),block(block){}
		TType* typeChk(Symtable &t){
			for(int i=0;i<args.size();i++){
				args[i]->typeChk(t);
			}
			return block.typeChk(t);
		}
			
};

class NArrayDeclaration : public NStatement{
	public:
		const NIdentifier& id;
		const NIdentifier& type;
        const NExpression& size;
		ExpressionList elements;
		NArrayDeclaration(const NIdentifier& id, const NIdentifier& type,
				const NExpression& size, ExpressionList& elements) :
			id(id), type(type), size(size), elements(*(new ExpressionList())){}

		NArrayDeclaration(const NIdentifier& id, const NIdentifier& type,
				const NExpression& size, ExpressionList& elements) :
			id(id), type(type), size(size), elements(elements){}

        TType* typeChk(Symtable &t){
            TType* s = size->typeChk(&t);
            TType* t = type->typeChk(&t);
            bool err=false;

            if(t->name == NULL){
                fprintf(stderr, "Array elements are not the same type.");
                return NULL;
            }else if(s->name!="integer"){
                fprintf(stderr, "Array size expression is not an integer.");
                return NULL;
            }

            for(int i=0; i<elements.size(); i++){
                if(t->name != elements[i]->typeChk(t)->name)
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
};


class NWhileDo : public NStatement{
	public:
		NExpression* cond;
		NBlock& block;
		NWhileDo(NExpression* cond, NBlock& block):cond(cond),block(block){}
		TType* typeChk(Symtable &t){
			cond->typeChk(t);	
			return block.typeChk(t);
		}
};

class NDoWhile : public NStatement{
	public:
		NExpression* cond;
		NBlock& block;
		NDoWhile(NExpression* cond, NBlock& block):cond(cond),block(block){}
		TType* typeChk(Symtable &t){
			cond->typeChk(t);	
			return block.typeChk(t);
		}
};

class NIf : public NStatement{
	public:
		NExpression& cond;
		NStatement& block;
		NStatement* elseBlock;
		NIf(NExpression& cond,NStatement& block):cond(cond),block(block){}
		NIf(NExpression& cond,NStatement& block, NStatement* elseBlock):cond(cond),block(block),elseBlock(elseBlock){}
		TType* typeChk(Symtable &t){
			cond.typeChk(t);
			if(elseBlock!=NULL) elseBlock->typeChk(t);
			return block.typeChk(t);
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
		const NExpression* expr;
		NReturn(){}
		NReturn(NExpression *expr):expr(expr){}
		
};

class NAssignment : public NStatement{
	public:
		NLRExpression* var;
		NExpression* assig;
		NAssignment (NLRExpression * var, NExpression *assigment):var(var),assig(assigment){}
		TType* typeChk(Symtable &t){
			printf("asdf\n");
			TType* varT = var->typeChk(t);
			TType* assigT = assig->typeChk(t);
			if (varT == NULL || assigT == NULL){
				return NULL;
			} else if (varT->name!=assigT->name){
				fprintf(stderr,"The assignment  expects a %s but received a %s\n",varT->name.c_str(),assigT->name.c_str());
				return NULL;
			}
		}
};
