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
    static constexpr int DIV    =  6; // //
    static constexpr int ID     =  7; // identifier
    static constexpr int KW     =  8; // keyword
    static constexpr int PRINT  =  9;
    static constexpr int CONST  = 10; // integer number
    static constexpr int UNKNOWN= 11;
    static constexpr int NEWLINE= 12;
    static constexpr int INDENT = 13;
    static constexpr int DEDENT = 14;
    static constexpr int LBRACK = 15; // [
    static constexpr int RBRACK = 16; // ]
    static constexpr int DOT    = 17; // .
    static constexpr int COLON  = 18; // :
    static constexpr int COMMA  = 19; // ,
    static constexpr int TRUE_  = 20;
    static constexpr int FALSE_ = 21;
    static constexpr int IF     = 22;
    static constexpr int ELIF   = 23;
    static constexpr int ELSE   = 24;
    static constexpr int WHILE  = 25;
    static constexpr int BREAK  = 26;
    static constexpr int CONTINUE = 27;
    static constexpr int AND    = 28;
    static constexpr int OR     = 29;
    static constexpr int NOT    = 30;
    static constexpr int EQEQ   = 31; // ==
    static constexpr int NOTEQ  = 32; // !=
    static constexpr int LT     = 33; // <
    static constexpr int LE     = 34; // <=
    static constexpr int GE     = 35; // >=
    static constexpr int GT     = 36; // >
    static constexpr int LIST   = 37;
    static constexpr int APPEND = 38;
    static constexpr int END    = 39; // EOF

    // Human-readable token words (for debug/printing)
    static constexpr const char* id2word[] = {
        "(", ")", "=", "+", "-", "*", "//", "ID", "KW", "print", "CONST", "UNKNOWN",
        "NEWLINE", "INDENT", "DEDENT", "[", "]", ".", ":", ",",
        "True", "False", "if", "elif", "else", "while", "break", "continue",
        "and", "or", "not", "==", "!=", "<", "<=", ">=", ">", "list", "append", "EOF"
    };

    // Symbolic token type names
    static constexpr const char* tag2string[] = {
        "LP", "RP", "EQ", "ADD", "SUB", "MUL", "DIV", "ID", "KW", "PRINT", "CONST", "UNKNOWN",
        "NEWLINE", "INDENT", "DEDENT", "LBRACK", "RBRACK", "DOT", "COLON", "COMMA",
        "TRUE", "FALSE", "IF", "ELIF", "ELSE", "WHILE", "BREAK", "CONTINUE",
        "AND", "OR", "NOT", "EQEQ", "NOTEQ", "LT", "LE", "GE", "GT", "LIST", "APPEND", "END"
    };

    Token(int t, const std::string& w) : tag{ t }, word{ w } {}
    ~Token() = default;
    Token(const Token&) = default;
    Token& operator=(const Token&) = default;

    int tag;           
    std::string word;  
};

// Output stream overload per stampare token leggibili
inline std::ostream& operator<<(std::ostream& os, const Token& t){
    os << "(" << Token::tag2string[t.tag] << ",\"" << t.word << "\")";
	return os;
};
#endif