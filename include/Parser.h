#if !defined(PARSER_H)
#define PARSER_H

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
        Program* Operator(const std::vector<Token>& tokens) {
            auto itr = tokens.begin();
            auto end = tokens.end();
            
            Program* program = ParseProgram(itr);
            return program;
        }

    private:


        Program* ParseProgram(std::vector<Token>::const_iterator& itr);
        Statement* ParseStatement(std::vector<Token>::const_iterator& itr);
        Expression* ParseExpression(std::vector<Token>::const_iterator& itr);

        Assignment* ParseAssignment(std::vector<Token>::const_iterator& itr);
        ListAssignment* ParseListAssignment(std::vector<Token>::const_iterator& itr);
        ListCreation* ParseListCreation(std::vector<Token>::const_iterator& itr);
        Append* ParseAppend(std::vector<Token>::const_iterator& itr);
        Break* ParseBreak(std::vector<Token>::const_iterator& itr);
        Continue* ParseContinue(std::vector<Token>::const_iterator& itr);
        Print* ParsePrint(std::vector<Token>::const_iterator& itr);

        Block* ParseBlock(std::vector<Token>::const_iterator& itr);
        IfStatement* ParseIfStatement(std::vector<Token>::const_iterator& itr);
        WhileStatement* ParseWhileStatement(std::vector<Token>::const_iterator& itr);

        ReturnStatement* ParseReturnStatement(std::vector<Token>::const_iterator& itr);
        Join* ParseJoin(std::vector<Token>::const_iterator& itr);
        Equality* ParseEquality(std::vector<Token>::const_iterator& itr);
        Relational* ParseRelationas(std::vector<Token>::const_iterator& itr);
        NumExpr* ParseNumExpr(std::vector<Token>::const_iterator& itr);
        Term* ParseTerm(std::vector<Token>::const_iterator& itr);
        Unary* ParseUnary(std::vector<Token>::const_iterator& itr);
        Factor* ParseFactor(std::vector<Token>::const_iterator& itr);
        Location* ParseLocation(std::vector<Token>::const_iterator& itr);

        auto end = tokens.end();
        inline void Next (std::vector<Token>::const_iterator& itr) {
            if (itr == end) {
                throw ParseException("Unexpected end of input");
            }
            ++itr;
        }

        inline std::stringstream GenError (std::vector<Token>::const_iterator& itr, const Token& tag) {
            std::stringstream err;
            err << "Expected token '" << Token::id2word[tag.tag] << "' at line " << tag.pos.line << ", column " << tag.pos.column<< std::endl
                << "Found token '" << Token::id2word[itr->tag] << "'";
            return err;
        }

};

#endif 