#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <stdexcept>

class PDA;
class TM;

using namespace std;

// custom exception class for parsing errors
class ParseError : public runtime_error
{
public:
    ParseError(const string& message) : runtime_error(message) {}
};

// custom exception class for invalid input errors
class InvalidInputError : public runtime_error
{
public:
    InvalidInputError(const string& message) : runtime_error(message) {}
};

// parsers
class PDAParser
{
public:
    PDA parseFile(const string& filename);
};

class TMParser
{
public:
    TM parseFile(const string& filename);
};

#endif