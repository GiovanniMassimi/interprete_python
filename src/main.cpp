#include <iostream>
#include <fstream>
#include <vector>
#include "Lexer.h"
#include "Token.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <file.txt>\n";
        return 1;
    }

    std::ifstream input(argv[1]);
    if (!input.is_open()) {
        std::cerr << "Errore nell'apertura del file: " << argv[1] << "\n";
        return 1;
    }

    Lexer lexer;
    std::vector<Token> tokens;

    try {
        lexer.tokenizeFile(input, tokens);  // o lexer.tokenize(input, tokens);
        for (const Token& token : tokens) {
            std::cout << token << "\n";  // richiede operator<< definito per Token
        }
    } catch (const std::exception& ex) {
        std::cerr << "Errore durante la tokenizzazione: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}