#if !defined(TOKEN_H)
#define TOKEN_H

/*
Parole chiave: if, else, while, list, print, continue, break, True, False
	•	Operatori e simboli: +, -, *, //, ==, !=, <, <=, >, >=, =, ., :, (, ), [, ]
	•	Identificatori: come x, myVar, counter (regex: [a-zA-Z][a-zA-Z0-9]*)
	•	Numeri interi: tipo 1, 42, 1000 (regex: [1-9][0-9]*)
	•	Indentazioni: INDENT, DEDENT, NEWLINE, EOF
*/
#include <string>

struct Token {
    static constexpr int LP     =  0; // (
    static constexpr int RP     =  1; // )
    static constexpr int EQ     =  2; // =
    static constexpr int ADD    =  3; // +
    static constexpr int SUB    =  4; // -
    static constexpr int MUL    =  5; // *
    static constexpr int DIV    =  6; // / non serve ma non voglio spostare gli indici
    static constexpr int DIVINT =  7; // //
    static constexpr int MOD    =  8; // %
    static constexpr int PRINT  =  9;
    static constexpr int NUM    = 10; // integer number
    static constexpr int NEWLINE= 11;
    static constexpr int INDENT = 12;
    static constexpr int DEDENT = 13;
    static constexpr int SEMCOL = 14; // ;
    static constexpr int DOT    = 15; // .
    static constexpr int COLON  = 16; // :
    static constexpr int ID     = 17; // Identificatori
    static constexpr int TRUE_  = 18;
    static constexpr int FALSE_ = 19;
    static constexpr int IF     = 20;
    static constexpr int ELIF   = 21;
    static constexpr int ELSE   = 22;
    static constexpr int WHILE  = 23;
    static constexpr int BREAK  = 24;
    static constexpr int CONTINUE = 25;
    static constexpr int AND    = 26;
    static constexpr int OR     = 27;
    static constexpr int NOT    = 28;
    static constexpr int EQEQ   = 29; // ==
    static constexpr int NOTEQ  = 30; // !=
    static constexpr int LT     = 31; // <
    static constexpr int LE     = 32; // <=
    static constexpr int GE     = 33; // >=
    static constexpr int GT     = 34; // >
    static constexpr int LIST   = 35;
    static constexpr int APPEND = 36;
    static constexpr int END    = 37; // EOF
    static constexpr int LB     = 38; // [
    static constexpr int RB     = 39; // ]

    static constexpr const char* id2word[] = {
        "(", ")", "=", "+", "-", "*", "/", "//", "%", "PRINT", "NUM",
        "NEWLINE", "INDENT", "DEDENT", ";", ".", ":", "ID", "TRUE", "FALSE", "IF", "ELIF", "ELSE", 
        "WHILE", "BREAK", "CONTINUE", "AND", "OR", "NOT", "==", "!=", "<", "<=", ">=", ">", "LIST", "APPEND", "END", "[", "]"
    };
    // Symbolic token type names
    static constexpr const char* tag2string[] = {
        "LP", "RP", "EQ", "ADD", "SUB", "MUL", "DIV", "DIVINT", "MOD", "PRINT", "NUM",
        "NEWLINE", "INDENT", "DEDENT", "SEMCOL", "DOT", "COLON", "ID", "TRUE_", "FALSE_", "IF", "ELIF", "ELSE",
        "WHILE", "BREAK", "CONTINUE", "AND", "OR", "NOT","EQEQ", "NOTEQ", "LT", "LE", "GE", "GT","LIST", "APPEND", "END", "LB", "RB"
    };

    Token(int t, const std::string& w) : tag{ t }, word{ w } {}
    ~Token() = default;
    Token(const Token&) = default;
    Token& operator=(const Token&) = default;

    int tag;           
    std::string word;  
};

// Output stream overload per stampare token leggibili
inline std::ostream& operator<<(std::ostream& os, const Token& t) {
    os << "Token(" << Token::tag2string[t.tag] << ", \"" << t.word << "\")";
    return os;
}

#endif