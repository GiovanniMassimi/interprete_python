//g++ -std=c++20 -Iinclude \
  src/main.cpp src/Lexer.cpp src/Parser.cpp  src/Syntax.cpp \
  -o parser_test

//g++ -std=c++20 -Iinclude \
  src/main.cpp src/Lexer.cpp src/Parser.cpp src/Syntax.cpp \
  -o parser_test
// ./parser_test PASS_SumVec.txt

#include <iostream>
#include <fstream>
#include <vector>
#include "Lexer.h"
#include "Parser.h"
#include "Token.h"
#include "Syntax.h"
#include "Exception.h"
#include "PrintVisitor.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <file.txt>\n";
        return 1;
    }

    std::ifstream input(argv[1]);
    if (!input.is_open()) {
        std::cerr << "Error: failed to open file: " << argv[1] << "\n";
        return 1;
    }

    Lexer lexer;
    std::vector<Token> tokens;

    try {
        lexer.tokenizeFile(input, tokens);
        std::cout << "Tokenization completed successfully. Number of tokens: " << tokens.size() << "\n";
    } catch (const std::exception& ex) {
        std::cerr << "Error: failed to tokenize input: " << ex.what() << "\n";
        return 1;
    }

    try {
        
        Parser parser;
        Program* ast = parser.parse(tokens);

        std::cout << "Parsing completed successfully.\n";

        // Stampa l’AST
        PrintVisitor printer(std::cout);
        ast->accept(printer);

        delete ast;
    } catch (const ParseError& pex) {
        std::cerr << "Error: parsing error: " << pex.what() << "\n";
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "Error:  " << ex.what() << "\n";
        return 1;
    }

    return 0;
}


