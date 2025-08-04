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
            tokens.emplace_back(Token{Token::NEWLINE, Token::id2word[Token::NEWLINE], {line, column}});
            line++;
            column = 0;

            // Calcola l'indentazione della riga successiva
            int spaces = 0;
            while (true) {
                char next = input.peek();
                if (next == ' ') {
                    input.get();
                    spaces++;
                    
                } else if (next == '\t') {
                    input.get();
                    spaces += 4; // tab = 4 spazi
                    column += 4;
                } else if (next == '\n') {
                    // Riga vuota, aggiungi NEWLINE e continua
                    input.get();
                    //tokens.emplace_back(Token{Token::NEWLINE, Token::id2word[Token::NEWLINE], {line, column}});
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
                tokens.emplace_back(Token{Token::INDENT, Token::id2word[Token::INDENT], {line, column}});
            } else {
                while (spaces < indentStack.back()) {
                    indentStack.pop_back();
                    tokens.emplace_back(Token{Token::DEDENT, Token::id2word[Token::DEDENT], {line, column}});
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
            tokens.emplace_back(Token{Token::LP, Token::id2word[Token::LP], {line, column}});
            column++;
        } else if (ch == ')') {
            tokens.emplace_back(Token{Token::RP, Token::id2word[Token::RP], {line, column}});
            column++;
        } else if (ch == '=') {
            if (input.peek() == '=') { //leggo il prossimo carattere per verificare se è un ==
                input.get();
                tokens.emplace_back(Token{Token::EQEQ, Token::id2word[Token::EQEQ], {line, column}});
                column += 2; // Incremento di 2 per il carattere '=' aggiuntivo
            } else {
                tokens.emplace_back(Token{Token::EQ, Token::id2word[Token::EQ], {line, column}});
                column++;
            }
        } else if (ch == '+') {
            tokens.emplace_back(Token{Token::ADD, Token::id2word[Token::ADD], {line, column}});
            column++;
        } else if (ch == '-') {
            tokens.emplace_back(Token{Token::SUB, Token::id2word[Token::SUB], {line, column}});
            column++;
        } else if (ch == '*') {
            tokens.emplace_back(Token{Token::MUL, Token::id2word[Token::MUL], {line, column}});
            column++;
        } else if (ch == '/') {
            if (input.peek() == '/') { // verifico div intera
                input.get();
                tokens.emplace_back(Token{Token::DIVINT, Token::id2word[Token::DIVINT], {line, column}});
                column += 2; // Incremento di 2 per il carattere '/' aggiuntivo
            } else {
                tokens.emplace_back(Token{Token::DIV, Token::id2word[Token::DIV], {line, column}});
                column++;
            }
        } else if (ch == '%') {
            tokens.emplace_back(Token{Token::MOD, Token::id2word[Token::MOD], {line, column}});
            column++;
        }  else if (ch == ';') {
            tokens.emplace_back(Token{Token::SEMCOL, Token::id2word[Token::SEMCOL], {line, column}});
            column++;
        } else if (ch == '.') {
            tokens.emplace_back(Token{Token::DOT, Token::id2word[Token::DOT], {line, column}});
            column++;
        } else if (ch == ':') {
            tokens.emplace_back(Token{Token::COLON, Token::id2word[Token::COLON], {line, column}});
            column++;
        } else if (ch == '[') {
            tokens.emplace_back(Token{Token::LB, Token::id2word[Token::LB], {line, column}});
            column++;
        } else if (ch == ']') {
            tokens.emplace_back(Token{Token::RB, Token::id2word[Token::RB], {line, column}});
            column++;
        } else if (ch == '<') {
            if (input.peek() == '=') { // verifico <=
                input.get();
                tokens.emplace_back(Token{Token::LE, Token::id2word[Token::LE], {line, column}});
                column += 2; // Incremento di 2 per il carattere '=' aggiuntivo
            } else {
                tokens.emplace_back(Token{Token::LT, Token::id2word[Token::LT], {line, column}});
                column++;
            }
        } else if (ch == '>') {
            if (input.peek() == '=') { // verifico >=
                input.get();
                tokens.emplace_back(Token{Token::GE, Token::id2word[Token::GE], {line, column}});
                column += 2; // Incremento di 2 per il carattere '=' aggiuntivo
            } else {
                tokens.emplace_back(Token{Token::GT, Token::id2word[Token::GT], {line, column}});
                column++;
            }
        } else if (ch == '!') {
            if (input.peek() == '=') { // verifico !=
                input.get();
                tokens.emplace_back(Token{Token::NOTEQ, Token::id2word[Token::NOTEQ], {line, column}});
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
                tokens.emplace_back(Token{Token::PRINT, Token::id2word[Token::PRINT], {line, column}});
            } else if (temp == "True") {
                tokens.emplace_back(Token{Token::TRUE_, Token::id2word[Token::TRUE_], {line, column}});
            } else if (temp == "False") {
                tokens.emplace_back(Token{Token::FALSE_, Token::id2word[Token::FALSE_], {line, column}});
            } else if (temp == "if") {
                tokens.emplace_back(Token{Token::IF, Token::id2word[Token::IF], {line, column}});
            } else if (temp == "elif") {
                tokens.emplace_back(Token{Token::ELIF, Token::id2word[Token::ELIF], {line, column}});
            } else if (temp == "else") {
                tokens.emplace_back(Token{Token::ELSE, Token::id2word[Token::ELSE], {line, column}});
            } else if (temp == "while") {
                tokens.emplace_back(Token{Token::WHILE, Token::id2word[Token::WHILE], {line, column}});
            } else if (temp == "break") {
                tokens.emplace_back(Token{Token::BREAK, Token::id2word[Token::BREAK], {line, column}});
            } else if (temp == "continue") {
                tokens.emplace_back(Token{Token::CONTINUE, Token::id2word[Token::CONTINUE], {line, column}});
            } else if (temp == "and") {
                tokens.emplace_back(Token{Token::AND, Token::id2word[Token::AND], {line, column}});
            } else if (temp == "or") {
                tokens.emplace_back(Token{Token::OR, Token::id2word[Token::OR], {line, column}});
            } else if (temp == "not") {
                tokens.emplace_back(Token{Token::NOT, Token::id2word[Token::NOT], {line, column}});
            } else if (temp == "list") {
                tokens.emplace_back(Token{Token::LIST, Token::id2word[Token::LIST], {line, column}});
            } else if (temp == "append") {
                tokens.emplace_back(Token{Token::APPEND, Token::id2word[Token::APPEND], {line, column}});
            } else {
                tokens.emplace_back(Token{Token::ID, temp, {line, column}});
            }
        } else if (std::isdigit(ch)) {
            std::string temp;
            temp += ch;
            while (std::isdigit(input.peek())) {
                temp += input.get();
                column++;
            }
            tokens.emplace_back(Token{Token::NUM, temp, {line, column}});
        } else{
            std:: stringstream err;
            err << "Error: Unrecognized character '" << ch << "' at line " << line << ", column " << column;
            throw LexicalError{err.str()};
        }
        ch = input.get();
    }
    tokens.emplace_back(Token{Token::END, Token::id2word[Token::END], {line, column}});
}

