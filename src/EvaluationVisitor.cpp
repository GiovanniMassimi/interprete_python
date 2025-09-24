#include "EvaluationVisitor.h"
#include "Syntax.h" 
#include "Exception.h"

#define DEBUG_ON 0

#if DEBUG_ON
    #define DEBUG_TRACE(msg) std::cerr << "[TRACE] " << msg << std::endl
#else
    #define DEBUG_TRACE(msg) 
#endif

void EvaluationVisitor::visit(const Program& node) {
    for (const auto& stmt : node.stmts) {
        if (stmt) stmt->accept(*this);// evaluate each statement
    }
}

void EvaluationVisitor::visit(const Statement& node) {
   //Should remove visit from virtual class

}

void EvaluationVisitor::visit(const Expression& node) {
    //Should remove visit from virtual class

}

void EvaluationVisitor::visit(const Assignment& node) {
    if (node.value) { // Check if value is not null
        node.value->accept(*this); // Evaluate the expression on the right-hand side
        symbolTable.setValue(node.id.word, result); // Assign the result to the variable id in the symbol table
        DEBUG_TRACE("Assigning value to: " << node.id.word << " = " << result.toString());

    } else {
        throw EvaluationError("Assignment value is missing");
    }
}

void EvaluationVisitor::visit(const ListAssignment& node) {
    node.pos->accept(*this); // Evaluate the index expression
    int index = result.asInt(); // Get the index as an integer

    node.value->accept(*this); // Evaluate the value expression
    Value valueToAssign = result; 

    Value& list_ = symbolTable.getValueMod(node.id.word); // Using getValueMod for not const list
    auto& list = list_.asList();

    if (index < 0 || index >= static_cast<int>(list.size())) { // Check for index bounds
        throw EvaluationError("Index out of bounds in ListAssignment to '" + node.id.word + "'");
    }

    list[index] = valueToAssign; // Assign the value at the index

    DEBUG_TRACE("Assigned to list " << node.id.word << "[" << index << "] = " << valueToAssign.toString());
}

void EvaluationVisitor::visit(const ListCreation& node) {
    DEBUG_TRACE("Creating list " << node.id.word);
    symbolTable.setValue(node.id.word, Value(std::vector<Value>{})); // Initialize with an empty list
    if (DEBUG_ON) {
        console_ << "[DEBUG] Created list: " << node.id.word << std::endl;
    }
    
}

void EvaluationVisitor::visit(const Append& node) {
    Value& identifier = symbolTable.getValueMod(node.id.word);
    auto& list = identifier.asList();

    node.expr->accept(*this); // Evaluate the expression to append
    DEBUG_TRACE("Evaluated expr: " << result.toString());

    list.push_back(result); // Append the result to the list

    DEBUG_TRACE("Appended to list " << node.id.word << ": " << result.toString());
    if (DEBUG_ON) {
        symbolTable.PrintSymbols(console_);
    }
}

void EvaluationVisitor::visit(const Break& node) {
    if (!insideLoop) {
        return; // Ignore break if not inside a loop
    }
    throw BreakException{};
}

void EvaluationVisitor::visit(const Continue& node) {
    if (!insideLoop) {
        return; // Ignore continue if not inside a loop
    }
    throw ContinueException{};
}

void EvaluationVisitor::visit(const Print& node) {
    node.expr->accept(*this); // Evaluate the expression to print
    console_ << result.toString() << std::endl; // Output the result to the console
}

void EvaluationVisitor::visit(const Block& node) {
    for (auto stmt : node.stmts) {
        stmt->accept(*this); // Evaluate each statement in the block
    }
}
void EvaluationVisitor::visit(const WhileStatement& node) {
    insideLoop = true; // Mark that we are inside a loop for break/continue handling
    try { // Catch BreakException to exit the loop
        node.condition->accept(*this); // Evaluate the loop condition
        while (result.asBool()) {
            try { // Catch ContinueException to skip to the next iteration
                node.block->accept(*this); // Evaluate the loop body
            } catch (const ContinueException&) {}
            node.condition->accept(*this); // Re-evaluate the condition to end or continue the loop
        }
    } catch (const BreakException&) {}
    insideLoop = false;
}

void EvaluationVisitor::visit(const IfStatement& node) {
    node.condition->accept(*this); // Evaluate the if condition
    if (result.asBool()) { // If true, execute the if block
        node.block->accept(*this); 
        return;
    }
    for (auto elif : node.elif_blocks) { // Check each elif block
        elif->accept(*this);
        if (result.asBool()) return;
    }
    if (node.else_block) { // If no conditions matched, execute the else block if it exists
        node.else_block->accept(*this);
    }
}

void EvaluationVisitor::visit(const ElifBlock& node) {
    node.condition->accept(*this); // Evaluate the elif condition
    if (result.asBool()) { // If true, execute the elif block
        node.block->accept(*this);
    }
}

void EvaluationVisitor::visit(const ElseBlock& node) {
    node.block->accept(*this); // Execute the else block
}

void EvaluationVisitor::visit(const Join& node) {
    node.left->accept(*this); // Evaluate the left expression
    bool leftVal = result.asBool(); // Get the left result as a boolean
    if (node.op.tag == Token::OR) {
        if (leftVal) { 
            result = Value(true); // Short-circuit for OR
            return;
        }
        node.right->accept(*this);
        result = Value(result.asBool());
    } else {
        if (!leftVal) {
            result = Value(false); // Short-circuit for AND
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
        bool resultBool = (leftVal == rightVal); // check equality
        DEBUG_TRACE("bool res: " << resultBool);
        result = Value(leftVal == rightVal);
    } else {
        result = Value(!(leftVal == rightVal));

    DEBUG_TRACE("Equality result: " << result.toString());
    }
}


void EvaluationVisitor::visit(const Relation& node) {
    node.left->accept(*this);
    int leftVal = result.asInt();
    node.right->accept(*this);
    int rightVal = result.asInt();

    switch (node.op.tag) {
        case Token::LT: result = Value(leftVal < rightVal); break;
        case Token::LE: result = Value(leftVal <= rightVal); break;
        case Token::GE: result = Value(leftVal >= rightVal); break;
        case Token::GT: result = Value(leftVal > rightVal); break;
        default:
            throw EvaluationError("Unknown relational operator");
    }
}

void EvaluationVisitor::visit(const NumExpr& node) {
    node.left->accept(*this);
    int leftVal = result.asInt();
    node.right->accept(*this);
    int rightVal = result.asInt();

    switch (node.op.tag) {
        case Token::ADD: result = Value(leftVal + rightVal); break;
        case Token::SUB: result = Value(leftVal - rightVal); break;
        default:
            throw EvaluationError("Unknown numeric operator");
    }
}

void EvaluationVisitor::visit(const Term& node) {
    node.left->accept(*this);
    int leftVal = result.asInt();
    node.right->accept(*this);
    int rightVal = result.asInt();

    switch (node.op.tag) {
        case Token::MUL: result = Value(leftVal * rightVal); break;
        case Token::DIVINT:
            if (rightVal == 0) throw EvaluationError("Division by zero"); 
            result = Value(leftVal / rightVal);
            break;
        case Token::DIV: // it shouldent be but in  boolexpr.txt
            if (rightVal == 0) throw EvaluationError("Division by zero");
            result = Value((leftVal) / rightVal);
            break;
        default:
            throw EvaluationError("Unknown term operator" + std::to_string(node.op.tag));
    }
}

void EvaluationVisitor::visit(const Unary& node) {
    node.right->accept(*this);
    if (node.op.tag == Token::NOT) {
        result = Value(!result.asBool()); //not operator
    } else if (node.op.tag == Token::SUB) {
        result = Value(-result.asInt()); //negation operator
    } else {
        throw EvaluationError("Unknown unary operator");
    }
}

void EvaluationVisitor::visit(const Factor& node) {
    if (node.expr) {
        node.expr->accept(*this);  // Evaluate the inner expression
    } else {
        node.token.accept(*this);  // Evaluate the literal
    }
}

void EvaluationVisitor::visit(const Literal& node) {
    if (node.value.tag == Token::NUM) {
        result = Value(std::stoi(node.value.word)); // Convert string to int
    } else if (node.value.tag == Token::TRUE_) {
        result = Value(true); // Convert "true"  to boolean
    } else if (node.value.tag == Token::FALSE_) {
        result = Value(false); // Convert "false" to boolean
    } else {
        throw EvaluationError("Unknown literal type");
    }
}

void EvaluationVisitor::visit(const Location& node) {
    Value identifier = symbolTable.getValue(node.id.word);
    if (node.index) {
        node.index->accept(*this); // Evaluate the index expression
        int index = result.asInt();
        auto& list = identifier.asList();
        DEBUG_TRACE("Accessing list: " << node.id.word  << " at index: " << index << ", list size: " << list.size());
        if (list.empty()) {
            throw EvaluationError("Attempted to access an empty list: '" + node.id.word + "'");
        }
        if (index < 0 || index >= static_cast<int>(list.size())) {
            throw EvaluationError("Index out of bounds");
        }
        result = list[index];
    } else {
        result = identifier;
    }
}
