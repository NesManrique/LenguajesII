#include <vector>
#include <iostream>

class NStatement;
class NExpression;
class NVariableDeclaration;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;

class Node{
	public:
		virtual ~Node() {}
};

class NExpression : public Node {

};

class NStatement : public Node {

};


class NExpressionStatement : public NStatement {
	public:
		NExpression &expr;
		NExpressionStatement(NExpression& expr):expr(expr){}
};

class NInteger : public NExpression {
	public :
		long long value;
		NInteger(long long value) : value(value) {}
};

class NDouble : public NExpression {
	public :
		double value;
		NDouble(double value) : value(value) {}
};

class NString : public NExpression {
	public :
		std::string value;
		NString(const std::string &value) : value(value) {}
};

class NArray : public NExpression {
	public :
		ExpressionList values;
		NArray() { }
};

class NChar : public NExpression {
	public :
		char value;
		NChar(char value) : value(value){}
};

class NBool : public NExpression {
	public:
		bool value;
		NBool(bool value) :value(value){}
};

class NIdentifier : public NExpression {
	public:
		std::string name;
		NIdentifier(const std::string &name) : name(name){}
};

class NFunctionCall : public NExpression {
	public:
		const NIdentifier &id;
		ExpressionList arguments;
		NFunctionCall(const NIdentifier &id, ExpressionList &arguments) : id(id), arguments(arguments){}
};

class NArrayAccess : public NExpression{
	public:
		const NIdentifier &id;
		NExpression &index;
		NArrayAccess(const NIdentifier &id, NExpression &index):id(id),index(index){}
};

/* ACCESS TO REGISTER FIELDS 
class NREGACCESS
*/

class  NBinaryOperator : public NExpression {
	public :
		int op;
		NExpression &lexp;
		NExpression &rexp;
		NBinaryOperator(NExpression& lexp,int op,NExpression& rexp):op(op),lexp(lexp),rexp(rexp){}
};

class NUnaryOperator : public NExpression {
	public :
		int op;
		NExpression &rexp;
		NUnaryOperator(int op,NExpression& rexp):op(op),rexp(rexp){}
};

class NBlock: public NStatement{
	public :
		StatementList statements;
		NBlock() {}
};


class NVariableDeclaration : public NStatement {
	public:
		const NIdentifier& type;
		NIdentifier& id;
		NExpression *assigment;
		NVariableDeclaration(const NIdentifier& type, NIdentifier& id):type(type),id(id){}
		NVariableDeclaration(const NIdentifier& type,NIdentifier& id,NExpression* assignment):type(type),id(id),assigment(assigment){}

};

class NFunctionDeclaration : public NStatement {
	public:
		const NIdentifier& type;
		const NIdentifier& id;
		VariableList args;
		NBlock &block;
		NFunctionDeclaration(const NIdentifier& type,const NIdentifier& id,const VariableList& args, NBlock block):type(type),id(id),args(args),block(block){}
};

class NArrayDeclaration : public NStatement{
	public:
		const NIdentifier& id;
		const NIdentifier& type;
		ExpressionList elements;
		NArrayDeclaration(const NIdentifier& id, const NIdentifier& type, 
				ExpressionList& elements) :
			id(id), type(type), elements(elements){}
};

class NRegisterDeclaration : public NStatement{
	public:
		const NIdentifier& type;
		VariableList fields;
		NRegisterDeclaration(const NIdentifier& type, VariableList fields) : 
			type(type), fields(fields){}
};

class NUnionDeclaration : public NStatement{
	public:
		const NIdentifier& type;
		VariableList fields;
		NUnionDeclaration(const NIdentifier& type, VariableList fields) : 
			type(type), fields(fields){}
};


class NWhileDo : public NStatement{
	public:
		NExpression* cond;
		NBlock& block;
		NWhileDo(NExpression* cond, NBlock& block):cond(cond),block(block){}
};

class NDoWhile : public NStatement{
	public:
		NExpression* cond;
		NBlock& block;
		NDoWhile(NExpression* cond, NBlock& block):cond(cond),block(block){}
};

class NIf : public NStatement{
	public:
		NExpression* cond;
		NStatement* block;
		NStatement* elseBlock;
		NIf(NExpression* cond,NStatement* block):cond(cond),block(block){}
		NIf(NExpression* cond,NStatement* block, NStatement* elseBlock):cond(cond),block(block),elseBlock(elseBlock){}
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





