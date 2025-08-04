#if !defined(PARSER_H)
#define PARSER_H

#include <string>
#include <sstream>

#include "Token.h"
#include "Syntax.h"
#include "Exception.h"


/*


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

class Parser {

public:
    //costructur e destructor
    Parser() = default;
    ~Parser() = default;
    //no copy constructor or assignment operator
    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
    //funzione di parsing principale
    Program* parse(const std::vector<Token>& tokens) {
        auto itr = tokens.begin();
        auto end = tokens.end();
        Program* program = ParseProgram(itr, end);
        if (itr != end) {
            throw ParseException("Errore di sintassi: token non elaborati alla fine del parsing.");
        }
        return program;
    }
    

    private:


        Program* ParseProgram(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        Statement* ParseStatement(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        Expression* ParseExpression(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);

        Assignment* ParseAssignment(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        ListAssignment* ParseListAssignment(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        ListCreation* ParseListCreation(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        Append* ParseAppend(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        Break* ParseBreak(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        Continue* ParseContinue(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        Print* ParsePrint(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);

        Block* ParseBlock(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        ElifBlock* ParseElifBlock(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        ElseBlock* ParseElseBlock(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        IfStatement* ParseIfStatement(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        WhileStatement* ParseWhileStatement(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);

        Expression* ParseJoin(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        Expression* ParseEquality(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        Expression* ParseRelation(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        Expression* ParseNumExpr(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        Expression* ParseTerm(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        Expression* ParseUnary(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        Expression* ParseFactor(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        Location* ParseLocation(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end);
        
        
        // Helper function to advance the iterator
        inline void Next(std::vector<Token>::const_iterator& itr, const std::vector<Token>::const_iterator& end) {
            if (itr != end) {
                ++itr;
            } else {
                throw ParseException("Unexpected end of input.");
            }
        }
        // Helper function to generate error messages

        inline std::string GenError (std::vector<Token>::const_iterator& itr, const int tag) {
            std::stringstream err;
            err << "Expected token '" << Token::id2word[tag] << "' at line " << itr->pos.first << ", column " << itr->pos.second << std::endl
                << "Found token '" << Token::id2word[itr->tag] << "'";
            return err.str();
        }

};

#endif 