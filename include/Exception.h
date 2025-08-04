#if !defined(EXCEPTION_H)
#define EXCEPTION_H

#include <stdexcept>
#include <string>

struct LexicalError : std::runtime_error {
	LexicalError(const char* msg) : std::runtime_error(msg) {}
	LexicalError(std::string msg) : std::runtime_error(msg.c_str()) {}
    
};

struct SyntaxError : std::runtime_error {
	SyntaxError(const char* msg) : std::runtime_error(msg) {}
	SyntaxError(std::string msg) : std::runtime_error(msg.c_str()) {}
    
};

struct EvaluationError : std::runtime_error {
	EvaluationError(const char* msg) : std::runtime_error(msg) {}
	EvaluationError(std::string msg) : std::runtime_error(msg.c_str()) {}
  
};

struct ParseException : std::runtime_error {
	ParseException(const char* msg) : std::runtime_error(msg) {}
	ParseException(std::string msg) : std::runtime_error(msg.c_str()) {}
};

#endif
