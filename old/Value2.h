#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <vector>
#include "Exception.h"
#include <iostream>

class Value {
    std::string type_;  // "int", "bool", "list"
    int int_val_;
    bool bool_val_;
    std::vector<Value> list_val_;

public:
    // Costruttori
    Value() : type_("undefined") {}
    Value(int v) : type_("int"), int_val_(v) {}
    Value(bool v) : type_("bool"), bool_val_(v) {}
    Value(std::vector<Value> v) : type_("list"), list_val_(std::move(v)) {}

    // Accessori
    std::string type() const { return type_; }

    int asInt() const {
        if (type_ != "int") throw EvaluationError("Value is not an int");
        return int_val_;
    }

    bool asBool() const {
        if (type_ != "bool") throw EvaluationError("Value is not a bool");
        return bool_val_;
    }

    const std::vector<Value>& asList() const {
        if (type_ != "list") throw EvaluationError("Value is not a list");
        return list_val_;
    }

    std::vector<Value>& asList() {
        if (type_ != "list") throw EvaluationError("Value is not a list");
        return list_val_;
    }

    // Utili per il debug
    void print(std::ostream& os) const {
        if (type_ == "int") os << int_val_;
        else if (type_ == "bool") os << (bool_val_ ? "True" : "False");
        else if (type_ == "list") {
            os << "[";
            for (size_t i = 0; i < list_val_.size(); ++i) {
                list_val_[i].print(os);
                if (i + 1 < list_val_.size()) os << ", ";
            }
            os << "]";
        } else {
            os << "<undefined>";
        }
    }
};

// operator<< per stamparlo facilmente
inline std::ostream& operator<<(std::ostream& os, const Value& val) {
    val.print(os);
    return os;
}

#endif // VALUE_H