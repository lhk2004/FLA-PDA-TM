#include "tm.h"
#include <iostream>
#include <stdexcept>

using namespace std;

void TMSimulator::run(const string& input)
{
    vector<Tape> tapes;
    for (int i = 0; i < tm_.num_tapes; i++)
    {
        tapes.emplace_back(tm_);
    }
    string current_state = tm_.start_state;
    int step = 0;

    // initialize the first tape with the input
    for (int i = 0; i < input.length(); i++)
    {
        tapes[0].content[i] = input[i];
        // see whether input string is legal or not
        if (tm_.alphabet.find(input[i]) == tm_.alphabet.end())
        {
            throw InvalidInputError("Invalid input character: " + string(1, input[i]));
        }
    }

    while (tm_.accept_states.find(current_state) == tm_.accept_states.end())
    {
        bool transition_found = false;
        vector<char> current_symbols;
        for (int i = 0; i < tm_.num_tapes; i++)
        {
            current_symbols.push_back(tapes[i].read());
        }

        for (const auto& entry : tm_.transitions)
        {
            if (entry.first.first == current_state) // current state
            {
                bool symbols_match = true;
                for (int i = 0; i < tm_.num_tapes; i++) // read symbols
                {
                    if ((entry.first.second[i] != '*' && entry.first.second[i] != current_symbols[i])
                        || (entry.first.second[i] == '*' && current_symbols[i] == '_'))  // star should not match blank symbol
                    {
                        symbols_match = false;
                        break;
                    }
                }

                if (symbols_match)
                {
                    const TM::TransitionFunction& transition = entry.second;
                    for (int i = 0; i < tm_.num_tapes; i++)
                    {
                        if (transition.write_symbols[i] != '*')
                        {
                            tapes[i].write(transition.write_symbols[i]);
                        }
                        if (transition.directions[i] == 'l')
                        {
                            tapes[i].move_left();
                        }
                        else if (transition.directions[i] == 'r')
                        {
                            tapes[i].move_right();
                        }
                    }
                    current_state = transition.next_state;
                    transition_found = true;
                    break;
                }
            }
        }

        if (!transition_found)
        {
            cout << tapes[0].get_content_string() << endl;
            return;
        }
    }

    cout << tapes[0].get_content_string() << endl;
}

void TMSimulator::run_verbose(const string& input)
{
    vector<Tape> tapes;
    for (int i = 0; i < tm_.num_tapes; i++)
    {
        tapes.emplace_back(tm_);
    }
    string current_state = tm_.start_state;
    int step = 0;

    cout << "Input: " << input << endl;

    // initialize the first tape with the input and input validation
    for (int i = 0; i < input.length(); i++)
    {
        tapes[0].content[i] = input[i];
        if (tm_.alphabet.find(input[i]) == tm_.alphabet.end())
        {
            cerr << "==================== ERR ====================" << endl;
            cerr << "error: '" << input[i] << "' was not declared in the set of input symbols" << endl;
            cerr << "Input: " << input << endl;
            cerr << string(7 + i, ' ') << "^" << endl;
            cerr << "==================== END ====================" << endl;
            exit(1);
        }
    }

    auto print_state = [&]()
    {
        cout << "Step   : " << step << endl;
        cout << "State  : " << current_state << endl;

        for (int i = 0; i < tm_.num_tapes; i++)
        {
            int min_idx, max_idx;

            min_idx = tapes[i].head_position;
            max_idx = tapes[i].head_position;

            if (!tapes[i].content.empty())
            {
                min_idx = tapes[i].content.begin()->first;
                max_idx = tapes[i].content.rbegin()->first;

                while (min_idx <= max_idx && tapes[i].content.count(min_idx) && tapes[i].content[min_idx] == '_')
                {
                    min_idx++;
                }
                while (min_idx <= max_idx && tapes[i].content.count(max_idx) && tapes[i].content[max_idx] == '_')
                {
                    max_idx--;
                }

                if (min_idx > max_idx)
                {
                    min_idx = tapes[i].head_position;
                    max_idx = tapes[i].head_position;
                }
            }

            min_idx = min(min_idx, tapes[i].head_position);
            max_idx = max(max_idx, tapes[i].head_position);

            string index_line = "Index" + to_string(i) + " :";
            string tape_line = "Tape" + to_string(i) + "  : ";
            string head_line = "Head" + to_string(i) + "  : ";

            string prev_index_str = "";
            bool is_first_index = true;

            for (int j = min_idx; j <= max_idx; j++)
            {
                string index_str = to_string(abs(j));
                int padding_spaces = prev_index_str.length();

                index_line += " " + index_str;

                if (!is_first_index)
                {
                    tape_line += string(padding_spaces, ' ');
                    head_line += string(padding_spaces, ' ');
                }
                else
                {
                    is_first_index = false;
                }


                if (tapes[i].content.find(j) != tapes[i].content.end())
                {
                    tape_line += tapes[i].content[j];
                }
                else
                {
                    tape_line += "_";
                }

                head_line += (j == tapes[i].head_position ? "^" : " ");


                prev_index_str = index_str;
            }
            cout << index_line << endl;
            cout << tape_line << endl;
            cout << head_line << endl;
        }
        cout << "---------------------------------------------" << endl;
    };

    cout << "==================== RUN ====================" << endl;
    print_state();

    while (tm_.accept_states.find(current_state) == tm_.accept_states.end())
    {
        bool transition_found = false;
        vector<char> current_symbols;
        for (int i = 0; i < tm_.num_tapes; i++)
        {
            current_symbols.push_back(tapes[i].read());
        }

        for (const auto& entry : tm_.transitions)
        {
            if (entry.first.first == current_state)
            {
                bool symbols_match = true;
                for (int i = 0; i < tm_.num_tapes; i++)
                {
                    if ((entry.first.second[i] != '*' && entry.first.second[i] != current_symbols[i])
                        || (entry.first.second[i] == '*' && current_symbols[i] == '_'))  // star should not match blank symbol
                    {
                        symbols_match = false;
                        break;
                    }
                }

                if (symbols_match)
                {
                    const TM::TransitionFunction& transition = entry.second;
                    for (int i = 0; i < tm_.num_tapes; i++)
                    {
                        if (transition.write_symbols[i] != '*')
                        {
                            tapes[i].write(transition.write_symbols[i]);
                        }
                        if (transition.directions[i] == 'l')
                        {
                            tapes[i].move_left();
                        }
                        else if (transition.directions[i] == 'r')
                        {
                            tapes[i].move_right();
                        }
                    }
                    current_state = transition.next_state;
                    transition_found = true;
                    break;
                }
            }
        }

        if (!transition_found)
        {
            cout << "==================== END ====================" << endl;
            cout << "Result: " << tapes[0].get_content_string() << endl;
            return;
        }
        step++;
        print_state();
    }
    cout << "Result: " << tapes[0].get_content_string() << endl;
    cout << "==================== END ====================" << endl;
}