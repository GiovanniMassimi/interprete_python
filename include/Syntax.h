#if !defined(SYNTAX_H)
#define SYNTAX_H

#include <string>
#include <vector>
#include "Token.h"


// Forward declarations
class Visitor;

//Definition of the syntax grammar

///////////////////////////////
// Statement structures.    //
///////////////////////////////

struct Statement {
    virtual void accept(Visitor& visitor) const;
    virtual ~Statement() = default; 
};

struct Program {
    Program() = default;
    ~Program() = default;

    std::vector<Statement*> stmts;

    void accept(Visitor& visitor) const;
};

struct Expression : Statement {
    virtual void accept(Visitor& visitor) const = 0;
    virtual ~Expression() = default; 
};

struct SimpleStatement : Statement {
    virtual void accept(Visitor& visitor) const = 0;
    virtual ~SimpleStatement() = default;
};

struct Assignment : SimpleStatement {
    Token id;
    Expression* value;

    Assignment(Token id, Expression* value)
        : id(id), value(value) {}

    void accept(Visitor& visitor) const ;
};

struct ListAssignment : SimpleStatement {
    Token id;
    Expression* pos;
    Expression* value;

    ListAssignment(Token id, Expression* pos, Expression* value)
        : id(id), pos(pos), value(value) {}

    void accept(Visitor& visitor) const ;
};

struct ListCreation : SimpleStatement {
    Token id;

    ListCreation(Token id)
        : id(id) {}

    void accept(Visitor& visitor) const ;
};

struct Append : SimpleStatement {
    Token id;
    Expression* expr;

    Append(Token id, Expression* expr)
        : id(id), expr(expr) {}

    void accept(Visitor& visitor) const ;
};

struct Break : SimpleStatement {
    int line;

    Break(int line) : line(line) {}

    void accept(Visitor& visitor) const ;
};

struct Continue : SimpleStatement {
    int line;

    Continue(int line) : line(line) {}

    void accept(Visitor& visitor) const ;
};

struct Print : SimpleStatement {
    Expression* expr;

    Print(Expression* expr)
        : expr(expr) {}

    void accept(Visitor& visitor) const ;
};

struct CompoundStatement : Statement {
    virtual void accept(Visitor& visitor) const = 0;
    virtual ~CompoundStatement() = default;
};

struct Block {
    std::vector<Statement*> stmts;

    Block(std::vector<Statement*> stmts)
        : stmts(std::move(stmts)) {}

    void accept(Visitor& visitor) const;
};

struct WhileStatement : CompoundStatement {
    Expression* condition;
    Block* block;

    WhileStatement(Expression* condition, Block* block)
        : condition(condition), block(block) {}

    void accept(Visitor& visitor) const ;
};

struct ElifBlock {
    Expression* condition;
    Block* block;

    ElifBlock(Expression* condition, Block* block)
        : condition(condition), block(block) {}

    void accept(Visitor& visitor) const ;
};

struct ElseBlock {
    Block* block;

    ElseBlock(Block* block)
        : block(block) {}

    void accept(Visitor& visitor) const ;
};

struct IfStatement : CompoundStatement {
    Expression* condition;
    Block* block;
    std::vector<ElifBlock*> elif_blocks;
    ElseBlock* else_block;

    IfStatement(Expression* condition, Block* block,
        std::vector<ElifBlock*> elif_blocks = {}, ElseBlock* else_block = nullptr)
        : condition(condition), block(block),
          elif_blocks(std::move(elif_blocks)), else_block(else_block) {}

    void accept(Visitor& visitor) const ;
};

///////////////////////////////
// Expression structures.   //
///////////////////////////////


struct Join : Expression {
    Expression* left; 
    Token op; // or, and
    Expression* right;

    Join(Expression* left, Token op, Expression* right)
        : left(left), op(op), right(right) {}

    void accept(Visitor& visitor) const ;
};

struct Equality : Expression {
    Expression* left;
    Token op; // ==, !=
    Expression* right;

    Equality(Expression* left, Token op, Expression* right)
        : left(left), op(op), right(right) {}

    void accept(Visitor& visitor) const ;
};

struct Relation : Expression {
    Expression* left;
    Token op; // <, <=, >, >=, +, -
    Expression* right;

    Relation(Expression* left, Token op, Expression* right)
        : left(left), op(op), right(right) {}

    void accept(Visitor& visitor) const ;
};

struct NumExpr : Expression {
    Expression* left;
    Token op; // +, -
    Expression* right;

    NumExpr(Expression* left, Token op, Expression* right)
        : left(left), op(op), right(right) {}

    void accept(Visitor& visitor) const ;
};

struct Term : Expression {
    Expression* left;
    Token op; // *, '//'
    Expression* right;

    Term(Expression* left, Token op, Expression* right)
        : left(left), op(op), right(right) {}

    void accept(Visitor& visitor) const ;
};

struct Unary : Expression {
    Token op; // not, -
    Expression* right;

    Unary(Token op, Expression* right) : op(op), right(right) {}

    void accept(Visitor& visitor) const ;
};


struct Literal : Expression {
    Token value; 

    Literal(Token value) : value(value) {}
    void accept(Visitor& visitor) const ;
};

struct Factor : Expression {
    Literal token; // can be a number, True, False, or an expression
    Expression* expr; // if it's an expression in parentheses

    Factor(Literal token, Expression* expr) : token(token), expr(expr) {}

    void accept(Visitor& visitor) const ;
};

struct Location : Expression {
    Token id;
    Expression* index; // can be nullptr if not a list access

    Location(Token id, Expression* index = nullptr) : id(id), index(index) {}

    void accept(Visitor& visitor) const ;
};

#endif