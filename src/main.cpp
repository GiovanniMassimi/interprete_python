#include <iostream>
#include "Token.h"

int main() {
    // Crea alcuni token manualmente
    Token t1(Token::ID, "x");
    Token t2(Token::EQ, "=");
    Token t3(Token::CONST, "42");
    Token t4(Token::ADD, "+");
    Token t5(Token::CONST, "8");

    // Stampali a schermo
    std::cout << t1 << "\n";
    std::cout << t2 << "\n";
    std::cout << t3 << "\n";
    std::cout << t4 << "\n";
    std::cout << t5 << "\n";

    return 0;
}