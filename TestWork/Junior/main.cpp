#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <unordered_map>
#include <unordered_set>

using namespace std;

const string CONSOLE_ARGUMENTS_ERROR = "Error! No output option specified.\n";
const string MANDATORY_PARAMETER_ERROR = "Error! Required parameter value not specified.\n";
const string FUNCTIONAL_PARAMETER_ERROR = "Error! Functional parameter not set.\n";
const string FUNCTIONAL_PARAMETER_IN_JSON_ERROR = "Error! Functional parameter found in json.\n";
unordered_set<string> functional_parameter = {"current_date"};

class Json {
public:
    Json(string file_path);

    vector<pair<string, string>> get_array() { return array; }

private:
    vector<pair<string, string>> array;
};

class Parameters {
public:
    Parameters(string file_path) {
        Json json(file_path);
        for (auto i : json.get_array()) {
            if (functional_parameter.find(i.first) != functional_parameter.end()) {
                cout << FUNCTIONAL_PARAMETER_IN_JSON_ERROR;
                flag = true;
                return;
            }
            parameters_map.emplace(i);
        }
    }

    bool get_flag() { return flag; }

    pair<bool, string> get_parameter(string key) {
        if (parameters_map.find(key) != parameters_map.end())
            return make_pair(true, parameters_map[key]);
        else
            return make_pair(false, "");
    }

private:
    unordered_map<string, string> parameters_map;
    bool flag = false;
};

class Template {
public:
    Template(string file_path_template, string file_path_parameters) :
            file_path_parameters(file_path_parameters),
            file_path_template(file_path_template) {}

    void print_to_console();

    bool print_to_file();


private:
    string file_path_template;
    string file_path_parameters;

    string current_date();
};

Json::Json(string file_path) {
    ifstream input(file_path);
    char current_char = input.get();
    int count = 0;
    bool shielding = false;
    while (current_char != EOF) {
        if (current_char == '"' && shielding == false) {
            string str = "";
            current_char = input.get();
            while (!(current_char == '"' && shielding == false)) {
                if (current_char == '\\') {
                    shielding = true;
                } else if (shielding) {
                    shielding = false;
                }
                if (!shielding)
                    str += current_char;
                current_char = input.get();
            }
            if (count % 2 == 0)
                array.emplace_back(pair<string, string>(str, ""));
            else
                array.back().second = str;
            count++;
        } else if (current_char == '\\') {
            shielding = true;
        } else if (shielding) {
            shielding = false;
        }
        current_char = input.get();
    }
}

bool Template::print_to_file() {
    ifstream input(file_path_template);
    ofstream output("output.txt");

    Parameters parameters(file_path_parameters);
    if (parameters.get_flag())
        return false;

    char current_char = input.get();
    while (current_char != EOF) {
        if (current_char == '{') {
            string str = "";
            current_char = input.get();
            while (current_char != '}') {
                str += current_char;
                current_char = input.get();
            }
            bool mandatory = !str.empty() && str[0] == '*';
            if (mandatory)
                str = str.substr(1, str.size() - 1);
            if (parameters.get_parameter(str).first) {
                if (mandatory && parameters.get_parameter(str).second == "") {
                    cout << MANDATORY_PARAMETER_ERROR;
                    return false;
                }
                output << parameters.get_parameter(str).second;
            } else {
                if (str == "current_date") {
                    output << current_date();
                } else {
                    cout << FUNCTIONAL_PARAMETER_ERROR;
                    return false;
                }
            }
        } else
            output.put(current_char);
        current_char = input.get();
    }
    return true;
}

void Template::print_to_console() {
    if (!print_to_file())
        return;
    ifstream input("output.txt");
    string output_to_console;
    while (getline(input, output_to_console))
        cout << output_to_console << '\n';
}

string Template::current_date() {
    time_t seconds = time(NULL);
    tm *timeinfo = localtime(&seconds);
    string date = asctime(timeinfo);
    date.pop_back();
    return date;
}

int main(int argc, char *argv[]) {

    bool output_to_file = true;
    if (argc > 1)
        output_to_file = (argv[1][1] == 'f');
    else {
        cout << CONSOLE_ARGUMENTS_ERROR;
        return 0;
    }

    string path_parameters = "..\\parameters.json";
    string path_template = "..\\template.txt";

    Template temp(path_template, path_parameters);

    if (output_to_file)
        temp.print_to_file();
    else
        temp.print_to_console();

    return 0;
}
