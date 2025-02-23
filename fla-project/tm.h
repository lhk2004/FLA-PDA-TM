#ifndef TM_H
#define TM_H

#include "parser.h"
#include <string>
#include <set>
#include <map>
#include <vector>
#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;

class TM
{
public:
    set<string> states;
    set<char> alphabet;
    set<char> tape_alphabet;
    string start_state;
    char blank_symbol = '\0';
    set<string> accept_states;
    int num_tapes = 100;

    struct TransitionFunction
    {
        vector<char> read_symbols;
        vector<char> write_symbols;
        vector<char> directions; // 'l', 'r', '*' for left, right, no move
        string next_state;

        bool operator<(const TransitionFunction& other) const
        {
            if (read_symbols != other.read_symbols) return read_symbols < other.read_symbols;
            if (write_symbols != other.write_symbols) return write_symbols < other.write_symbols;
            if (directions != other.directions) return directions < other.directions;
            return next_state < other.next_state;
        }
    };

    // transition function: (current_state, read_symbols) -> Transition
    using Transitions = map<pair<string, vector<char>>, TransitionFunction>;
    Transitions transitions;
};

class TMSimulator
{
public:
    TMSimulator(const TM& tm) : tm_(tm) {}

    void run(const string& input);
    void run_verbose(const string& input);

private:
    const TM& tm_;

    struct Tape
    {
        Tape(const TM& tm) : tm_(tm) {}

        map<int, char> content;
        int head_position = 0;
        const TM& tm_;

        char read() const
        {
            return content.count(head_position) ? content.at(head_position) : tm_.blank_symbol;
        }

        void write(char symbol)
        {
            content[head_position] = symbol;
        }

        void move_left()
        {
            head_position--;
        }

        void move_right()
        {
            head_position++;
        }

        string get_content_string() const
        {
            if (content.empty())
            {
                return "";
            }
            int min_pos = content.begin()->first;
            int max_pos = content.rbegin()->first;
            string result = "";
            for (int i = min_pos; i <= max_pos; i++)
            {
                result += (content.count(i) ? content.at(i) : tm_.blank_symbol);
            }

            // trim leading and trailing blank symbols
            size_t first = result.find_first_not_of(tm_.blank_symbol);
            if (string::npos == first)
            {
                return "";
            }
            size_t last = result.find_last_not_of(tm_.blank_symbol);
            return result.substr(first, (last - first + 1));
        }
    };
};

#endif