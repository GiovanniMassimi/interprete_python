#if !defined(TOKEN_H)
#define TOKEN_H

#include <string>
#include <utility>
#include <iostream>

struct Token {
    static constexpr int LP     =  0; // (
    static constexpr int RP     =  1; // )
    static constexpr int EQ     =  2; // =
    static constexpr int ADD    =  3; // +
    static constexpr int SUB    =  4; // -
    static constexpr int MUL    =  5; // *
    static constexpr int DIV    =  6; // / don't need it but I dont want to move index
    static constexpr int DIVINT =  7; // //
    static constexpr int MOD    =  8; // %
    static constexpr int PRINT  =  9;
    static constexpr int NUM    = 10; // integer number
    static constexpr int NEWLINE= 11;
    static constexpr int INDENT = 12;
    static constexpr int DEDENT = 13;
    static constexpr int SEMCOL = 14; // ; found in the test vectors but not used
    static constexpr int DOT    = 15; // .
    static constexpr int COLON  = 16; // :
    static constexpr int ID     = 17; // Identificators
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
    // Token string representations
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

    // Constructor and destructor
    Token(int t, const std::string& w, std::pair<int, int> pos) : tag{ t }, word{ w }, pos{ pos } {}
    ~Token() = default;
    Token(const Token&) = default;
    Token& operator=(const Token&) = default;

    int tag; // token type
    std::string word; // token value (for ID and NUM)
    std::pair<int, int> pos; // (line, column) position in the source code for error reporting

};

 // Overload the output stream operator for easy printing of tokens
inline std::ostream& operator<<(std::ostream& os, const Token& t) {
    const char* tagName = (t.tag >= 0 && t.tag < 40) ? Token::tag2string[t.tag] : "UNKNOWN";
    os << "Token(" << tagName << ", \"" << t.word << "\", (" << t.pos.first << ", " << t.pos.second << "))";
    return os;
}

#endif