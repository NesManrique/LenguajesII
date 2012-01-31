#include <iostream>
#include <vector>

class NStatement;
class NExpression;
class NVariableDeclaration;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;

class Node {
	public:
		virtual ~Node() {}
};

class NExpression : public Node {
};

class NStatement : public Node {
};

class NInteger : public NExpression {
	public:
		long long value;
		NInteger(long long value) : value(value) { }
};

class NDouble : public NExpression {
	public:
		double value;
		NDouble(double value) : value(value) { }
};

class NIdentifier : public NExpression {
	public:
		std::string name;
		NIdentifier(const std::string& name) : name(name) { }
};

class NFunctionCall : public NExpression {
	public:
		const NIdentifier& id;
		ExpressionList arguments;
		NFunctionCall(const NIdentifier& id, ExpressionList& arguments) :
			id(id), arguments(arguments) { }
		NFunctionCall(const NIdentifier& id) : id(id) { }
};

class NBinaryOperator : public NExpression {
	public:
		int op;
		NExpression& loperand;
		NExpression& roperand;
		NBinaryOperator(NExpression& loperand, int op, NExpression& roperand) :
			loperand(loperand), roperand(roperand), op(op) { }
};

class NAssignment : public NExpression {
	public:
		NIdentifier& lvalue;
		NExpression& rvalue;
		NAssignment(NIdentifier& lvalue, NExpression& rvalue) :
			lvalue(lvalue), rvalue(rvalue) { }
};

class NBlock : public NExpression {
	public:
		StatementList statements;
		NBlock() { }
};

class NExpressionStatement : public NStatement {
	public:
		NExpression& expression;
		NExpressionStatement(NExpression& expression) :
			expression(expression) { }
};

class NVariableDeclaration : public NStatement {
	public:
		const NIdentifier& type;
		NIdentifier& id;
		NExpression *assignmentExpr;
		NVariableDeclaration(const NIdentifier& type, NIdentifier& id) :
			type(type), id(id) { }
		NVariableDeclaration(const NIdentifier& type, NIdentifier& id, NExpression *assignmentExpr) :
			type(type), id(id), assignmentExpr(assignmentExpr) { }
};

class NFunctionDeclaration : public NStatement {
	public:
		const NIdentifier& type;
		const NIdentifier& id;
		VariableList arguments;
		NBlock& block;
		NFunctionDeclaration(const NIdentifier& type, const NIdentifier& id,
				const VariableList& arguments, NBlock& block) :
			type(type), id(id), arguments(arguments), block(block) { }
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
