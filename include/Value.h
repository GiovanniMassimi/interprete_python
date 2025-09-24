#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include "Exception.h"

//helper class for managing number and boolean values
// (got the idea from a youtube video)
class Value {
public:
    enum class Type { INT, BOOL, LIST }; //enum for the type of value

private:
    //definition for all possible types
    Type type_; 
    int int_val_;
    bool bool_val_;
    std::vector<Value> list_val_;

public:
    //constructors for all possible types
    Value(int v) : type_(Type::INT), int_val_(v) {}
    Value(bool b) : type_(Type::BOOL), bool_val_(b) {}
    Value(std::vector<Value> v) : type_(Type::LIST), list_val_(std::move(v)) {}
    Value() : type_(Type::INT), int_val_(0) {}

    //function to get the type for the value
    Type type() const { return type_; }

    //function to get the list value
    const std::vector<Value>& asList() const {
        if (type_ != Type::LIST) throw TypeError("It's not a list");
        return list_val_;
    }
    //modifiable version for append
    std::vector<Value>& asList() {
        if (type_ != Type::LIST) throw TypeError("It's not a list");
        return list_val_;
    }
    
    //functions to get the int value
    int asInt() const {
        if (type_ != Type::INT) throw TypeError("It's not an int");
        return int_val_;
    }
    //function to get the bool value
    bool asBool() const {
        if (type_ != Type::BOOL) throw TypeError("It's not a bool");
        return bool_val_;
    }
    //function to convert the value to a string (for printing)
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
        }
        throw TypeError("Invalid Value type"); // should never happen
    }

    // Override equality operator
    bool operator==(const Value& other) const {
        if (type_ != other.type_) return false;

        switch (type_) {
            case Type::INT:
                return int_val_ == other.int_val_;
            case Type::BOOL:
                return bool_val_ == other.bool_val_;
            case Type::LIST:
                return list_val_ == other.list_val_;
        }
        throw TypeError("Invalid Value type in operator==");
    }

    // Override less than operator
    bool operator<(const Value& other) const {
        if (type_ != other.type_)
            throw TypeError("Cannot compare different types");

        switch (type_) {
            case Type::INT:
                return int_val_ < other.int_val_;
            case Type::BOOL:
                return bool_val_ < other.bool_val_;
            case Type::LIST:
                throw TypeError("Cannot compare lists");
        }
        throw TypeError("Invalid Value type in operator<");
    }

    // Override greater than operator
    bool operator>(const Value& other) const {
        if (type_ != other.type_)
            throw TypeError("Cannot compare different types");

        switch (type_) {
            case Type::INT:
                return int_val_ > other.int_val_;
            case Type::BOOL:
                return bool_val_ > other.bool_val_;
            case Type::LIST:
                throw TypeError("Cannot compare lists");
        }
        throw TypeError("Invalid Value type in operator>");
    }

    // Override less than or equal operator
    bool operator<=(const Value& other) const {
        return *this < other || *this == other;
    }

    // Override greater than or equal operator
    bool operator>=(const Value& other) const {
        return *this > other || *this == other;
    }
};

// Overload the output stream operator for easy printing of values
inline std::ostream& operator<<(std::ostream& os, const Value& val) {
    os << val.toString();
    return os;
}

#endif
