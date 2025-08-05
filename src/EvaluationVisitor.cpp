

#include "EvaluationVisitor.h"
#include "Syntax.h" 
#include "Exception.h"

#define DEBUG_ON 0

#if DEBUG_ON
    #define DEBUG_TRACE(msg) std::cerr << "[TRACE] " << msg << std::endl
#else
    #define DEBUG_TRACE(msg) // niente, disabilitato
#endif

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
        DEBUG_TRACE("Assigning value to: " << node.id.word << " = " << result.toString());

    } else {
        throw EvaluationError("Assignment value is missing");
    }
}

void EvaluationVisitor::visit(const ListAssignment& node) {
    // Valuta la posizione della lista
    node.pos->accept(*this);
    int index = result.asInt();

    // Valuta il valore da assegnare
    node.value->accept(*this);
    Value valueToAssign = result;

    // Ottieni la lista dalla symbol table
    Value& listContainer = symbolTable.getValuemod(node.id.word); // uso getValuemod per avere una reference modificabile
    auto& list = listContainer.asList();

    // Controlla che l'indice sia valido
    if (index < 0 || index >= static_cast<int>(list.size())) {
        throw EvaluationError("Index out of bounds in ListAssignment to '" + node.id.word + "'");
    }

    // Assegna il nuovo valore
    list[index] = valueToAssign;

    DEBUG_TRACE("Assigned to list " << node.id.word << "[" << index << "] = " << valueToAssign.toString());
}

void EvaluationVisitor::visit(const ListCreation& node) {
    DEBUG_TRACE("Creating list " << node.id.word);
    symbolTable.setValue(node.id.word, Value(std::vector<Value>{}));
    //stampa solo se DEBUG_ON
    if (DEBUG_ON) {
        console_ << "[DEBUG] Created list: " << node.id.word << std::endl;
    }
    
}

void EvaluationVisitor::visit(const Append& node) {
    // Ottieni riferimento modificabile
    Value& container = symbolTable.getValuemod(node.id.word);
    auto& list = container.asList();

    node.expr->accept(*this);
    DEBUG_TRACE("About to append this evaluated expr: " << result.toString());

    list.push_back(result);

    DEBUG_TRACE("Appended to list " << node.id.word << ": " << result.toString());
    if (DEBUG_ON) {
        symbolTable.dump(console_);
    }
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
//non so se funziona
void EvaluationVisitor::visit(const WhileStatement& node) {
    try {
        node.condition->accept(*this);
        while (result.asBool()) {
            try {
                node.block->accept(*this);
            } catch (const ContinueException&) {
                //non so come farlo
                
            }
            node.condition->accept(*this);
        }
    } catch (const BreakException&) {
       //non so come farlo
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

    DEBUG_TRACE("Equality: " << leftVal.toString() 
             << " == " << rightVal.toString());
    if (node.op.tag == Token::EQEQ) {
        bool resultBool = (leftVal == rightVal);
        DEBUG_TRACE("Risultato confronto bool grezzo: " << resultBool);
        result = Value(leftVal == rightVal);
    } else {
        result = Value(!(leftVal == rightVal));

    DEBUG_TRACE("Equality result: " << result.toString());
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
        case Token::DIV: // non ci dovrebbe essere pero boolexpr
            if (rhs == 0) throw EvaluationError("Division by zero");
            result = Value((lhs) / rhs);
            break;
        default:
            throw EvaluationError("Unknown term operator" + std::to_string(node.op.tag));
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
        DEBUG_TRACE("Accessing list: " << node.id.word  << " at index: " << index << ", list size: " << list.size());
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
