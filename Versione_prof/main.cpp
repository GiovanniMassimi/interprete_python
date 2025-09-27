
#include <iostream>
#include <fstream>
#include <vector>

#include "Lexer.h"
#include "Parser.h"
#include "Token.h"
#include "Syntax.h"
#include "Exception.h"
//#include "PrintVisitor.h"
#include "EvaluationVisitor.h"

#define DEBUG_ON 0 // 0 no debug, 1 debug on

int main(int argc, char* argv[]) {
    if (argc < 2) { // Check for input file argument
        std::cerr << "Using: " << argv[0] << " <file.txt>\n";
        return 1;
    }

    std::ifstream input(argv[1]); // Open the input file
    if (!input.is_open()) {
        std::cerr << "Error: (file) failed to open file: " << argv[1] << "\n";
        return 1;
    }

    Lexer lexer; // Create a lexer instance
    std::vector<Token> tokens; // Vector to hold the tokens

    try {
        lexer.tokenizeFile(input, tokens); // Tokenize the input file
        if (DEBUG_ON) std::cout <<"Tokenizing completed successfully. Number of tokens: " << tokens.size() << "\n";
    } catch (const LexicalError& err) { // Catch lexical errors
        std::cerr << "Error: (lexing) " << err.what() << "\n";
        return 1;
    } catch (const std::exception& err) { // Catch other exceptions
        std::cerr << "Error: (generic) " << err.what() << "\n";
        return 1;
    }

    try {
        Parser parser; // Create a parser instance
        Program* ast = parser.parse(tokens); // Parse the tokens into an AST

        if (DEBUG_ON) std::cout << "Parsing completed successfully.\n";

        // Debug for ast
        if (DEBUG_ON) {
            std::cout << "\n----- AST ----\n";
            //PrintVisitor printer(std::cout);
            //ast->accept(printer);
        }
        

        // Evaluation
        if (DEBUG_ON) std::cout << "\n----- Program Execution ----\n";
        SymbolTable table; // Create a symbol table instance
        EvaluationVisitor evaluator(table, std::cout); // Create an evaluation visitor
        ast->accept(evaluator); // Evaluate the AST

        delete ast;
    } catch (const ParseError& parse_err) { // Catch parsing errors
        std::cerr << "Error: (parsing) " << parse_err.what() << "\n";
        return 1;
    } catch (const EvaluationError& eval_err) { // Catch evaluation errors
        std::cerr << "Error: (evaluation) " << eval_err.what() << "\n";
        return 1;
    } catch (const TypeError& type_err) { // Catch type errors
        std::cerr << "Error: (type) " << type_err.what() << "\n";
        return 1;
    } catch (const std::exception& err) { // Catch other exceptions
        std::cerr << "Error: (generic) " << err.what() << "\n";
        return 1;
    }

    return 0;
}