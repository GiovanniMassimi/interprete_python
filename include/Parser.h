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

        IfStatement* ParseIfStatement(std::vector<Token>::const_iterator& itr);
        WhileStatement* ParseWhileStatement(std::vector<Token>::const_iterator& itr);

        ReturnStatement* ParseReturnStatement(std::vector<Token>::const_iterator& itr);

        auto end = tokens.end();
        void Next (std::vector<Token>::const_iterator& itr) {
            if (itr == end) {
                throw ParseException("Unexpected end of input");
            }
            ++itr;
        }
};

#endif 