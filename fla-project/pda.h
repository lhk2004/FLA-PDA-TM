#ifndef PDA_H
#define PDA_H

#include "parser.h"
#include <string>
#include <set>
#include <map>
#include <vector>
#include <stack>
#include <stdexcept>
#include <iostream>
#include <algorithm>

using namespace std;

class PDA
{
public:
    set<string> states;
    set<char> alphabet;
    set<char> stack_alphabet;
    string start_state;
    char start_stack_symbol = '\0';
    set<string> final_states;

    // transition: (next_state, stack_push_sequence)
    using Transition = pair<string, vector<char>>;

    // transition function: (current_state, input_symbol, stack_top) -> Transition
    using TransitionFunction = map<tuple<string, char, char>, Transition>;
    TransitionFunction transitions;
};

class PDASimulator
{
public:
    PDASimulator(const PDA& pda) : pda_(pda) {}

    bool run(const std::string& input);
    void run_verbose(const string& input);

private:
    const PDA& pda_;
};

#endif