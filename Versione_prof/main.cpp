/*
g++ -std=c++20 -Iinclude \
    src/main.cpp src/Lexer.cpp src/Parser.cpp src/Syntax.cpp src/EvaluationVisitor.cpp \
    -o parser_test

./parser_test PASS_SumVec.txt
*/


#include <iostream>
#include <fstream>
#include <vector>

#include "Lexer.h"
#include "Parser.h"
#include "Token.h"
#include "Syntax.h"
#include "Exception.h"
#include "PrintVisitor.h"
#include "EvaluationVisitor.h"

#define DEBUG_ON 0

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Using: " << argv[0] << " <file.txt>\n";
        return 1;
    }

    std::ifstream input(argv[1]);
    if (!input.is_open()) {
        std::cerr << "Error opening file: " << argv[1] << "\n";
        return 1;
    }

    Lexer lexer;
    std::vector<Token> tokens;

    try {
        lexer.tokenizeFile(input, tokens);
        if (DEBUG_ON) std::cout <<"Tokenizing completed successfully. Number of tokens: " << tokens.size() << "\n";
    } catch (const LexicalError& ex) {
        std::cerr << "Errore nel lexer: " << ex.what() << "\n";
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "Generic error in lexer: " << ex.what() << "\n";
        return 1;
    }

    try {
        Parser parser;
        Program* ast = parser.parse(tokens);

        if (DEBUG_ON) std::cout << "Parsing completed successfully.\n";

        // --- AST Debug (opzionale)
        if (DEBUG_ON) {
            std::cout << "\n--- AST ---\n";
            PrintVisitor printer(std::cout);
            ast->accept(printer);
        }
        

        // --- Valutazione
        if (DEBUG_ON) std::cout << "\n--- Program Execution ---\n";
        SymbolTable table;
        EvaluationVisitor evaluator(table, std::cout);
        ast->accept(evaluator);

        delete ast;
    } catch (const ParseError& pex) {
        std::cerr << "Parsing Error: " << pex.what() << "\n";
        return 1;
    } catch (const EvaluationError& eex) {
        std::cerr << "Evaluation Error: " << eex.what() << "\n";
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "Generic Error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}