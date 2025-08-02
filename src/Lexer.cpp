#include<sstream>


#include "Lexer.h"

void Lexer::tokenizeFile(std::ifstream& input, std::vector<Token>& tokens) {
    char ch{};
    unsigned int line = 1;
    unsigned int column = 0;
    std::vector<int> indentStack = {0}; // Stack per gestire l'indentazione
    ch = input.get();
    while ( !input.eof()) {
        if (ch == '\n') {
            tokens.emplace_back(Token::NEWLINE, Token::id2word[Token::NEWLINE]);
            line++;
            column = 0;

            // Calcola l'indentazione della riga successiva
            int spaces = 0;
            while (true) {
                char next = input.peek();
                if (next == ' ') {
                    input.get();
                    spaces++;
                    column++;
                } else if (next == '\t') {
                    input.get();
                    spaces += 4; // tab = 4 spazi
                    column += 4;
                } else if (next == '\n') {
                    // Riga vuota, aggiungi NEWLINE e continua
                    input.get();
                    tokens.emplace_back(Token::NEWLINE, Token::id2word[Token::NEWLINE]);
                    line++;
                    column = 0;
                    spaces = 0;
                } else {
                    break;
                }
            }
            
            // Gestione dell'indentazione
            if (spaces > indentStack.back()) {
                indentStack.push_back(spaces);
                tokens.emplace_back(Token::INDENT, Token::id2word[Token::INDENT]);
            } else {
                while (spaces < indentStack.back()) {
                    indentStack.pop_back();
                    tokens.emplace_back(Token::DEDENT, Token::id2word[Token::DEDENT]);
                }
                if (spaces != indentStack.back()) {
                    throw std::runtime_error("Error: Inconsistent indentation");
                }
            }

            ch = input.get();
            continue;
        }

        if (ch == ' ' || ch == '\t') {
            // Spazi non significativi (es. nel mezzo di un'espressione)
            column += (ch == ' ') ? 1 : 4;
            ch = input.get();
            continue;
        } else if (ch == '(') { //simboli 
            tokens.emplace_back(Token{Token::LP, Token::id2word[Token::LP]});
            column++;
        } else if (ch == ')') {
            tokens.emplace_back(Token::RP, Token::id2word[Token::RP]);
            column++;
        } else if (ch == '=') {
            if (input.peek() == '=') { //leggo il prossimo carattere per verificare se è un ==
                input.get(); 
                tokens.emplace_back(Token::EQEQ, Token::id2word[Token::EQEQ]);
                column += 2; // Incremento di 2 per il carattere '=' aggiuntivo
            } else {
                tokens.emplace_back(Token::EQ, Token::id2word[Token::EQ]);
                column++;
            }
        } else if (ch == '+') {
            tokens.emplace_back(Token::ADD, Token::id2word[Token::ADD]);
            column++;
        } else if (ch == '-') {
            tokens.emplace_back(Token::SUB, Token::id2word[Token::SUB]);
            column++;
        } else if (ch == '*') {
            tokens.emplace_back(Token::MUL, Token::id2word[Token::MUL]);
            column++;
        } else if (ch == '/') {
            if (input.peek() == '/') { // verifico div intera
                input.get();
                tokens.emplace_back(Token::DIVINT, Token::id2word[Token::DIVINT]);
                column += 2; // Incremento di 2 per il carattere '/' aggiuntivo
            } else {
                tokens.emplace_back(Token::DIV, Token::id2word[Token::DIV]);
                column++;
            }
        } else if (ch == '%') {
            tokens.emplace_back(Token::MOD, Token::id2word[Token::MOD]);
            column++;
        }  else if (ch == ';') {
            tokens.emplace_back(Token::SEMCOL, Token::id2word[Token::SEMCOL]);
            column++;
        } else if (ch == '.') {
            tokens.emplace_back(Token::DOT, Token::id2word[Token::DOT]);
            column++;
        } else if (ch == ':') {
            tokens.emplace_back(Token::COLON, Token::id2word[Token::COLON]);
            column++;
        } else if (ch == '[') {
            tokens.emplace_back(Token::LB, Token::id2word[Token::LB]);
            column++;
        } else if (ch == ']') {
            tokens.emplace_back(Token::RB, Token::id2word[Token::RB]);
            column++;
        } else if (ch == '<') {
            if (input.peek() == '=') { // verifico <=
                input.get();
                tokens.emplace_back(Token::LE, Token::id2word[Token::LE]);
                column += 2; // Incremento di 2 per il carattere '=' aggiuntivo
            } else {
                tokens.emplace_back(Token::LT, Token::id2word[Token::LT]);
                column++;
            }
        } else if (ch == '>') {
            if (input.peek() == '=') { // verifico >=
                input.get();
                tokens.emplace_back(Token::GE, Token::id2word[Token::GE]);
                column += 2; // Incremento di 2 per il carattere '=' aggiuntivo
            } else {
                tokens.emplace_back(Token::GT, Token::id2word[Token::GT]);
                column++;
            }
        } else if (ch == '!') {
            if (input.peek() == '=') { // verifico !=
                input.get();
                tokens.emplace_back(Token::NOTEQ, Token::id2word[Token::NOTEQ]);
                column += 2; // Incremento di 2 per il carattere '=' aggiuntivo
            } else {
                std::stringstream err;
                err << "Error: Unrecognized character '!' at line " << line << ", column" << column;
                throw LexicalError{err.str()};    
            }
        }else if (std::isalpha(ch)) { // identificatori e parole chiave
            std::string temp;
            temp += ch;
            while (std::isalnum(input.peek())) {
                temp += input.get();
                column++;
            }
            if (temp == "print") {
                tokens.emplace_back(Token::PRINT, Token::id2word[Token::PRINT]);
            } else if (temp == "True") {
                tokens.emplace_back(Token::TRUE_, Token::id2word[Token::TRUE_]);
            } else if (temp == "False") {
                tokens.emplace_back(Token::FALSE_, Token::id2word[Token::FALSE_]);
            } else if (temp == "if") {
                tokens.emplace_back(Token::IF, Token::id2word[Token::IF]);
            } else if (temp == "elif") {
                tokens.emplace_back(Token::ELIF, Token::id2word[Token::ELIF]);
            } else if (temp == "else") {
                tokens.emplace_back(Token::ELSE, Token::id2word[Token::ELSE]);
            } else if (temp == "while") {
                tokens.emplace_back(Token::WHILE, Token::id2word[Token::WHILE]);
            } else if (temp == "break") {
                tokens.emplace_back(Token::BREAK, Token::id2word[Token::BREAK]);
            } else if (temp == "continue") {
                tokens.emplace_back(Token::CONTINUE, Token::id2word[Token::CONTINUE]);
            } else if (temp == "and") {
                tokens.emplace_back(Token::AND, Token::id2word[Token::AND]);
            } else if (temp == "or") {
                tokens.emplace_back(Token::OR, Token::id2word[Token::OR]);
            } else if (temp == "not") {
                tokens.emplace_back(Token::NOT, Token::id2word[Token::NOT]);
            } else if (temp == "list") {
                tokens.emplace_back(Token::LIST, Token::id2word[Token::LIST]);
            } else if (temp == "append") {
                tokens.emplace_back(Token::APPEND, Token::id2word[Token::APPEND]);
            } else {
                tokens.emplace_back(Token{Token::ID, temp});
            }
        } else if (std::isdigit(ch)) {
            std::string temp;
            temp += ch;
            while (std::isdigit(input.peek())) {
                temp += input.get();
                column++;
            }
            tokens.emplace_back(Token{Token::NUM, temp});
        } else{
            std:: stringstream err;
            err << "Error: Unrecognized character '" << ch << "' at line " << line << ", column " << column;
            throw LexicalError{err.str()};
        }
        ch = input.get();
    }
    tokens.emplace_back(Token::END, Token::id2word[Token::END]);
}

