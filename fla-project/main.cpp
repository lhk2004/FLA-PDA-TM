#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>

#include "parser.h"
#include "pda.h"
#include "tm.h"

using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        cerr << "Usage: fla [-h|--help] <pda_file> <input_string>" << endl;
        cerr << "   or: fla [-v|--verbose] [-h|--help] <tm_file> <input_string>" << endl;
        return 1;
    }

    string automaton_file;
    string input_string;
    bool verbose_mode = false;

    if (string(argv[1]) == "-h" || string(argv[1]) == "--help")
    {
        cout << "Usage: fla [-h|--help] <pda_file> <input_string>" << endl;
        cout << "   or: fla [-v|--verbose] [-h|--help] <tm_file> <input_string>" << endl;
        return 0;
    }
    else if (string(argv[1]) == "-v" || string(argv[1]) == "--verbose")
    {
        verbose_mode = true;
        if (argc < 4)
        {
            cerr << "Usage: fla [-v|--verbose] [-h|--help] <tm_file> <input_string>" << endl;
            return 1;
        }
        if (string(argv[2]) == "-h" || string(argv[2]) == "--help")
        {
            cout << "Usage: fla [-h|--help] <pda_file> <input_string>" << endl;
            cout << "   or: fla [-v|--verbose] [-h|--help] <tm_file> <input_string>" << endl;
            return 0;
        }
        automaton_file = argv[2];
        input_string = argv[3];
    }
    else
    {
        automaton_file = argv[1];
        input_string = argv[2];
    }

    // start parsing and simulating
    try
    {
        if (automaton_file.rfind(".pda", automaton_file.length() - 4) != string::npos)
        {
            // it's a PDA file
            PDAParser parser;
            PDA pda = parser.parseFile(automaton_file);
            PDASimulator simulator(pda);
            if (verbose_mode)
            {
                simulator.run_verbose(input_string);
            }
            else
            {
                if (simulator.run(input_string))
                {
                    cout << "true" << endl;
                }
                else
                {
                    cout << "false" << endl;
                }
            }
        }
        else if (automaton_file.rfind(".tm", automaton_file.length() - 3) != string::npos)
        {
            // it's a TM file
            TMParser parser;
            TM tm = parser.parseFile(automaton_file);
            TMSimulator simulator(tm);
            if (verbose_mode)
            {
                simulator.run_verbose(input_string);
            }
            else
            {
                simulator.run(input_string);
            }
        }
        else
        {
            cerr << "Error: Invalid file extension. Expected .pda or .tm" << endl;
            return 1;
        }
    }
    catch (const ParseError& e)
    {
        // cerr << "Parse error: " << e.what() << endl;
        cerr << "syntax error" << endl;
        return 1;
    }
    catch (const InvalidInputError& e)
    {
        // cerr << "Invalid input: " << e.what() << endl;
        cerr << "illegal input" << endl;
        return 1;
    }
    catch (const runtime_error& e)
    {
        // cerr << "Error: " << e.what() << endl;
        cerr << "syntax error" << endl;
        return 1;
    }

    return 0;
}