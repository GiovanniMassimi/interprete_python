//g++ -std=c++20 -Iinclude \
  src/main.cpp src/Lexer.cpp src/Parser.cpp  src/Syntax.cpp \
  -o parser_test

#include <iostream>
#include <fstream>
#include <vector>
#include "Lexer.h"
#include "Parser.h"
#include "Token.h"
#include "Syntax.h"
#include "Exception.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <file.txt>\n";
        return 1;
    }

    std::ifstream input(argv[1]);
    if (!input.is_open()) {
        std::cerr << "Errore nell'aprire il file: " << argv[1] << "\n";
        return 1;
    }

    Lexer lexer;
    std::vector<Token> tokens;

    try {
        lexer.tokenizeFile(input, tokens);
        std::cout << "Tokenizzazione completata con successo. Numero di token: " << tokens.size() << "\n";
    } catch (const std::exception& ex) {
        std::cerr << "Errore nel lexer: " << ex.what() << "\n";
        return 1;
    }

    try {
        Parser parser;
        Program* ast = parser.parse(tokens);

        std::cout << "Parsing completato correttamente.\n";
        
        // Se hai un visitor o una funzione per stampare l'AST, lo puoi fare qui:
        // ast->accept(visitor);

        delete ast;
    } catch (const ParseException& pex) {
        std::cerr << "Errore di parsing: " << pex.what() << "\n";
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "Errore generico: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}


