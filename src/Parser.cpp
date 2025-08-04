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

//////////////////////////////////////////
//          Statement Parsing           //
//////////////////////////////////////////

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
        if (itr->tag != Token::RB) throw ParseException(GenError(itr, Token::RB));
        Token RB = *itr;
        Next(itr);
        if (itr->tag != Token::EQ) throw ParseException(GenError(itr, Token::EQ));
        Token eq = *itr;
        Next(itr);
        Expression* value = ParseExpression(itr);
        Next(itr);
        if (itr->tag != Token::NEWLINE) throw ParseException(GenError(itr, Token::NEWLINE));
        return new ListAssignment(id, LB, pos, RB, eq, value);

        } else if (itr->tag == Token::EQ) {
            // casi = expr o id =list()
            Token eq = *itr;
            Next(itr);
            if (itr->tag == Token::LIST){
                // parsing di id = list() nl
                Token list_token = *itr;
                Next(itr);
                if (itr->tag != Token::LP) throw ParseException(GenError(itr, Token::LP));
                Token LP = *itr;
                Next(itr);
                if (itr->tag != Token::RP) throw ParseException(GenError(itr, Token::RP));
                Token RP = *itr;
                Next(itr);
                if (itr->tag != Token::NEWLINE) throw ParseException(GenError(itr, Token::NEWLINE));
                return new ListCreation(id, eq, list_token, LP, RP);
            } else {
                // parsing di id = expr nl
                Next(itr);
                Expression* value = ParseExpression(itr);
                Next(itr);
                if (itr->tag != Token::NEWLINE) throw ParseException(GenError(itr, Token::NEWLINE));
                return new Assignment(id, eq, value);
            }
        } else if (itr->tag == Token::DOT) {
            // parsing di id.append(expr) nl
            Token dot = *itr;
            Next(itr);
            if (itr->tag != Token::APPEND) throw ParseException(GenError(itr, Token::APPEND));
            Token append_token = *itr;
            Next(itr);
            if (itr->tag != Token::LP) throw ParseException(GenError(itr, Token::LP));
            Token LP = *itr;
            Next(itr);
            Expression* value = ParseExpression(itr);
            if (itr->tag != Token::RP) throw ParseException(GenError(itr, Token::RP));
            Token RP = *itr;
            Next(itr);
            if (itr->tag != Token::NEWLINE) throw ParseException(GenError(itr, Token::NEWLINE));
            return new Append(id, dot, append_token, LP, value, RP);
        } else {
            throw ParseException("Errore di sintassi dopo l'ID");
        }

    } else if (itr->tag == Token::PRINT) {
        // parsing di print(expr) nl
        Token print_token = *itr;
        Next(itr);
        if (itr->tag != Token::LP) throw ParseException(GenError(itr, Token::LP));
        Token LP = *itr;
        Next(itr);
        Expression* value = ParseExpression(itr);
        if (itr->tag != Token::RP) throw ParseException(GenError(itr, Token::RP));
        Token RP = *itr;
        Next(itr);
        if (itr->tag != Token::NEWLINE) throw ParseException(GenError(itr, Token::NEWLINE));
        return new Print(print_token, LP, value, RP);
    } else if (itr->tag == Token::BREAK) {
        // parsing di break nl
        Token break_token = *itr;
        Next(itr);
        if (itr->tag != Token::NEWLINE) throw ParseException(GenError(itr, Token::NEWLINE));
        return new Break(break_token);
    } else if (itr->tag == Token::CONTINUE) {
        // parsing di continue nl
        Token continue_token = *itr;
        Next(itr);
        if (itr->tag != Token::NEWLINE) throw ParseException(GenError(itr, Token::NEWLINE));
        return new Continue(continue_token);
    } else if (itr->tag == Token::IF) {
        // parsing di if expr: block elif_block else_block
        return ParseIfStatement(itr);
    } else if (itr->tag == Token::WHILE) {
        // parsing di while expr: block
        return ParseWhileStatement(itr);
    } else {
        throw ParseException("Errore di sintassi: istruzione non riconosciuta");
    }

}

//////////////////////////////////////////
//          Expression Parsing          //
//////////////////////////////////////////

Expression* Parser::ParseExpression(auto& itr) {
    // parsing di espressioni
    Expression* left = ParseJoin(itr);
    while (itr != end && itr->tag == Token::OR) {
        Token op = *itr;
        Next(itr);
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
        Next(itr);
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
        Next(itr);
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
        Next(itr);
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
        Next(itr);
        Expression* right = ParseTerm(itr);
        left = new NumExpr(left, op, right);
    }
    return left;
}

Expression* Parser::ParseTerm(auto& itr) {
    // parsing di termini
    Expression* left = ParseUnary(itr);
    while (itr != end && (itr->tag == Token::MUL || itr->tag == Token::DIV || itr->tag == Token::DIVINT || itr->tag == Token::MOD)) {
        Token op = *itr;
        Next(itr);
        Expression* right = ParseUnary(itr);
        left = new Term(left, op, right);
    }
    return left;
}

Expression* Parser::ParseUnary(auto& itr) {
    // parsing di unari
    if (itr != end && (itr->tag == Token::ADD || itr->tag == Token::SUB)) {
        Token op = *itr;
        Next(itr);
        Expression* right = ParseUnary(itr);
        return new Unary(op, right);
    }
    return ParseFactor(itr);
}

Expression* Parser::ParseFactor(auto& itr) {
    // parsing di fattori
    
    if (itr->tag == Token::LP) {
        Token LP = *itr;
        Next(itr);
        Expression* expr = ParseExpression(itr);
        if (itr->tag != Token::RP) throw ParseException(GenError(itr, Token::RP));
        Token RP = *itr;
        Next(itr);
        return new Parenthesis(LP, expr, RP);
    } else if (itr->tag == Token::ID) {
        Token id = *itr;
        Next(itr);
        if (itr->tag == Token::LB) {
            // parsing di id[expr]
            Token LB = *itr;
            Next(itr);
            Expression* pos = ParseExpression(itr);
            if (itr->tag != Token::RB) throw ParseException(GenError(itr, Token::RB));
            Token RB = *itr;
            Next(itr);
            return new ArrayAccess(id, LB, pos, RB);
        }
        return new Variable(id);
    } else if (itr->tag == Token::NUM || itr->tag == Token::TRUE_ || itr->tag == Token::FALSE_) {
        Token num_token = *itr;
        Next(itr);
        return new Literal(num_token);
    } else {
        sstream err;
        err << "Errore di sintassi: fattore non riconosciuto, trovato '" << Token::id2word[itr->tag] << "' alla riga " << itr->pos.line << ", colonna " << itr->pos.column;
        throw ParseException(err.str());
    }

    if (itr->tag == Token::NUM) {
        Token num = *itr;
        Next(itr);
        return new Literal(num);
    } else if (itr->tag == Token::TRUE_ || itr->tag == Token::FALSE_) {
        Token boolean = *itr;
        Next(itr);
        return new Literal(boolean);
    } else {
        sstream err;
        err << "Errore di sintassi: fattore non riconosciuto, trovato '" << Token::id2word[itr->tag] << "' alla riga " << itr->pos.line << ", colonna " << itr->pos.column;
        throw ParseException(err.str());
    }

}


//patse if statment e while statment e block

IfStatement* Parser::ParseIfStatement(auto& itr) {
    // parsing di if expr: block elif_block else_block
    Token if_token = *itr;
    Next(itr);
    Expression* condition = ParseExpression(itr);
    if (itr->tag != Token::COLON) throw ParseException(GenError(itr, Token::COLON));
    Token colon = *itr;
    Next(itr);
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
    Next(itr);
    Expression* condition = ParseExpression(itr);
    if (itr->tag != Token::COLON) throw ParseException(GenError(itr, Token::COLON));
    Token colon = *itr;
    Next(itr);
    Block* block = ParseBlock(itr);
    return new ElifBlock(elif_token, condition, colon, block);
}

ElseBlock* Parser::ParseElseBlock(auto& itr) {
    Token else_token = *itr;
    Next(itr);
    if (itr->tag != Token::COLON) throw ParseException(GenError(itr, Token::COLON));
    Token colon = *itr;
    Next(itr);
    Block* block = ParseBlock(itr);
    return new ElseBlock(else_token, colon, block);
}

WhileStatement* Parser::ParseWhileStatement(auto& itr) {
    // parsing di while expr: block
    Token while_token = *itr;
    Next(itr);
    Expression* condition = ParseExpression(itr);
    if (itr->tag != Token::COLON) throw ParseException(GenError(itr, Token::COLON));
    Token colon = *itr;
    Next(itr);
    Block* block = ParseBlock(itr);

    return new WhileStatement(while_token, condition, colon, block);
}

Block* Parser::ParseBlock(auto& itr) {
    // parsing di blocchi
    if (itr->tag != Token::NEWLINE) throw ParseException(GenError(itr, Token::NEWLINE));
    Token newline = *itr;
    Next(itr);
    if (itr->tag != Token::INDENT) throw ParseException(GenError(itr, Token::INDENT));
    Token indent = *itr;
    Next(itr);

    std::vector<Statement*> stmts;
    while (itr != end && itr->tag != Token::DEDENT) {
        Statement* stmt = ParseStatement(itr);
        if (stmt) {
            stmts.push_back(stmt);
        }
    }

    if (itr->tag != Token::DEDENT) throw ParseException(GenError(itr, Token::DEDENT));
    Token dedent = *itr;
    Next(itr);

    return new Block(newline, indent, stmts, dedent);
}
