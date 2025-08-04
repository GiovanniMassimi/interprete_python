#if !defined(SYNTAX_H)
#define SYNTAX_H



/*
<program>→ <stmts>endmarker
<stmts>→ <stmt><stmts>| ϵ
<stmt>→ <compound_stmt>| <simple_stmt>

<simple_stmt>→ <loc>= <expr>newline
            | <id>= list() newline
            | <id>. append ( <expr>) newline
            | break newline
            | continue newline
            | print ( <expr>) newline

<compound_stmt>→ <if_stmt>| <while_stmt>
    <if_stmt>→ if <expr>: <block>
        | if <expr>: <block><elif_block>
        | if <expr>: <block><else_block>
    <elif_block>→ elif <expr>: <block>
        | elif <expr>: <block><elif_block>
        | elif <expr>: <block><else_block>
    <else_block>→ else : <block>
    <while_stmt>→ while <expr>: <block>
<block>→ newline indent <stmts>dedent

<expr>→ <expr>or <join>| <join>
<join>→ <join>and <equality>| <equality>
<equality>→ <equality>== <rel>| <equality>!= <rel>| <rel>
<rel>→ <numexpr>< <numexpr>| <numexpr><= <numexpr>
        | <numexpr>>= <numexpr>| <numexpr>> <numexpr>| <numexpr>
<numexpr>→ <numexpr>+ <term>| <numexpr>- <term>| <term>
<term>→ <term>* <unary>| <term>// <unary>| <unary>
<unary>→ not <unary>|- <unary>| <factor>
<factor>→ ( <expr>) | <loc>| num | True | False
<loc>→ id | id [ <expr>]

*/

#include <string>
#include <vector>
#include "Token.h"

// Forward declarations
class Visitor;

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
    int line_number; // per tracciare la posizione nel codice

    Break(int line_number) : line_number(line_number) {}

    void accept(Visitor& visitor) const ;
};

struct Continue : SimpleStatement {
    int line_number; // per tracciare la posizione nel codice

    Continue(int line_number) : line_number(line_number) {}

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

struct Join : Expression {
    Expression* left;
    Token op; // 'or' or 'and'
    Expression* right;

    Join(Expression* left, Token op, Expression* right)
        : left(left), op(op), right(right) {}

    void accept(Visitor& visitor) const ;
};

struct Equality : Expression {
    Expression* left;
    Token op; // '==', '!='
    Expression* right;

    Equality(Expression* left, Token op, Expression* right)
        : left(left), op(op), right(right) {}

    void accept(Visitor& visitor) const ;
};

struct Relation : Expression {
    Expression* left;
    Token op; // '<', '<=', '>=', '>'
    Expression* right;

    Relation(Expression* left, Token op, Expression* right)
        : left(left), op(op), right(right) {}

    void accept(Visitor& visitor) const ;
};

struct NumExpr : Expression {
    Expression* left;
    Token op; // '+', '-'
    Expression* right;

    NumExpr(Expression* left, Token op, Expression* right)
        : left(left), op(op), right(right) {}

    void accept(Visitor& visitor) const ;
};

struct Term : Expression {
    Expression* left;
    Token op; // '*', '//'
    Expression* right;

    Term(Expression* left, Token op, Expression* right)
        : left(left), op(op), right(right) {}

    void accept(Visitor& visitor) const ;
};

struct Unary : Expression {
    Token op; // 'not', '-'
    Expression* expr;

    Unary(Token op, Expression* expr) : op(op), expr(expr) {}

    void accept(Visitor& visitor) const ;
};

//facor , literal, location

struct Literal : Expression {
    Token value; // se is_bool true rappresenta 1 True e 0 False se no e il numero
    bool is_bool; 

    Literal(Token value) : value(value), is_bool(false) {
        if (value.tag == Token::TRUE_ || value.tag == Token::FALSE_) {
            is_bool = true;
        }
    }

    void accept(Visitor& visitor) const ;
};

struct Factor : Expression {
    Literal token; // può essere un numero, True, False, o un'espressione
    Expression* expr; // se è un'espressione tra parentesi

    Factor(Literal token, Expression* expr) : token(token), expr(expr) {}

    void accept(Visitor& visitor) const ;
};

struct Location : Expression {
    Token id; // identificatore
    Expression* index; // può essere nullptr se non è un accesso a lista

    Location(Token id, Expression* index = nullptr) : id(id), index(index) {}

    void accept(Visitor& visitor) const ;
};

#endif