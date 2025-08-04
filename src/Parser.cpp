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

Program* Parser::ParseProgram(auto& itr) {
    Program* program = new Program();
    while (itr != end) {
        Statement* stmt = ParseStatement(itr);
        if (stmt) { // verifico che esista un'istruzione valida
            program->stmts.push_back(stmt);
        }
    }
    return program;
}

Statement* Parser::ParseStatement(auto& itr){
    // due casi simple e compound ed errore
    if (itr == end) {
        return nullptr; // fine del file
    }
    if (itr->tag == Token::ID) {
        Token id = *itr;
        Next(itr);
       if (itr->tag == Token::LB) {
        // parsing di id[expr] = expr nl
        Token LB = *itr;
        Next(itr);
        Expression* pos = ParseExpression(itr);
        if (itr->tag != Token::RB) throw ParseException("Expected ']' after expression");
        Token RB = *itr;
        Next(itr);
        if (itr->tag != Token::EQ) throw ParseException("Expected '='");
        Token eq = *itr;
        Next(itr);
        Expression* value = ParseExpression(itr);
        Next(itr);
        if (itr->tag != Token::NEWLINE) throw ParseException("Expected newline after assignment");
        return new ListAssignment(id, LB, pos, RB, eq, value);

        } else if (itr->tag == Token::EQ) {
            // casi = expr o id =list()
            Token eq = *itr;
            Next(itr);
            if (itr->tag == Token::LIST){
                // parsing di id = list() nl
                Token list_token = *itr;
                Next(itr);
                if (itr->tag != Token::LP) throw ParseException("Expected '(' after 'list'");
                Token LP = *itr;
                Next(itr);
                if (itr->tag != Token::RP) throw ParseException("Expected ')' after 'list'");
                Token RP = *itr;
                Next(itr);
                if (itr->tag != Token::NEWLINE) throw ParseException("Expected newline after list creation");
                return new ListCreation(id, eq, list_token, LP, RP);
            } else {
                // parsing di id = expr nl
                Next(itr);
                Expression* value = ParseExpression(itr);
                Next(itr);
                if (itr->tag != Token::NEWLINE) throw ParseException("Expected newline after assignment");
                return new Assignment(id, eq, value);
            }
        } else if (itr->tag == Token::DOT) {
            // parsing di id.append(expr) nl
            Token dot = *itr;
            Next(itr);
            if (itr->tag != Token::APPEND) throw ParseException("Manca append");
            Token append_token = *itr;
            Next(itr);
            if (itr->tag != Token::LP) throw ParseException("Manca (");
            Token LP = *itr;
            Next(itr);
            Expression* value = ParseExpression(itr);
            if (itr->tag != Token::RP) throw ParseException("Manca )");
            Token RP = *itr;
            Next(itr);
            if (itr->tag != Token::NEWLINE) throw ParseException("Expected newline after append");
            return new Append(id, dot, append_token, LP, value, RP);
        } else {
            throw ParseException("Errore di sintassi dopo l'ID");
        }

    } 

}
    