#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

const string CONSOLE_ARGUMENTS_ERROR = "Error! No output option specified.\n";
const string MANDATORY_PARAMETER_ERROR = "Error! Required parameter value not specified.\n";

class Parameters {
public:
    Parameters(string file_path) {
        ifstream input(file_path);
        string current_parameter;
        while (getline(input, current_parameter))
            array_parameters.emplace_back(current_parameter);
    };

    string get_parameter(int index) { return array_parameters[index]; }

private:
    vector<string> array_parameters;
};

int main(int argc, char *argv[]) {

    bool output_to_file;
    if (argc > 1)
        output_to_file = (argv[1][1] == 'f');
    else {
        cout << CONSOLE_ARGUMENTS_ERROR;
        return 0;
    }

    Parameters parameters("..\\parameters.txt");


    ifstream input_template("..\\template.txt");
    ofstream output_file("output.txt");
    string current_string;
    int count_parameters = 0;
    bool flag = true;
    while (getline(input_template, current_string)) {

        for (int i = 0; i < current_string.size(); i++) {
            if (current_string[i] == '{') {
                if (current_string[i + 1] == '*' && parameters.get_parameter(count_parameters).empty()) {
                    cout << MANDATORY_PARAMETER_ERROR;
                    return 0;
                }
                flag = false;
                output_file << parameters.get_parameter(count_parameters);
                count_parameters++;
            } else if (current_string[i] == '}')
                flag = true;
            else if (flag) {
                output_file << current_string[i];
            }
        }

        output_file << '\n';

    }
    output_file.close();
    if (!output_to_file) {
        ifstream input_file("C:\\Users\\raslb\\CLionProjects\\ITMO\\C++Sect\\Intern\\cmake-build-debug\\output.txt");
        string output_to_console;
        while (getline(input_file, output_to_console))
            cout << output_to_console << '\n';
    }

    return 0;
}