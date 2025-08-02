#if !defined(LEXER_H)
#define LEXER_H

#include <vector>
#include <fstream>

#include "Token.h"
#include "Exception.h"

class Lexer {
    public:
        Lexer() = default;
        ~Lexer() = default;
        Lexer(Lexer const&) = delete;
        Lexer& operator=(Lexer const&) = delete;

        std::vector<Token> operator()(std::ifstream& input);
    //private:
        void tokenizeFile(std::ifstream& input, std::vector<Token>& tokens);
};

#endif // LEXER_H