#include "Syntax.h"
#include "Visitor.h"

void Program::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Statement::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Expression::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Assignment::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void ListAssignment::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void ListCreation::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Append::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Break::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Continue::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Print::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Block::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void WhileStatement::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void IfStatement::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void ElifBlock::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void ElseBlock::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Join::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Equality::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Relation::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void NumExpr::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Term::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Unary::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Factor::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Location::accept(Visitor& visitor) const {
    visitor.visit(*this);
}

void Literal::accept(Visitor& visitor) const {
    visitor.visit(*this);
}
