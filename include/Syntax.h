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

// Forward declarations
class Visitor;

struct Statment {
    virtual void accept(Visitor& visitor) const;
    virtual ~Statment() = default; 
};

struct Program {
    Program() = default;
    ~Program() = default;

    std::vector<Statment*> stmts;

    void accept(Visitor& visitor) const;
};

struct Expression : Statment {
    virtual void accept(Visitor& visitor) const = 0;
    virtual ~Expression() = default; 
};

struct SimpleStatment : Statment {
    virtual void accept(Visitor& visitor) const = 0;
    virtual ~SimpleStatment() = default;
};

struct Assignment : SimpleStatment {
    Token id;
    Token eq;
    Expression* value;

    Assignment(Token id, Token eq, Expression* value)
        : id(id), eq(eq), value(value) {}

    void accept(Visitor& visitor) const override;
};

struct ListAssignment : SimpleStatment {
    Token id;
    Token LB;
    Expression* pos;
    Token RB;
    Token eq;
    Expression* value;

    ListAssignment(Token id, Token LB, Expression* pos, Token RB, Token eq, Expression* value)
        : id(id), LB(LB), pos(pos), RB(RB), eq(eq), value(value) {}

    void accept(Visitor& visitor) const override;
};

struct ListCreation : SimpleStatment {
    Token id;
    Token eq;
    Token list_token;
    Token LP;
    Token RP;

    ListCreation(Token id, Token eq, Token list_token, Token LP, Token RP)
        : id(id), eq(eq), list_token(list_token), LP(LP), RP(RP) {}

    void accept(Visitor& visitor) const override;
};

struct Append : SimpleStatment {
    Token id;
    Token dot;
    Token append_token;
    Token LP;
    Expression* value;
    Token RP;

    Append(Token id, Token dot, Token append_token, Token LP, Expression* value, Token RP)
        : id(id), dot(dot), append_token(append_token), LP(LP), value(value), RP(RP) {}

    void accept(Visitor& visitor) const override;
};

struct Break : SimpleStatment {
    Token break_token;

    Break(Token break_token) : break_token(break_token) {}

    void accept(Visitor& visitor) const override;
};

struct Continue : SimpleStatment {
    Token continue_token;

    Continue(Token continue_token) : continue_token(continue_token) {}

    void accept(Visitor& visitor) const override;
};

struct Print : SimpleStatment {
    Token print_token;
    Token LP;
    Expression* value;
    Token RP;

    Print(Token print_token, Token LP, Expression* value, Token RP)
        : print_token(print_token), LP(LP), value(value), RP(RP) {}

    void accept(Visitor& visitor) const override;
};

struct CompoundStatment : Statment {
    virtual void accept(Visitor& visitor) const = 0;
    virtual ~CompoundStatment() = default;
};

struct Block {
    Token newline;
    Token indent;
    std::vector<Statment*> stmts;
    Token dedent;

    Block(Token newline, Token indent, std::vector<Statment*> stmts, Token dedent)
        : newline(newline), indent(indent), stmts(std::move(stmts)), dedent(dedent) {}
};

struct WhileStatement : CompoundStatment {
    Token while_token;
    Expression* condition;
    Token colon;
    Block* block;

    WhileStatement(Token while_token, Expression* condition, Token colon, Block* block)
        : while_token(while_token), condition(condition), colon(colon), block(block) {}

    void accept(Visitor& visitor) const override;
};

struct ElifBlock {
    Token elif_token;
    Expression* condition;
    Token colon;
    Block* block;

    ElifBlock(Token elif_token, Expression* condition, Token colon, Block* block)
        : elif_token(elif_token), condition(condition), colon(colon), block(block) {}
    void accept(Visitor& visitor) const override;
};

struct ElseBlock {
    Token else_token;
    Token colon;
    Block* block;

    ElseBlock(Token else_token, Token colon, Block* block)
        : else_token(else_token), colon(colon), block(block) {}
    void accept(Visitor& visitor) const override;
};

struct IfStatement : CompoundStatment {
    Token if_token;
    Expression* condition;
    Token colon;
    Block* block;
    std::vector<ElifBlock*> elif_blocks;
    ElseBlock* else_block;

    IfStatement(Token if_token, Expression* condition, Token colon, Block* block,
        std::vector<ElifBlock*> elif_blocks = {}, ElseBlock* else_block = nullptr)
        : if_token(if_token), condition(condition), colon(colon), block(block),
          elif_blocks(std::move(elif_blocks)), else_block(else_block) {}

    void accept(Visitor& visitor) const override;
};

struct ReturnStatement : SimpleStatment {
    Token return_token;
    Expression* value;

    ReturnStatement(Token return_token, Expression* value)
        : return_token(return_token), value(value) {}

    void accept(Visitor& visitor) const override;
};


struct Join : Expression {
    Expression* left;
    Token op; // 'or' or 'and'
    Expression* right;

    Join(Expression* left, Token op, Expression* right)
        : left(left), op(op), right(right) {}

    void accept(Visitor& visitor) const override;
};

struct Equality : Expression {
    Expression* left;
    Token op; // '==', '!='
    Expression* right;

    Equality(Expression* left, Token op, Expression* right)
        : left(left), op(op), right(right) {}

    void accept(Visitor& visitor) const override;
};

struct Relation : Expression {
    Expression* left;
    Token op; // '<', '<=', '>=', '>'
    Expression* right;

    Relation(Expression* left, Token op, Expression* right)
        : left(left), op(op), right(right) {}

    void accept(Visitor& visitor) const override;
};

struct NumExpr : Expression {
    Expression* left;
    Token op; // '+', '-'
    Expression* right;

    NumExpr(Expression* left, Token op, Expression* right)
        : left(left), op(op), right(right) {}

    void accept(Visitor& visitor) const override;
};

struct Term : Expression {
    Expression* left;
    Token op; // '*', '//'
    Expression* right;

    Term(Expression* left, Token op, Expression* right)
        : left(left), op(op), right(right) {}

    void accept(Visitor& visitor) const override;
};

struct Unary : Expression {
    Token op; // 'not', '-'
    Expression* expr;

    Unary(Token op, Expression* expr) : op(op), expr(expr) {}

    void accept(Visitor& visitor) const override;
};

struct Factor : Expression {
    Token LP; 
    Expression* expr;
    Token RP; 
    Token id; 
    Token num; // numeric constant
    Token boolean; // True or False

    Factor(Token LP, Expression* expr, Token RP)
        : LP(LP), expr(expr), RP(RP), id(), num(), boolean() {}

    Factor(Token id) : LP(), expr(nullptr), RP(), id(id), num(), boolean() {}
    Factor(Token num, bool isNum) : LP(), expr(nullptr), RP(), id(), num(num), boolean() {}
    Factor(Token boolean, bool isBoolean) : LP(), expr(nullptr), RP(), id(), num(), boolean(boolean) {}

    void accept(Visitor& visitor) const override;
};

// non lo ho usato
struct Location : Expression {
    Token id; 
    Token LB; 
    Expression* index; 
    Token RB; 

    Location(Token id, Token LB, Expression* index, Token RB)
        : id(id), LB(LB), index(index), RB(RB) {}

    Location(Token id) : id(id), LB(), index(nullptr), RB() {}

    void accept(Visitor& visitor) const override;
};

struct Literal {
    Token value; // può essere un numero, True o False

    Literal(Token value) : value(value) {}

    void accept(Visitor& visitor) const {
        visitor.visit(*this);
    }
};


#endif