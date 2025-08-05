#if !defined(PRINT_VISITOR_H)
#define PRINT_VISITOR_H


#include <iostream>

#include "Visitor.h"
#include "Syntax.h"

class PrintVisitor : public Visitor {
    int indent = 0;

    void printIndent() const {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
    }

public:
//TODO: complete definition of constructors, destructors, etc.
	PrintVisitor(std::ostream& con) : console_{ con } { }

    void visit(const Program& node) override {
        std::cout << "Program:\n";
        indent++;
        for (const auto stmt : node.statements) {
            if (stmt) stmt->accept(*this);
        }
        indent--;
    }

    void visit(const Assignment& node) override {
        printIndent();
        std::cout << "Assignment: " << node.id.word << "\n";
        indent++;
        if (node.value) node.value->accept(*this);
        indent--;
    }

    void visit(const Print& node) override {
        printIndent();
        std::cout << "Print:\n";
        indent++;
        if (node.expr) node.expr->accept(*this);
        indent--;
    }

    void visit(const Literal& node) override {
        printIndent();
        std::cout << "Literal: " << node.token.word << "\n";
    }

    void visit(const Location& node) override {
        printIndent();
        std::cout << "Location: " << node.id.word;
        if (node.index) {
            std::cout << "[\n";
            indent++;
            node.index->accept(*this);
            indent--;
            printIndent();
            std::cout << "]\n";
        } else {
            std::cout << "\n";
        }
    }

    void visit(const BinaryExpression& node) override {
        printIndent();
        std::cout << "BinaryExpression: " << Token::id2word[node.op.tag] << "\n";
        indent++;
        node.left->accept(*this);
        node.right->accept(*this);
        indent--;
    }

    // Aggiungi altri metodi visit(...) per ogni tipo AST che hai definito
};

#endif