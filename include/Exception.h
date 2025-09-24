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

struct ParseError : std::runtime_error {
	ParseError(const char* msg) : std::runtime_error(msg) {}
	ParseError(std::string msg) : std::runtime_error(msg.c_str()) {}
};

struct TypeError : std::runtime_error {
    TypeError(const char* msg) : std::runtime_error(msg) {}
    TypeError(std::string msg) : std::runtime_error(msg.c_str()) {}
};

// exception to handle break and continue with try and catch in while loops
struct BreakException : public std::exception {
    const char* what() const noexcept override { return "Break statement encountered"; }
};

struct ContinueException : public std::exception {
    const char* what() const noexcept override { return "Continue statement encountered"; }
};

#endif
