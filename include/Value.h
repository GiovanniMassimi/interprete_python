#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <vector>

class Value {
public:
    enum class Type { INT, BOOL, LIST, NONE };

private:
    Type type_;
    int int_val_;
    bool bool_val_;
    std::vector<Value> list_val_;

public:
    Value() : type_(Type::NONE) {}
    Value(int v) : type_(Type::INT), int_val_(v) {}
    Value(bool b) : type_(Type::BOOL), bool_val_(b) {}
    Value(std::vector<Value> v) : type_(Type::LIST), list_val_(std::move(v)) {}

    Type type() const { return type_; }

    const std::vector<Value>& asList() const {
        if (type_ != Type::LIST) throw std::runtime_error("Value is not a list");
        return list_val_;
    }
    std::vector<Value>& asList() {
        if (type_ != Type::LIST) throw std::runtime_error("Value is not a list");
        return list_val_;
    }

    // altri metodi (asInt, asBool, operator==, ecc.)
    int asInt() const {
        if (type_ != Type::INT) throw EvaluationError("Value is not an int");
        return int_val_;
    }

    bool asBool() const {
        if (type_ != Type::BOOL) throw EvaluationError("Value is not a bool");
        return bool_val_;
    }

    std::string toString() const {
        switch(type_) {
            case Type::INT:
                return std::to_string(int_val_);
            case Type::BOOL:
                return bool_val_ ? "True" : "False";
            case Type::LIST: {
                std::stringstream ss;
                ss << "[";
                for (size_t i = 0; i < list_val_.size(); ++i) {
                    ss << list_val_[i].toString();
                    if (i + 1 < list_val_.size())
                        ss << ", ";
                }
                ss << "]";
                return ss.str();
            }
            default:
                return "None";
        }
    }


    bool operator==(const Value& other) const {
    if (type_ != other.type_) {
        return false;
    }

    switch (type_) {
        case Type::INT:
            return int_val_ == other.int_val_;
        case Type::BOOL:
            return bool_val_ == other.bool_val_;
        case Type::LIST:
            return list_val_ == other.list_val_;
        case Type::NONE:
            return true;
        default:
            throw std::runtime_error("Unexpected Value type in operator==");
    }
}

bool operator<(const Value& other) const {
    if (type_ != other.type_)
        throw std::runtime_error("Cannot compare different types");

        switch (type_) {
            case Type::INT:
                return int_val_ < other.int_val_;
            case Type::BOOL:
                return bool_val_ < other.bool_val_;
            default:
                throw std::runtime_error("Cannot compare this type");
        }
    }

    bool operator>(const Value& other) const {
        if (type_ != other.type_)
            throw std::runtime_error("Cannot compare different types");

        switch (type_) {
            case Type::INT:
                return int_val_ > other.int_val_;
            case Type::BOOL:
                return bool_val_ > other.bool_val_;
            default:
                throw std::runtime_error("Cannot compare this type");
        }
    }

    

    bool operator<=(const Value& other) const {
        return *this < other || *this == other;
    }

    bool operator>=(const Value& other) const {
        return *this > other || *this == other;
    }

};

inline std::ostream& operator<<(std::ostream& os, const Value& val) {
    os << val.toString();
    return os;
}
#endif // VALUE_H