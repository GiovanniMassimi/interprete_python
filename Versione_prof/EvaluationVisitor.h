#ifndef EVALUATION_VISITOR_H
#define EVALUATION_VISITOR_H

#include <iostream>

#include "Visitor.h"
#include "SymbolTable.h"
#include "Value.h" 

class EvaluationVisitor : public Visitor {
    SymbolTable& symbolTable;
    std::ostream& console_;

public:
    EvaluationVisitor(SymbolTable& table, std::ostream& con)
        : symbolTable{table}, console_{con}, result{0} {}

    Value result; 

    // Dichiarazioni dei visit
    void visit(const Program& node) override;
    void visit(const Statement& node) override;
    void visit(const Expression& node) override;
    void visit(const Assignment& node) override;
    void visit(const ListAssignment& node) override;
    void visit(const ListCreation& node) override;
    void visit(const Append& node) override;
    void visit(const Print& node) override;
    void visit(const Block& node) override;
    void visit(const Break& node) override;
    void visit(const Continue& node) override;
    void visit(const WhileStatement& node) override;
    void visit(const IfStatement& node) override;
    void visit(const ElifBlock& node) override;
    void visit(const ElseBlock& node) override;

    void visit(const Join& node) override;
    void visit(const Equality& node) override;
    void visit(const Relation& node) override;
    void visit(const NumExpr& node) override;
    void visit(const Term& node) override;
    void visit(const Unary& node) override;
    void visit(const Factor& node) override;
    void visit(const Literal& node) override;
    void visit(const Location& node) override;
};

#endif 