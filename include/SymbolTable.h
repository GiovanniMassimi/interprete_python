#if !defined(SYMBOL_TABLE_H)
#define SYMBOL_TABLE_H

#include <string>
#include <sstream>
#include <unordered_map>

#include "Exception.h"
#include "Value.h"


class SymbolTable {

public:
    //constructors and destructors
	SymbolTable() = default;
	~SymbolTable() = default;

	SymbolTable(const SymbolTable& other) = delete;
	SymbolTable& operator=(const SymbolTable& other) = delete;

    //gives a modifiable reference for the append case
    Value& getValueMod(std::string const& key) {
    auto itr = map.find(key);
    if (itr == map.end()) {
        std::stringstream temp;
        temp << "Undeclared identifier: " << key;
        throw EvaluationError{temp.str()};
    }
    return itr->second;
    }

    // function to get the value of a variable
	Value getValue(std::string const& key) const {
    auto itr = map.find(key);
    if (itr == map.end()) {
        std::stringstream temp;
        temp << "Undeclared identifier: " << key;
        throw EvaluationError{temp.str()};
    }
    return itr->second;
    }

    // function to set the value of a variable
	void setValue(std::string const& key, Value value) {
		map[key] = value;
	}

    //function for debugging
    void PrintSymbols(std::ostream& out) const {
        out << "---- Symbol Table ----" << std::endl;
        for (const auto& [key, val] : map) {
            out << key << " = " << val.toString() << std::endl;
        }
        out << "----------------------" << std::endl;
    }

    private:
        //map of variable names and values
        std::unordered_map<std::string, Value> map;

};

#endif 