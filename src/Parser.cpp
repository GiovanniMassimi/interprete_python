#include <sstream>

#include "Parser.h"
#include "Exception.h"
#include "Token.h"
#define DEBUG_ON 1

#if DEBUG_ON
    #define DEBUG_TRACE(msg) std::cerr << "[TRACE] " << msg << std::endl
#else
    #define DEBUG_TRACE(msg) // niente, disabilitato
#endif
Program* Parser::ParseProgram(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    Program* program = new Program();
    DEBUG_TRACE("ParseProgram");
    while (itr != end && itr->tag != Token::END) {
    Statement* stmt = ParseStatement(itr, end);
    if (stmt) {
        program->stmts.push_back(stmt);
    }
    }
    if (itr != end && itr->tag == Token::END) {
        Next(itr, end);
    }
    return program;
}

//parse location
Location* Parser::ParseLocation(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    if (itr == end || itr->tag != Token::ID) {
        throw ParseError("Errore di sintassi: atteso identificatore.");
    }

    Token id = *itr;
    DEBUG_TRACE("ParseLocation: " << *itr);
    Next(itr, end); 

    if (itr != end && itr->tag == Token::LB) {
        // caso: id [ expr ]
        Next(itr,end); // salta '['
        Expression* index = ParseExpression(itr, end);
        DEBUG_TRACE("Dopo ParseExpression, token attuale: (loc) " << *itr);
        if (itr == end || itr->tag != Token::RB) {
            throw ParseError(GenError(itr, Token::RB));
        }
        Next(itr,end); // salta ']'
        return new Location(id, index);
    }

    // caso: solo id
    return new Location(id, nullptr);
}
//////////////////////////////////////////
//          Statement Parsing           //
//////////////////////////////////////////


Statement* Parser::ParseStatement(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    if (itr == end) {
        throw ParseError("Unexpected end of input while parsing statement.");
    }
    DEBUG_TRACE("ParseStatement: " << *itr);
    switch (itr->tag) {
        case Token::ID: {
            //options : listcreatio, listassgnmet,listcreation, append, assignment
            if (std::next(itr) != end && std::next(itr)->tag == Token::EQ){
                if (std::next(itr, 2) != end && std::next(itr, 2)->tag == Token::LIST) {
                    return ParseListCreation(itr, end);
                }
                return ParseAssignment(itr, end);
            } else if (std::next(itr) != end && std::next(itr)->tag == Token::LB) {
                return ParseListAssignment(itr, end);
            }else if (std::next(itr) != end && std::next(itr)->tag == Token::DOT &&
                       std::next(std::next(itr))->tag == Token::APPEND) {
                return ParseAppend(itr, end);
            } else {
                //maybe
                throw ParseError("Invalid statement after identifier: " + itr->word + " at line " + std::to_string(itr->pos.first) + ", column " + std::to_string(itr->pos.second));
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
            throw ParseError("Invalid statement token: " + itr->word);
    }
    return nullptr; // No valid statement found
}

Assignment* Parser::Parser::ParseAssignment(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    Token id = *itr;
    DEBUG_TRACE("ParseAssignment: " << *itr);
    Next(itr, end); 
    
    if (itr->tag != Token::EQ) {
        throw ParseError(GenError(itr, Token::EQ));
    }
    Token eq = *itr;
    Next(itr, end);
    DEBUG_TRACE("Prima ParseExpression, token attuale: " << *itr);
    Expression* value = ParseExpression(itr, end);
    DEBUG_TRACE("Dopo ParseExpression, token attuale: (ass)" << *itr);
    if (value == nullptr) {
        throw ParseError("Syntax error: missing value after '='.");
    }
    DEBUG_TRACE("ParseAssignment after value: " << *itr);
    if (itr->tag != Token::NEWLINE) {
        throw ParseError(GenError(itr, Token::NEWLINE));
    }
    Next(itr, end);
    DEBUG_TRACE("Assignment " << *itr);
    return new Assignment(id, value);
}

ListAssignment* Parser::ParseListAssignment(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    Token id = *itr;
    DEBUG_TRACE("ParseListAssignment: " << *itr);
    Next(itr, end);
    if (itr->tag != Token::LB) {
        throw ParseError(GenError(itr, Token::LB));
    }
    Next(itr, end);
    Expression* pos = ParseExpression(itr, end);
    DEBUG_TRACE("Dopo ParseExpression, token attuale: (listass) " << *itr);
    if (pos == nullptr) {
        throw ParseError("Syntax error: missing position after '['.");
    }
    if (itr->tag != Token::RB) {
        throw ParseError(GenError(itr, Token::RB));
    }
    Next(itr, end);
    if (itr->tag != Token::EQ) {
        throw ParseError(GenError(itr, Token::EQ));
    }
    Next(itr, end);
    Expression* value = ParseExpression(itr, end);
    DEBUG_TRACE("Dopo ParseExpression, token attuale: (listass2) " << *itr);
    if (value == nullptr) {
        throw ParseError("Syntax error: missing value after '='.");
    }
    //Next(itr, end);  //ricordatelo
    if (itr->tag != Token::NEWLINE) {
        throw ParseError(GenError(itr, Token::NEWLINE));
    }
    Next(itr, end);
    return new ListAssignment(id, pos, value);
}

ListCreation* Parser::ParseListCreation(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    Token id = *itr;
    DEBUG_TRACE("ParseListCreation: " << *itr);
    Next(itr, end);
    if (itr->tag != Token::EQ) {
        throw ParseError(GenError(itr, Token::EQ));
    }
    Next(itr, end);
    if (itr->tag != Token::LIST) {
        throw ParseError(GenError(itr, Token::LIST));
    }
    Next(itr, end);
    if (itr->tag != Token::LP) {
        throw ParseError(GenError(itr, Token::LP));
    }
    Next(itr, end);
    if (itr->tag != Token::RP) {
        throw ParseError(GenError(itr, Token::RP));
    }
    Next(itr, end);
    if (itr->tag != Token::NEWLINE) {
        throw ParseError(GenError(itr, Token::NEWLINE));
    }
    
    Next(itr, end);

    return new ListCreation(id);
}

Append* Parser::ParseAppend(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    Token id = *itr;
    DEBUG_TRACE("ParseAppend: " << *itr);
    Next(itr, end);
    if (itr->tag != Token::DOT) {
        throw ParseError(GenError(itr, Token::DOT));
    }
    Next(itr, end);
    if (itr->tag != Token::APPEND) {
        throw ParseError(GenError(itr, Token::APPEND));
    }
    Next(itr, end);
    if (itr->tag != Token::LP) {
        throw ParseError(GenError(itr, Token::LP));
    }
    Next(itr, end);
    Expression* value = ParseExpression(itr, end);
    DEBUG_TRACE("Dopo ParseExpression, token attuale: (append) " << *itr);
    if (value == nullptr) {
        throw ParseError("Syntax error: missing value after 'append('.");
    }
    if (itr->tag != Token::RP) {
        throw ParseError(GenError(itr, Token::RP));
    }
    Next(itr, end);
    if (itr->tag != Token::NEWLINE) {
        throw ParseError(GenError(itr, Token::NEWLINE));
    }
    
    Next(itr, end);

    return new Append(id, value);
}

Break* Parser::ParseBreak(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    int line = itr->pos.first;
    DEBUG_TRACE("ParseBreak: " << *itr);
    Next(itr, end);
    if (itr->tag != Token::NEWLINE) {
        throw ParseError(GenError(itr, Token::NEWLINE));
    }
    
    Next(itr, end);

    return new Break(line);
}

Continue* Parser::ParseContinue(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    int line = itr->pos.first;
    DEBUG_TRACE("ParseContinue: " << *itr);
    Next(itr, end);
    if (itr->tag != Token::NEWLINE) {
        throw ParseError(GenError(itr, Token::NEWLINE));
    }
    
    Next(itr, end);

    return new Continue(line);
}

Print* Parser::ParsePrint(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    DEBUG_TRACE("ParsePrint: " << *itr);
    Next(itr, end);
    if (itr->tag != Token::LP) {
        throw ParseError(GenError(itr, Token::LP));
    }
    Next(itr, end);
    Expression* expr = ParseExpression(itr, end);
    DEBUG_TRACE("Dopo ParseExpression, token attuale: (print) " << *itr);
    if (expr == nullptr) {
        throw ParseError("Syntax error: missing value after 'print('.");
    }
    if (itr->tag != Token::RP) {
        throw ParseError(GenError(itr, Token::RP));
    }
    
    Next(itr, end);
    if (itr->tag != Token::NEWLINE) {
        throw ParseError(GenError(itr, Token::NEWLINE));
    }
    
    Next(itr, end);

    return new Print(expr);
}

//////////////////////////////////////////
//          Expression Parsing          //
//////////////////////////////////////////

Expression* Parser::ParseExpression(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    DEBUG_TRACE("ParseExpression: " << *itr);
    Expression* left = ParseJoin(itr, end);
    while (itr != end && itr->tag == Token::OR) {
        Token op = *itr;

        Next(itr,end);
        Expression* right = ParseJoin(itr, end);
        left = new Join(left, op, right);
    }
    return left;
}

Expression* Parser::ParseJoin(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    //DEBUG_TRACE("ParseJoin: " << *itr);
    Expression* left = ParseEquality(itr, end);
    
    while (itr != end && itr->tag == Token::AND) {
        Token op = *itr;
        Next(itr,end);
        Expression* right = ParseEquality(itr, end);
        left = new Join(left, op, right);
    }
    return left;
}

Expression* Parser::ParseEquality(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    //DEBUG_TRACE("ParseEquality: " << *itr);
    Expression* left = ParseRelation(itr, end);
    
    while (itr != end && (itr->tag == Token::EQEQ || itr->tag == Token::NOTEQ)) {
        Token op = *itr;
        Next(itr,end);
        Expression* right = ParseRelation(itr, end);
        left = new Equality(left, op, right);
    }
    return left;
}

Expression* Parser::ParseRelation(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    //DEBUG_TRACE("ParseRelation: " << *itr);
    Expression* left = ParseNumExpr(itr, end);
    
    while (itr != end && (itr->tag == Token::LT || itr->tag == Token::LE || itr->tag == Token::GE || itr->tag == Token::GT)) {
        Token op = *itr;
        Next(itr,end);
        Expression* right = ParseNumExpr(itr, end);
        left = new Relation(left, op, right);
    }
    return left;
}

Expression* Parser::ParseNumExpr(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    //DEBUG_TRACE("ParseNumExpr: " << *itr);
    Expression* left = ParseTerm(itr, end);
    
    while (itr != end && (itr->tag == Token::ADD || itr->tag == Token::SUB)) {
        Token op = *itr;
        Next(itr,end);
        Expression* right = ParseTerm(itr, end);
        left = new NumExpr(left, op, right);
    }
    return left;
}

Expression* Parser::ParseTerm(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    //DEBUG_TRACE("ParseTerm: " << *itr);
    Expression* left = ParseUnary(itr, end);
    
    while (itr != end && (itr->tag == Token::MUL || itr->tag == Token::DIV || itr->tag == Token::DIVINT || itr->tag == Token::MOD)) {
        Token op = *itr;
        Next(itr,end);
        Expression* right = ParseUnary(itr, end);
        left = new Term(left, op, right);
    }
    return left;
}

Expression* Parser::ParseUnary(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    //DEBUG_TRACE("ParseUnary: " << *itr);
    if (itr != end && (itr->tag == Token::NOT || itr->tag == Token::SUB)) {
        Token op = *itr;
        Next(itr, end);
        Expression* right = ParseUnary(itr, end);
        return new Unary(op, right);
    }
    return ParseFactor(itr, end);
}

Expression* Parser::ParseFactor(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    if (itr == end) {
        throw ParseError("Errore di sintassi: atteso un fattore.");
    }
    DEBUG_TRACE("ParseFactor: " << *itr);
    if (itr->tag == Token::LP) {
        Token lp = *itr;
        Next(itr,end);
        Expression* expr = ParseExpression(itr, end);
        if (itr == end || itr->tag != Token::RP) {
            throw ParseError(GenError(itr, Token::RP));
        }
        Next(itr,end); // salta ')'
        DEBUG_TRACE("ParseFactor pos: " << *itr);
        // parentesi → usa Factor con expr, token non significativo
        Literal placeholder(Token(Token::LP, "(", {0, 0}));

        //forse bisogna controllare new line |||||||||||||||||||||||||||||||||||||||||||||||||||||
        return new Factor(placeholder, expr);
    }

    if (itr->tag == Token::ID) {
        DEBUG_TRACE("ParseFactor ID: " << *itr);
        Location* loc = ParseLocation(itr, end);
        return new Factor(Literal(Token(Token::ID, "id", {0,0})), loc);
    }

    if (itr->tag == Token::NUM || itr->tag == Token::TRUE_ || itr->tag == Token::FALSE_) {
        Literal lit(*itr);
        
        Next(itr, end);
        
        return new Factor(lit, nullptr); // solo literal, nessuna expr
    }

    throw ParseError(" Invalid factor, current token: '" + itr->word + "' at line " + std::to_string(itr->pos.first) + ", column " + std::to_string(itr->pos.second));
}


/////////////////////////////////////////////
//          compound statement             //
/////////////////////////////////////////////

Block* Parser::ParseBlock(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    if (itr == end) {
        throw ParseError("Unexpected end of input while parsing block.");
    }
    DEBUG_TRACE("ParseBlock: " << *itr);
    if (itr->tag != Token::NEWLINE) {
        throw ParseError(GenError(itr, Token::NEWLINE));
    }
    Next(itr, end); // salta NEWLINE
    if (itr->tag != Token::INDENT) {
        throw ParseError(GenError(itr, Token::INDENT));
    }
    Next(itr, end); // salta INDENT
    std::vector<Statement*> stmts;
    while (itr != end && itr->tag != Token::DEDENT) {
        Statement* stmt = ParseStatement(itr, end);
        if (stmt) {
            stmts.push_back(stmt);
        }
    }
    if (itr == end || itr->tag != Token::DEDENT) {
        throw ParseError(GenError(itr, Token::DEDENT));
    }
    Next(itr, end); // salta DEDENT 

    return new Block(stmts);
}

ElifBlock* Parser::ParseElifBlock(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    DEBUG_TRACE("ParseElifBlock: " << *itr);
    Next(itr, end);
    Expression* condition = ParseExpression(itr, end);
    if (itr->tag != Token::COLON) throw ParseError(GenError(itr, Token::COLON));
    Next(itr, end);
    Block* block = ParseBlock(itr, end);
    return new ElifBlock(condition, block);
}

ElseBlock* Parser::ParseElseBlock(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    DEBUG_TRACE("ParseElseBlock: " << *itr);
    Next(itr, end);
    if (itr->tag != Token::COLON) throw ParseError(GenError(itr, Token::COLON));
    Next(itr, end);
    Block* block = ParseBlock(itr, end);
    return new ElseBlock(block);
}

IfStatement* Parser::ParseIfStatement(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    DEBUG_TRACE("ParseIfStatement: " << *itr);
    Next(itr, end);
    Expression* condition = ParseExpression(itr, end);
    DEBUG_TRACE("Dopo ParseExpression, token attuale: (if) " << *itr);
    if (itr->tag != Token::COLON) throw ParseError(GenError(itr, Token::COLON));
    Next(itr, end);
    Block* block = ParseBlock(itr, end);

    std::vector<ElifBlock*> elif_blocks;
    while (itr != end && itr->tag == Token::ELIF) {
        elif_blocks.push_back(ParseElifBlock(itr, end));
    }

    ElseBlock* else_block = nullptr;
    if (itr != end && itr->tag == Token::ELSE) {
        else_block = ParseElseBlock(itr, end);
    }

    return new IfStatement(condition, block, elif_blocks, else_block);
}



WhileStatement* Parser::ParseWhileStatement(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
    DEBUG_TRACE("ParseWhileStatement: " << *itr);
    Next(itr, end);
    Expression* condition = ParseExpression(itr, end);
    DEBUG_TRACE("Dopo ParseExpression, token attuale: (while) " << *itr);
    if (itr->tag != Token::COLON) throw ParseError(GenError(itr, Token::COLON));
    Next(itr, end);
    Block* block = ParseBlock(itr, end);

    return new WhileStatement(condition, block);
}
