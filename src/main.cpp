#include <iostream>
#include "Syntax.h"
#include "Token.h"
#include "Visitor.h"

// Visitor che stampa cosa viene visitato
struct TestVisitor : Visitor {
    void visit(Assignment& stmt) override {
        std::cout << "Visitato Assignment: " << stmt.id.lexeme << "\n";
    }

    void visit(Program& prog) override {
        std::cout << "Visitato Program\n";
        for (auto stmt : prog.stmts)
            stmt->accept(*this);
    }

    // Definisci gli altri visit(...) come vuoti se non ti servono ora
    void visit(Print&) override {}
    void visit(IfStatment&) override {}
    void visit(WhileStatment&) override {}
    void visit(Break&) override {}
    void visit(Continue&) override {}
    void visit(Append&) override {}
    void visit(ListCreation&) override {}
    void visit(Expression&) override {}
    void visit(Statment&) override {}
};

int main() {
    // Crea token dummy per il test
    Token id(TokenType::IDENTIFIER, "x");
    Token eq(TokenType::EQUAL, "=");
    Token value(TokenType::NUMBER, "5");

    // Costruisci un Assignment manualmente: x = 5
    Assignment* assign = new Assignment(id, eq, value);

    // Inseriscilo in un Program
    Program prog;
    prog.stmts.push_back(assign);

    // Applica il visitor
    TestVisitor visitor;
    prog.accept(visitor);

    delete assign; // pulizia
    return 0;
}