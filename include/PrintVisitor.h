#if !defined(PRINT_VISITOR_H)
#define PRINT_VISITOR_H


#include <iostream>

#include "Visitor.h"
#include "Syntax.h"

class PrintVisitor : public Visitor {
    public:
    PrintVisitor(std::ostream& con) : console_{ con } { }
    int indent = 0;

    void printIndent() const {
        for (int i = 0; i < indent; ++i) console_ << "  ";
    }

    void visit(const Program& node) override {
        printIndent();
        console_ << "Program:\n";
        indent++;
        //std::vector<Statement*> stmts;
        for (const auto stmt : node.stmts) {
            if (stmt) stmt->accept(*this);
        }
        indent--;
    }

    void visit(const Statement& node) override {
        printIndent();
        console_ << "Statement\n";
    }

    void visit(const Expression& node) override {
        printIndent();
        console_ << "Expression\n";
    }

    void visit(const Assignment& node) override {
        printIndent();
        console_ << "Assignment: " << node.id.word << "\n";
        indent++;
        if (node.value) node.value->accept(*this);
        indent--;
    }

    void visit(const ListAssignment& node) override {
        printIndent();
        console_ << "List Assignment: " << node.id.word << "\n";
        indent++;
        if (node.pos) node.pos->accept(*this);
        if (node.value) node.value->accept(*this);
        indent--;
    }

    void visit(const ListCreation& node) override {
        printIndent();
        console_ << "List Creation: " << node.id.word << "\n";
    }

    void visit(const Append& node) override {
        printIndent();
        console_ << "Append to List: " << node.id.word << "\n";
        indent++;
        if (node.expr) node.expr->accept(*this);
        indent--;
    }

    void visit(const Break& node) override {
        printIndent();
        console_ << "Break at line: " << node.line_number << "\n";
    }

    void visit(const Continue& node) override {
        printIndent();
        console_ << "Continue at line: " << node.line_number << "\n";
    }

    void visit(const Print& node) override {
        printIndent();
        console_ << "Print:\n";
        indent++;
        if (node.expr) node.expr->accept(*this);
        indent--;
    }

    void visit(const Block& node) override {
        printIndent();
        console_ << "Block:\n";
        indent++;
        for (const auto stmt : node.stmts) {
            if (stmt) stmt->accept(*this);
        }
        indent--;
    }

    void visit(const WhileStatement& node) override {
        printIndent();
        console_ << "While Statement:\n";
        indent++;
        if (node.condition) node.condition->accept(*this);
        if (node.block) node.block->accept(*this);
        indent--;
    }

    void visit(const ElifBlock& node) override {
        printIndent();
        console_ << "Elif Block:\n";
        indent++;
        if (node.condition) node.condition->accept(*this);
        if (node.block) node.block->accept(*this);
        indent--;
    }

    void visit(const ElseBlock& node) override {
        printIndent();
        console_ << "Else Block:\n";
        indent++;
        if (node.block) node.block->accept(*this);
        indent--;
    }

    void visit(const IfStatement& node) override {
        printIndent();
        console_ << "If Statement:\n";
        indent++;
        if (node.condition) node.condition->accept(*this);
        if (node.block) node.block->accept(*this);
        for (const auto& elif : node.elif_blocks) {
            elif->accept(*this);
        }
        if (node.else_block) node.else_block->accept(*this);
        indent--;
    }

    void visit(const Join& node) override {
        printIndent();
        console_ << "Join Expression:\n";
        indent++;
        if (node.left) node.left->accept(*this);
        printIndent();
        console_ << "Operator: " << node.op.word << "\n";
        if (node.right) node.right->accept(*this);
        indent--;
    }

    void visit(const Equality& node) override {
        printIndent();
        console_ << "Equality Expression:\n";
        indent++;
        if (node.left) node.left->accept(*this);
        printIndent();
        console_ << "Operator: " << node.op.word << "\n";
        if (node.right) node.right->accept(*this);
        indent--;
    }

    void visit(const Relation& node) override {
        printIndent();
        console_ << "Relation Expression:\n";
        indent++;
        if (node.left) node.left->accept(*this);
        printIndent();
        console_ << "Operator: " << node.op.word << "\n";
        if (node.right) node.right->accept(*this);
        indent--;
    }

    void visit(const NumExpr& node) override {
        printIndent();
        console_ << "Num Expression:\n";
        indent++;
        if (node.left) node.left->accept(*this);
        printIndent();
        console_ << "Operator: " << node.op.word << "\n";
        if (node.right) node.right->accept(*this);
        indent--;
    }

    void visit(const Term& node) override {
        printIndent();
        console_ << "Term Expression:\n";
        indent++;
        if (node.left) node.left->accept(*this);
        printIndent();
        console_ << "Operator: " << node.op.word << "\n";
        if (node.right) node.right->accept(*this);
        indent--;
    }

    void visit(const Unary& node) override {
        printIndent();
        console_ << "Unary Expression: " << node.op.word << "\n";
        indent++;
        if (node.expr) node.expr->accept(*this);
        indent--;
    }

    void visit(const Factor& node) override {
    printIndent();
    console_ << "Factor:\n";
    indent++;

    // Caso 1: expr presente → stampa ricorsiva
    if (node.expr) {
        node.expr->accept(*this);
    }
    // Caso 2: token non "vuoto" (e.g., Literal presente)
    else if (node.token.value.tag == Token::NUM || node.token.value.tag == Token::TRUE_ || node.token.value.tag == Token::FALSE_) {  // o un altro check sensato
        indent++;
        printIndent();
        console_ << "Literal: " << node.token.value.word << "\n";
        indent--;
    }
    // Caso 3: errore di parsing o factor incompleto
    else {
        printIndent();
        console_ << "[!] Factor vuoto o malformato\n";
    }

    indent--;
}

    void visit(const Literal& node) override {
        printIndent();
        console_ << "Literal: " << node.value.word << "\n";
    }

    void visit(const Location& node) override {
        printIndent();
        console_ << "Location: " << node.id.word;
        if (node.index) {
            console_ << "[\n";
            indent++;
            node.index->accept(*this);
            indent--;
            printIndent();
            console_ << "]\n";
        } else {
            console_ << "\n";
        }
    }




    private:
    std::ostream& console_;
};

#endif 