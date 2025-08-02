#include <iostream>
#include <fstream>
#include <vector>
#include "Lexer.h"

int main() {
    std::ifstream input("PASS_SumVec.txt");
    if (!input.is_open()) {
        std::cerr << "Errore nell'apertura del file.\n";
        return 1;
    }

    Lexer lexer;
    std::vector<Token> tokens;

    try {
    lexer.tokenizeFile(input, tokens);
    for (const auto& token : tokens) {
        std::cout << token << "\n";  // usa l'overload <<
    }
    } catch (const std::exception& e) {
    std::cerr << "Errore durante la tokenizzazione: " << e.what() << "\n";
    return 1;
    }

    return 0;
}