#if !defined(VISITOR_H)
#define VISITOR_H

#include "Syntax.h"

class Visitor {
    public:
        virtual void visit(const Program& program) = 0;
        virtual void visit(const Statement& statement) = 0;
        virtual void visit(const Expression& expression) = 0;

        virtual void visit(const Assignment& assignment) = 0;
        virtual void visit(const ListAssignment& listAssignment) = 0;
        virtual void visit(const ListCreation& listCreation) = 0;
        virtual void visit(const Append& append) = 0;
        virtual void visit(const Break& breakStatement) = 0;
        virtual void visit(const Continue& continueStatement) = 0;
        virtual void visit(const Print& printStatement) = 0;

        virtual void visit(const WhileStatement& whileStatement) = 0;
        virtual void visit(const IfStatement& ifStatement) = 0;

        virtual void visit(const ReturnStatement& returnStatement) = 0;
        virtual void visit(const Join& join) = 0;
        virtual void visit(const Equality& equality) = 0;
        virtual void visit(const Relational& relational) = 0;
        virtual void visit(const NumExpr& numExpr) = 0;
        virtual void visit(const Term& term) = 0;
        virtual void visit(const Unary& unary) = 0;
        virtual void visit(const Factor& factor) = 0;
        virtual void visit(const Location& location) = 0; //non lo uso

       virtual ~Visitor() = default;

};


#endif
