

#include "EvaluationVisitor.h"
#include "Syntax.h" 
#include "Exception.h"

void EvaluationVisitor::visit(const Program& node) {
    for (const auto& stmt : node.stmts) {
        if (stmt) stmt->accept(*this);
    }
}

void EvaluationVisitor::visit(const Statement& node) {
   //dovrei togliere accept da statement
    
}

void EvaluationVisitor::visit(const Expression& node) {
    //dovrei togliere accept da expression
    
}

void EvaluationVisitor::visit(const Assignment& node) {
    if (node.value) {
        node.value->accept(*this);
        symbolTable.setValue(node.id.word, result);
        console_ << "[DEBUG] Assigning value to: " << node.id.word << " = " << result.toString() << std::endl;

    } else {
        throw EvaluationError("Assignment value is missing");
    }
}

void EvaluationVisitor::visit(const ListAssignment& node) {
    throw EvaluationError("ListAssignment evaluation not implemented yet");
}

void EvaluationVisitor::visit(const ListCreation& node) {
    console_ << "[DEBUG] Creating list " << node.id.word << std::endl;
    symbolTable.setValue(node.id.word, Value(std::vector<Value>{}));
    symbolTable.dump(console_);
}

void EvaluationVisitor::visit(const Append& node) {
    // Ottieni riferimento modificabile
    Value& container = symbolTable.getValuemod(node.id.word);
    auto& list = container.asList();

    node.expr->accept(*this);
    console_ << "[DEBUG] About to append this evaluated expr: " << result.toString() << std::endl;

    list.push_back(result);

    console_ << "[DEBUG] Appended to list " << node.id.word << ": " << result.toString() << std::endl;
    symbolTable.dump(console_);
}

void EvaluationVisitor::visit(const Break& node) {
    throw BreakException{};
}

void EvaluationVisitor::visit(const Continue& node) {
    throw ContinueException{};
}

void EvaluationVisitor::visit(const Print& node) {
    node.expr->accept(*this);
    console_ << result.toString() << std::endl;
}

void EvaluationVisitor::visit(const Block& node) {
    for (auto stmt : node.stmts) {
        stmt->accept(*this);
    }
}

void EvaluationVisitor::visit(const WhileStatement& node) {
    try {
        node.condition->accept(*this);
        while (result.asBool()) {
            try {
                node.block->accept(*this);
            } catch (const ContinueException&) {
                // Salta all'inizio del ciclo
            }
            node.condition->accept(*this);
        }
    } catch (const BreakException&) {
        // Esci dal ciclo

    }
}

void EvaluationVisitor::visit(const IfStatement& node) {
    
    node.condition->accept(*this);
    if (result.asBool()) {
        node.block->accept(*this);
        return;
    }
    for (auto elif : node.elif_blocks) {
        elif->accept(*this);
        if (result.asBool()) return;
    }
    if (node.else_block) {
        node.else_block->accept(*this);
    }
}

void EvaluationVisitor::visit(const ElifBlock& node) {
    node.condition->accept(*this);
    if (result.asBool()) {
        node.block->accept(*this);
    }
}

void EvaluationVisitor::visit(const ElseBlock& node) {
    node.block->accept(*this);
}

void EvaluationVisitor::visit(const Join& node) {
    node.left->accept(*this);
    bool leftVal = result.asBool();
    if (node.op.tag == Token::OR) {
        if (leftVal) {
            result = Value(true);
            return;
        }
        node.right->accept(*this);
        result = Value(result.asBool());
    } else { // AND
        if (!leftVal) {
            result = Value(false);
            return;
        }
        node.right->accept(*this);
        result = Value(result.asBool());
    }
}

void EvaluationVisitor::visit(const Equality& node) {
    node.left->accept(*this);
    Value leftVal = result;
    node.right->accept(*this);
    Value rightVal = result;

    console_ << "[DEBUG] Equality: " << leftVal.toString() 
             << " == " << rightVal.toString() << std::endl;
    std::cerr << "[DEBUG] Token EQ? Got tag: " << static_cast<int>(node.op.tag)
          << " Expected EQ: " << static_cast<int>(Token::EQ) << "\n";
    if (node.op.tag == Token::EQEQ) {
        bool resultBool = (leftVal == rightVal);
        std::cerr << "[DEBUG] Risultato confronto bool grezzo: " << resultBool << "\n";
        result = Value(leftVal == rightVal);
    } else {
        result = Value(!(leftVal == rightVal));

    console_ << "[DEBUG] Equality result: " << result.toString() << std::endl;
    }
}


void EvaluationVisitor::visit(const Relation& node) {
    node.left->accept(*this);
    int lhs = result.asInt();
    node.right->accept(*this);
    int rhs = result.asInt();

    switch (node.op.tag) {
        case Token::LT: result = Value(lhs < rhs); break;
        case Token::LE: result = Value(lhs <= rhs); break;
        case Token::GE: result = Value(lhs >= rhs); break;
        case Token::GT: result = Value(lhs > rhs); break;
        default:
            throw EvaluationError("Unknown relational operator");
    }
}

void EvaluationVisitor::visit(const NumExpr& node) {
    node.left->accept(*this);
    int lhs = result.asInt();
    node.right->accept(*this);
    int rhs = result.asInt();

    switch (node.op.tag) {
        case Token::ADD: result = Value(lhs + rhs); break;
        case Token::SUB: result = Value(lhs - rhs); break;
        default:
            throw EvaluationError("Unknown numeric operator");
    }
}

void EvaluationVisitor::visit(const Term& node) {
    node.left->accept(*this);
    int lhs = result.asInt();
    node.right->accept(*this);
    int rhs = result.asInt();

    switch (node.op.tag) {
        case Token::MUL: result = Value(lhs * rhs); break;
        case Token::DIVINT: 
            if (rhs == 0) throw EvaluationError("Division by zero");
            result = Value(lhs / rhs); 
            break;
        default:
            throw EvaluationError("Unknown term operator");
    }
}

void EvaluationVisitor::visit(const Unary& node) {
    node.expr->accept(*this);
    if (node.op.tag == Token::NOT) {
        result = Value(!result.asBool());
    } else if (node.op.tag == Token::SUB) {
        result = Value(-result.asInt());
    } else {
        throw EvaluationError("Unknown unary operator");
    }
}


void EvaluationVisitor::visit(const Factor& node) {
    if (node.expr) {
        node.expr->accept(*this); // caso: ( <expr> )
    } else {
        node.token.accept(*this); // caso: literal vero e proprio
    }
}



void EvaluationVisitor::visit(const Literal& node) {
    if (node.value.tag == Token::NUM) {
        result = Value(std::stoi(node.value.word));
    } else if (node.value.tag == Token::TRUE_) {
        result = Value(true);
    } else if (node.value.tag == Token::FALSE_) {
        result = Value(false);
    } else {
        throw EvaluationError("Unknown literal type");
    }
}

void EvaluationVisitor::visit(const Location& node) {
    Value container = symbolTable.getValue(node.id.word);
    if (node.index) {
        node.index->accept(*this);
        int index = result.asInt();
        auto& list = container.asList();
        console_ << "[DEBUG] Accessing list: " << node.id.word 
                 << " at index: " << index 
                 << ", list size: " << list.size() << std::endl;
        if (list.empty()) {
            throw EvaluationError("Attempted to access an empty list: '" + node.id.word + "'");
        }
        if (index < 0 || index >= static_cast<int>(list.size())) {
            throw EvaluationError("Index out of bounds");
        }
        result = list[index];
    } else {
        result = container;
    }
}
