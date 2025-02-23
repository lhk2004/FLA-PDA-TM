#include "pda.h"

bool PDASimulator::run(const string& input)
{
    // initialize PDA
    string current_state = pda_.start_state;
    stack<char> stack;
    stack.push(pda_.start_stack_symbol);
    // initialize input string
    string remaining_input = input;

    while (true)
    {
        // check for acceptance
        if (remaining_input.empty() && pda_.final_states.count(current_state))
        {
            return true;
        }

        bool transition_taken = false;

        // try transitions with the next input symbol
        char current_input_symbol = remaining_input.empty() ? '_' : remaining_input[0];

        // check if the current input symbol is in input alphabet
        if (current_input_symbol != '_' && pda_.alphabet.find(current_input_symbol) == pda_.alphabet.end())
        {
            throw InvalidInputError("Invalid Input Character: " + current_input_symbol);
        }

        char top_stack_symbol = stack.empty() ? '_' : stack.top();

        auto key = make_tuple(current_state, current_input_symbol, top_stack_symbol);
        // check if the transition exists
        if (pda_.transitions.count(key))
        {
            const auto& transition = pda_.transitions.at(key);
            current_state = transition.first;
            if (!stack.empty()) stack.pop();
            for (char symbol : transition.second)
            {
                stack.push(symbol);
            }
            if (!remaining_input.empty())
            {
                remaining_input.erase(0, 1);
            }
            transition_taken = true;
        }

        // try epsilon transitions
        if (!transition_taken)
        {
            key = make_tuple(current_state, '_', top_stack_symbol);
            if (pda_.transitions.count(key))
            {
                const auto& transition = pda_.transitions.at(key);
                current_state = transition.first;
                if (!stack.empty()) stack.pop();
                for (char symbol : transition.second)
                {
                    stack.push(symbol);
                }
                transition_taken = true;
            }
        }

        if (!transition_taken)
        {
            return false; // no valid transition, reject
        }
    }
}

void PDASimulator::run_verbose(const string& input)
{
    // initialize PDA
    string current_state = pda_.start_state;
    stack<char> stack;
    stack.push(pda_.start_stack_symbol);
    string remaining_input = input;
    string processed_input = "";
    int step = 0;

    cout << "==================== PDA RUN VERBOSE ====================" << endl;
    cout << "Input: " << input << endl;

    while (true)
    {
        cout << "Step   : " << step << endl;
        cout << "State  : " << current_state << endl;
        cout << "Input  : " << processed_input << endl;
        cout << "Current: ";
        if (remaining_input.empty())
        {
            cout << "_" << endl;
        }
        else
        {
            cout << remaining_input[0] << endl;
        }
        cout << "Stack  : ";
        if (stack.empty())
        {
            cout << "_" << endl;
        }
        else
        {
            std::stack<char> temp_stack = stack; // use std::stack explicitly here
            string stack_content = "";
            vector<char> stack_vec;
            while (!temp_stack.empty())
            {
                char top = temp_stack.top();
                stack_vec.push_back(top);
                temp_stack.pop();
            }
            for (char symbol : stack_vec)
            {
                stack_content += symbol;
            }
            if (stack_content.empty())
            {
                cout << "_" << endl;
            }
            else
            {
                cout << stack_content << endl;
            }
        }
        cout << "---------------------------------------------" << endl;


        // check for acceptance
        if (remaining_input.empty() && pda_.final_states.count(current_state))
        {
            cout << "Result: true" << endl;
            cout << "==================== PDA END VERBOSE ====================" << endl;
            return;
        }

        bool transition_taken = false;

        // try transitions with the next input symbol
        char current_input_symbol = remaining_input.empty() ? '_' : remaining_input[0];

        // check if the current input symbol is in input alphabet
        if (current_input_symbol != '_' && pda_.alphabet.find(current_input_symbol) == pda_.alphabet.end())
        {
            cerr << "==================== ERR ====================" << endl;
            cerr << "error: '" << current_input_symbol << "' was not declared in the set of input symbols" << endl;
            cerr << "Input: " << input << endl;
            cerr << "Processed Input: " << processed_input << endl;
            cerr << "Current Input Symbol: " << current_input_symbol << endl;
            cerr << "==================== END ====================" << endl;
            exit(1);
        }

        char top_stack_symbol = stack.empty() ? '_' : stack.top();
        auto key = make_tuple(current_state, current_input_symbol, top_stack_symbol);
        // check if the transition exists
        if (pda_.transitions.count(key))
        {
            const auto& transition = pda_.transitions.at(key);
            current_state = transition.first;
            if (!stack.empty()) stack.pop();
            vector<char> push_symbols = transition.second;
            for (char symbol : push_symbols)
            {
                stack.push(symbol);
            }
            if (!remaining_input.empty())
            {
                processed_input += remaining_input[0];
                remaining_input.erase(0, 1);
            }
            transition_taken = true;
        }

        // try epsilon transitions
        if (!transition_taken)
        {
            key = make_tuple(current_state, '_', top_stack_symbol);
            if (pda_.transitions.count(key))
            {
                const auto& transition = pda_.transitions.at(key);
                current_state = transition.first;
                if (!stack.empty()) stack.pop();
                vector<char> push_symbols = transition.second;
                for (char symbol : push_symbols)
                {
                    stack.push(symbol);
                }
                transition_taken = true;
            }
        }

        if (!transition_taken)
        {
            cout << "Result: false" << endl;
            cout << "==================== PDA END VERBOSE ====================" << endl;
            return;
        }
        step++;
    }
}