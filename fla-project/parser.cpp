#include "parser.h"
#include "pda.h"
#include "tm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

using namespace std;

// helper function to trim whitespace on the two ends of a string
string trim(const string& str)
{
    size_t first = str.find_first_not_of(' ');
    // if the entire string is filled with whitespace, just return the original string
    if (string::npos == first) return str;
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// helper function to split a string into tokens given a delimiter
vector<string> split(const string& s, char delimiter)
{
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) tokens.push_back(trim(token));
    return tokens;
}

PDA PDAParser::parseFile(const string& filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        throw runtime_error("Error opening file: " + filename);
    }

    PDA pda;
    string line;

    while (getline(file, line))
    {
        // ignore comments
        size_t comment_pos = line.find(';');
        if (comment_pos != string::npos)
        {
            line = line.substr(0, comment_pos);
        }
        // skip empty lines
        line = trim(line);
        if (line.empty()) continue;

        // set of states
        if (pda.states.empty() && line.rfind("#Q =", 0) == 0)
        {
            string states_str = line.substr(line.find('{') + 1, line.rfind('}') - line.find('{') - 1);
            vector<string> states = split(states_str, ',');
            pda.states = set<string>(states.begin(), states.end());
        }
        // set of input alphabet
        else if (pda.alphabet.empty() && line.rfind("#S =", 0) == 0)
        {
            string alphabet_str = line.substr(line.find('{') + 1, line.rfind('}') - line.find('{') - 1);
            pda.alphabet = set<char>();
            for (const auto& s : split(alphabet_str, ','))
            {
                if (s.length() == 1)
                {
                    if (s == " " || s == "," || s == ";" || s == "{" || s == "}" || s == "*" || s == "_")
                    {
                        throw ParseError("Invalid input symbol: " + s);
                    }
                    pda.alphabet.insert(s[0]);
                }
                else
                {
                    throw ParseError("Invalid input symbol: " + s);
                }
            }
        }
        // set of stack alphabet
        else if (pda.stack_alphabet.empty() && line.rfind("#G =", 0) == 0)
        {
            string stack_alphabet_str = line.substr(line.find('{') + 1, line.rfind('}') - line.find('{') - 1);
            pda.stack_alphabet = set<char>();
            for (const auto& s : split(stack_alphabet_str, ','))
            {
                if (s.length() == 1)
                {
                    if (s == " " || s == "," || s == ";" || s == "{" || s == "}" || s == "*" || s == "_")
                    {
                        throw ParseError("Invalid stack symbol: " + s);
                    }
                    pda.stack_alphabet.insert(s[0]);
                }
                else
                {
                    throw ParseError("Invalid stack symbol: " + s);
                }
            }
        }
        // start state
        else if (pda.start_state.empty() && line.rfind("#q0 =", 0) == 0)
        {
            pda.start_state = trim(line.substr(line.find('=') + 1));
            if (pda.states.find(pda.start_state) == pda.states.end())
            {
                throw ParseError("Start state not defined in the set of states.");
            }
        }
        // start stack symbol
        else if (pda.start_stack_symbol == '\0' && line.rfind("#z0 =", 0) == 0)
        {
            pda.start_stack_symbol = trim(line.substr(line.find('=') + 1))[0];
            if (pda.stack_alphabet.find(pda.start_stack_symbol) == pda.stack_alphabet.end())
            {
                throw ParseError("Start stack symbol not defined in stack alphabet.");
            }
        }
        // final state(s)
        else if (pda.final_states.empty() && line.rfind("#F =", 0) == 0)
        {
            string final_states_str = line.substr(line.find('{') + 1, line.rfind('}') - line.find('{') - 1);
            vector<string> final_states = split(final_states_str, ',');
            pda.final_states = set<string>(final_states.begin(), final_states.end());
            for (const auto& state : pda.final_states)
            {
                if (pda.states.find(state) == pda.states.end())
                {
                    throw ParseError("Final state not defined in the set of states.");
                }
            }
        }
        else
        {
            // parse transition function
            // δ(q, a, X) = (p, Y)
            stringstream ss(line);
            string current_state, input_symbol_str, stack_top_str, next_state, stack_push_str;
            if (ss >> current_state >> input_symbol_str >> stack_top_str >> next_state >> stack_push_str)
            {
                char input_symbol = input_symbol_str[0];
                char stack_top = stack_top_str[0];
                // check if current state, next state, input symbol and stack symbol has been defined
                if (pda.states.find(current_state) == pda.states.end()
                    || pda.states.find(next_state) == pda.states.end())
                {
                    throw ParseError("Invalid state in transition function.");
                }
                if (input_symbol != '_' && pda.alphabet.find(input_symbol) == pda.alphabet.end())
                {
                    throw ParseError("Invalid input symbol in transition function.");
                }
                if (stack_top != '_' && pda.stack_alphabet.find(stack_top) == pda.stack_alphabet.end())
                {
                    throw ParseError("Invalid stack symbol in transition function.");
                }

                vector<char> stack_push;
                for (char c : stack_push_str)
                {
                    if (pda.stack_alphabet.find(c) != pda.stack_alphabet.end())
                    {
                        stack_push.push_back(c);
                    }
                    else
                    {
                        if (c != '_') throw ParseError("Invalid stack symbol in stack_push string.");
                    }
                }
                reverse(stack_push.begin(), stack_push.end()); // stack bottom is at the end

                pda.transitions[{current_state, input_symbol, stack_top}] = {next_state, stack_push};

            }
            else
            {
                throw ParseError("Error parsing transition function: " + line);
            }
        }
    }

    if (pda.states.empty() || pda.alphabet.empty() || pda.stack_alphabet.empty() ||
        pda.start_state.empty() || pda.start_stack_symbol == '\0' || pda.final_states.empty())
    {
        throw ParseError("Incomplete PDA definition.");
    }

    return pda;
}

TM TMParser::parseFile(const string& filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        throw runtime_error("Error opening file: " + filename);
    }

    TM tm;
    string line;

    while (getline(file, line))
    {
        // ignore comments
        size_t comment_pos = line.find(';');
        if (comment_pos != string::npos)
        {
            line = line.substr(0, comment_pos);
        }
        // skip empty lines
        line = trim(line);
        if (line.empty()) continue;
        
        // set of states
        if (tm.states.empty() && line.rfind("#Q =", 0) == 0)
        {
            string states_str = line.substr(line.find('{') + 1, line.rfind('}') - line.find('{') - 1);
            vector<string> states = split(states_str, ',');
            tm.states = set<string>(states.begin(), states.end());
        }
        // input alphabet
        else if (tm.alphabet.empty() && line.rfind("#S =", 0) == 0)
        {
            string alphabet_str = line.substr(line.find('{') + 1, line.rfind('}') - line.find('{') - 1);
            tm.alphabet = set<char>(alphabet_str.begin(), alphabet_str.end());
            tm.alphabet.erase(','); // remove comma
        }
        // tape alphabet
        else if (tm.tape_alphabet.empty() && line.rfind("#G =", 0) == 0)
        {
            string tape_alphabet_str = line.substr(line.find('{') + 1, line.rfind('}') - line.find('{') - 1);
            tm.tape_alphabet = set<char>(tape_alphabet_str.begin(), tape_alphabet_str.end());
            tm.tape_alphabet.erase(',');
        }
        // start state
        else if (tm.start_state.empty() && line.rfind("#q0 =", 0) == 0)
        {
            tm.start_state = trim(line.substr(line.find('=') + 1));
            if (tm.states.find(tm.start_state) == tm.states.end())
            {
                throw ParseError("Start state not defined in the set of states.");
            }
        }
        // blank symbol (ensured to be '_')
        else if (tm.blank_symbol == '\0' && line.rfind("#B =", 0) == 0)
        {
            tm.blank_symbol = trim(line.substr(line.find('=') + 1))[0];
            if (tm.blank_symbol != '_')
            {
                throw ParseError("Blank symbol was expected to be \'_\', but got " + tm.blank_symbol);
            }
        }
        // accept states
        else if (tm.accept_states.empty() && line.rfind("#F =", 0) == 0)
        {
            string accepting_states_str = line.substr(line.find('{') + 1, line.rfind('}') - line.find('{') - 1);
            vector<string> accepting_states = split(accepting_states_str, ',');
            tm.accept_states = set<string>(accepting_states.begin(), accepting_states.end());
            for (const auto& state : tm.accept_states)
            {
                if (tm.states.find(state) == tm.states.end())
                {
                    throw ParseError("Accepting state not defined in the set of states.");
                }
            }
        }
        // number of tapes
        else if (tm.num_tapes == 100 && line.rfind("#N =", 0) == 0) // number of tapes is ensured to be less than 10
        {
            tm.num_tapes = stoi(trim(line.substr(line.find('=') + 1)));
            if (tm.num_tapes <= 0)
            {
                throw ParseError("Number of tapes must be a positive integer.");
            }
        }
        else
        {
            // parse transition function
            // δ(current_state, read_symbols) = (write_symbols, directions, next_state)
            stringstream ss(line);
            string current_state, next_state, directions_str, read_symbols_str, write_symbols_str;
            vector<string> read_symbols(tm.num_tapes);
            vector<string> write_symbols(tm.num_tapes);

            ss >> current_state >> read_symbols_str >> write_symbols_str >> directions_str >> next_state;

            if (read_symbols_str.length() != tm.num_tapes
                || write_symbols_str.length() != tm.num_tapes
                || directions_str.length() != tm.num_tapes)
            {
                throw ParseError("Number of read symbols / write symbols / directions does not match number of tapes." + line);
            }

            for (int i = 0; i < tm.num_tapes; i++)
            {
                read_symbols[i] = read_symbols_str[i];
                write_symbols[i] = write_symbols_str[i];
            }

            if (ss.fail())
            {
                throw ParseError("Error parsing TM transition: " + line);
            }

            if (tm.states.find(current_state) == tm.states.end()
                || tm.states.find(next_state) == tm.states.end())
            {
                throw ParseError("Invalid state in transition function." + line);
            }

            TM::TransitionFunction transition;
            for (int i = 0; i < tm.num_tapes; i++)
            {
                char read_symbol_char;
                if (read_symbols[i] == "*")
                {
                    read_symbol_char = '*';
                }
                else if (read_symbols[i].length() == 1)
                {
                    read_symbol_char = read_symbols[i][0];
                    if (tm.tape_alphabet.find(read_symbol_char) == tm.tape_alphabet.end() && read_symbol_char != '*')
                    {
                        throw ParseError("Invalid read symbol in transition function.");
                    }
                }
                else
                {
                     throw ParseError("Invalid read symbol format in transition function.");
                }
                transition.read_symbols.push_back(read_symbol_char);

                char write_symbol_char;
                if (write_symbols[i] == "*")
                {
                    write_symbol_char = '*';
                }
                else if (write_symbols[i].length() == 1)
                {
                    write_symbol_char = write_symbols[i][0];
                    if (tm.tape_alphabet.find(write_symbol_char) == tm.tape_alphabet.end() && write_symbol_char != '*')
                    {
                        throw ParseError("Invalid write symbol in transition function." + line);
                    }
                }
                else
                {
                    throw ParseError("Invalid write symbol format in transition function." + line);
                }

                // when star appears in write symbol, read symbol should not contain star
                if (write_symbol_char == '*' && read_symbol_char != '*')
                {
                    throw ParseError("Invalid write symbol format in transition function." + line);
                }

                transition.write_symbols.push_back(write_symbol_char);

                char direction = directions_str[i];
                if (direction != 'l' && direction != 'r' && direction != '*')
                {
                    throw ParseError("Invalid direction in transition function." + line);
                }
                transition.directions.push_back(direction);
            }
            transition.next_state = next_state;

            tm.transitions[{current_state, transition.read_symbols}] = transition;
        }
    }

    if (tm.states.empty() || tm.alphabet.empty() || tm.tape_alphabet.empty() ||
        tm.start_state.empty() || tm.blank_symbol == '\0' || tm.accept_states.empty() || tm.num_tapes == 100)
    {
        throw ParseError("Incomplete TM definition.");
    }

    return tm;
}