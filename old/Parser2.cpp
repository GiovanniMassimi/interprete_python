#include <sstream>

#include "Parser.h"
#include "Exception.h"
#include "Token.h"

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

Program* Parser::ParseProgram(auto& itr, auto& end) {
    Program* program = new Program();
    while (itr != end) {
        Statement* stmt = ParseStatement(itr);
        if (stmt) { // verifico che esista un'istruzione valida
            program->stmts.push_back(stmt);
        }
    }
    return program;
}

//////////////////////////////////////////
//          Statement Parsing           //
//////////////////////////////////////////

Statement* Parser::ParseStatement(auto& itr, const auto& end) {

    if (itr == end) return nullptr; // se non ci sono più token, ritorno nullptr

        switch (itr->tag) {
            case Token::ID: {
                // parsing di assegnazione o accesso a lista
                if (std::next(itr) != end && std::next(itr)->tag == Token::EQ) {
                    // devo verifiare non ci sia un list creation
                    if (std::next(itr, 2) != end && std::next(itr, 2)->tag == Token::LIST) {
                        return ParseListCreation(itr, end);
                    }
                    return ParseAssignment(itr, end);
                } else if (std::next(itr) != end && std::next(itr)->tag == Token::LB) {
                    return ParseListAssignment(itr, end);
                } else if (std::next(itr) != end && std::next(itr)->tag == Token::DOT) {
                    return ParseAppend(itr, end);
                } else {
                    throw ParseError("Error: (parsing) expected '=' or '[' after identifier.");
                }
            }
            case Token::BREAK:
                return ParseBreak(itr, end);
            case Token::CONTINUE:
                return ParseContinue(itr, end);
            case Token::PRINT:
                return ParsePrint(itr, end);
            case Token::IF:
                return ParseIfStatement(itr, end);
            case Token::WHILE:
                return ParseWhileStatement(itr, end);
            default:
                throw ParseError("Error: (parsing) unrecognized statement.");
        }
    }

Assignment* Parser::ParseAssignment(auto& itr, const auto& end) {
    Token id = *itr;
    Next(itr, end); // passo al token successivo
    if (itr->tag != Token::EQ) {
        throw ParseError(GenError(itr, Token::EQ));
    }
    Token eq = *itr;
    Next(itr, end);
    Expression* value = ParseExpression(itr, end);
    if (value == nullptr) {
        throw ParseError("Error: (parsing) missing value after '='.");
    }
    return new Assignment(id, eq, value);
}

ListAssignment* Parser::ParseListAssignment(auto& itr, const auto& end) {
    Token id = *itr;
    Next(itr, end);
    if (itr->tag != Token::LB) {
        throw ParseError(GenError(itr, Token::LB));
    }
    Token LB = *itr;
    Next(itr, end);
    Expression* pos = ParseExpression(itr, end);
    if (pos == nullptr) {
        throw ParseError("Error: (parsing) missing expression between '[' and ']'.");
    }
    if (itr->tag != Token::RB) {
        throw ParseError(GenError(itr, Token::RB));
    }
    Token RB = *itr;
    Next(itr, end);
    if (itr->tag != Token::EQ) {
        throw ParseError(GenError(itr, Token::EQ));
    }
    Token eq = *itr;
    Next(itr, end);
    Expression* value = ParseExpression(itr, end);
    if (value == nullptr) {
        throw ParseError("Error: (parsing) missing value after '='.");
    }
    return new ListAssignment(id, LB, pos, RB, eq, value);
}

ListCreation* Parser::ParseListCreation(auto& itr, const auto& end) {
    Token id = *itr;
    Next(itr, end);
    if (itr->tag != <expr>→ <expr>or <join>| <join>Token::EQ) {
        throw ParseError(GenError(itr, Token::EQ));
    }
    Token eq = *itr;
    Next(itr, end);
    if (itr->tag != Token::LIST) {
        throw ParseError(GenError(itr, Token::LIST));
    }
    Token list_token = *itr;
    Next(itr, end);
    if (itr->tag != Token::LP) {
        throw ParseError(GenError(itr, Token::LP));
    }
    Token LP = *itr;
    Next(itr, end);
    if (itr->tag != Token::RP) {
        throw ParseError(GenError(itr, Token::RP));
    }
    Token RP = *itr;
    Next(itr, end);
    if (itr->tag != Token::NEWLINE) {
        throw ParseError(GenError(itr, Token::NEWLINE));
    }
   
    Next(itr, end);
    
    return new ListCreation(id, eq, list_token, LP, RP);
}

Append* Parser::ParseAppend(auto& itr, const auto& end) {
    Token id = *itr;
    Next(itr, end);
    if (itr->tag != Token::DOT) {
        throw ParseError(GenError(itr, Token::DOT));
    }
    Token dot = *itr;
    Next(itr, end);
    if (itr->tag != Token::APPEND) {
        throw ParseError(GenError(itr, Token::APPEND));
    }
    Token append_token = *itr;
    Next(itr, end);
    if (itr->tag != Token::LP) {
        throw ParseError(GenError(itr, Token::LP));
    }
    Token LP = *itr;
    Next(itr, end);
    Expression* value = ParseExpression(itr, end);
    if (value == nullptr) {
        throw ParseError("Error: (parsing) missing value after 'append('.");
    }
    if (itr->tag != Token::RP) {
        throw ParseError(GenError(itr, Token::RP));
    }
    Token RP = *itr;
    Next(itr, end);
    if (itr->tag != Token::NEWLINE) {
        throw ParseError(GenError(itr, Token::NEWLINE));
    }
    
    Next(itr, end);

    return new Append(id, dot, append_token, LP, value, RP);
}

Break* Parser::ParseBreak(auto& itr, const auto& end) {
    Token break_token = *itr;
    Next(itr, end);
    if (itr->tag != Token::NEWLINE) {
        throw ParseError(GenError(itr, Token::NEWLINE));
    }
    
    Next(itr, end);
   
    return new Break(break_token);
}

Continue* Parser::ParseContinue(auto& itr, const auto& end) {
    Token continue_token = *itr;
    Next(itr, end);
    if (itr->tag != Token::NEWLINE) {
        throw ParseError(GenError(itr, Token::NEWLINE));
    }
    
    Next(itr, end);
    return new Continue(continue_token);
}

Print* Parser::ParsePrint(auto& itr, const auto& end) {
    Token print_token = *itr;
    Next(itr, end);
    if (itr->tag != Token::LP) {
        throw ParseError(GenError(itr, Token::LP));
    }
    Token LP = *itr;
    Next(itr, end);
    Expression* value = ParseExpression(itr, end);
    if (value == nullptr) {
        throw ParseError("Error: (parsing) missing value after 'print('.");
    }
    if (itr->tag != Token::RP) {
        throw ParseError(GenError(itr, Token::RP));
    }
    Token RP = *itr;
    Next(itr, end);

    return new Print(print_token, LP, value, RP);
}



//////////////////////////////////////////
//          Expression Parsing          //
//////////////////////////////////////////

Expression* Parser::ParseExpression(auto& itr) {
    // parsing di espressioni
    Expression* left = ParseJoin(itr);
    while (itr != end && itr->tag == Token::OR) {
        Token op = *itr;
        Next(itr,end);
        Expression* right = ParseJoin(itr);
        left = new BinaryExpression(left, op, right);
    }
    return left;
}

Expression* Parser::ParseJoin(auto& itr) {
    // parsing di join
    Expression* left = ParseEquality(itr);
    while (itr != end && itr->tag == Token::AND) {
        Token op = *itr;
        Next(itr,end);
        Expression* right = ParseEquality(itr);
        left = new BinaryExpression(left, op, right);
    }
    return left;
}

Expression* Parser::ParseEquality(auto& itr) {
    // parsing di uguaglianze
    Expression* left = ParseRelation(itr);
    while (itr != end && (itr->tag == Token::EQEQ || itr->tag == Token::NOTEQ)) {
        Token op = *itr;
        Next(itr,end);
        Expression* right = ParseRelation(itr);
        left = new Equality(left, op, right);
    }
    return left;
}

Expression* Parser::ParseRelation(auto& itr) {
    // parsing di relazioni
    Expression* left = ParseNumExpr(itr);
    while (itr != end && (itr->tag == Token::LT || itr->tag == Token::LE || itr->tag == Token::GE || itr->tag == Token::GT)) {
        Token op = *itr;
        Next(itr,end);
        Expression* right = ParseNumExpr(itr);
        left = new Relation(left, op, right);
    }
    return left;
}

Expression* Parser::ParseNumExpr(auto& itr) {
    // parsing di espressioni numeriche
    Expression* left = ParseTerm(itr);
    while (itr != end && (itr->tag == Token::ADD || itr->tag == Token::SUB)) {
        Token op = *itr;
        Next(itr,end);
        Expression* right = ParseTerm(itr);
        left = new NumExpr(left, op, right);
    }
    return left;
}

Expression* Parser::ParseTerm(auto& itr) {
    // parsing di termini
    Expression* left = ParseUnary(itr);
    //while (itr != end && (itr->tag == Token::MUL || itr->tag == Token::DIV || itr->tag == Token::DIVINT || itr->tag == Token::MOD)) {

    while (itr != end && (itr->tag == Token::MUL || itr->tag == Token::DIV || itr->tag == Token::DIVINT)) {
        Token op = *itr;
        Next(itr,end);
        Expression* right = ParseUnary(itr);
        left = new Term(left, op, right);
    }
    return left;
}

Expression* Parser::ParseUnary(auto& itr) {
    // parsing di unari
    if (itr != end && (itr->tag == Token::ADD || itr->tag == Token::SUB)) {
        Token op = *itr;
        Next(itr,end);
        Expression* right = ParseUnary(itr);
        return new Unary(op, right);
    }
    return ParseFactor(itr);
}

// parse factor
Expression* Parser::ParseFactor(auto& itr) {
    // parsing di fattori
    if (itr == end) {
        throw ParseError("Error: (parsing) missing factor.");
    }

    if (itr->tag == Token::LP) {
        Token LP = *itr;
        Next(itr,end);
        Expression* expr = ParseExpression(itr);
        if (itr->tag != Token::RP) throw ParseError(GenError(itr, Token::RP));
        Token RP = *itr;
        Next(itr,end);
        return new Factor(LP, expr, RP);
    }

    if (itr->tag == Token::ID) {
        //manca
    }

    if (itr->tag == Token::NUM) {
        Token num = *itr;
        Next(itr,end);
        return new Literal(num);
    }

    if (itr->tag == Token::TRUE) {
        Token true_token = *itr;
        Next(itr,end);
        return new Literal(true_token);
    }

    if (itr->tag == Token::FALSE) {
        Token false_token = *itr;
        Next(itr,end);
        return new Literal(false_token);
    }

    throw ParseError("Error: (parsing) invalid factor.");
}

IfStatement* Parser::ParseIfStatement(auto& itr) {
    // parsing di if expr: block elif_block else_block
    Token if_token = *itr;
    Next(itr,end);
    Expression* condition = ParseExpression(itr);
    if (itr->tag != Token::COLON) throw ParseError(GenError(itr, Token::COLON));
    Token colon = *itr;
    Next(itr,end);
    Block* block = ParseBlock(itr);

    std::vector<ElifBlock*> elif_blocks;
    while (itr != end && itr->tag == Token::ELIF) {
        elif_blocks.push_back(ParseElifBlock(itr));
    }

    ElseBlock* else_block = nullptr;
    if (itr != end && itr->tag == Token::ELSE) {
        else_block = ParseElseBlock(itr);
    }

    return new IfStatement(if_token, condition, colon, block, elif_blocks, else_block);
}

ElifBlock* Parser::ParseElifBlock(auto& itr) {
    Token elif_token = *itr;
    Next(itr,end);
    Expression* condition = ParseExpression(itr);
    if (itr->tag != Token::COLON) throw ParseError(GenError(itr, Token::COLON));
    Token colon = *itr;
    Next(itr,end);
    Block* block = ParseBlock(itr);
    return new ElifBlock(elif_token, condition, colon, block);
}

ElseBlock* Parser::ParseElseBlock(auto& itr) {
    Token else_token = *itr;
    Next(itr,end);
    if (itr->tag != Token::COLON) throw ParseError(GenError(itr, Token::COLON));
    Token colon = *itr;
    Next(itr,end);
    Block* block = ParseBlock(itr);
    return new ElseBlock(else_token, colon, block);
}

WhileStatement* Parser::ParseWhileStatement(auto& itr) {
    // parsing di while expr: block
    Token while_token = *itr;
    Next(itr,end);
    Expression* condition = ParseExpression(itr);
    if (itr->tag != Token::COLON) throw ParseError(GenError(itr, Token::COLON));
    Token colon = *itr;
    Next(itr,end);
    Block* block = ParseBlock(itr);

    return new WhileStatement(while_token, condition, colon, block);
}

Block* Parser::ParseBlock(auto& itr) {
    // parsing di blocchi
    if (itr->tag != Token::NEWLINE) throw ParseError(GenError(itr, Token::NEWLINE));
    Token newline = *itr;
    Next(itr,end);
    if (itr->tag != Token::INDENT) throw ParseError(GenError(itr, Token::INDENT));
    Token indent = *itr;
    Next(itr,end);

    std::vector<Statement*> stmts;
    while (itr != end && itr->tag != Token::DEDENT) {
        Statement* stmt = ParseStatement(itr);
        if (stmt) {
            stmts.push_back(stmt);
        }
    }

    if (itr->tag != Token::DEDENT) throw ParseError(GenError(itr, Token::DEDENT));
    Token dedent = *itr;
    Next(itr,end);

    return new Block(newline, indent, stmts, dedent);
}
