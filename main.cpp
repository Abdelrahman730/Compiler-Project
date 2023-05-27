#include <iostream>
#include <regex>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <string>
#include <stdexcept>

std::vector<std::string> datatypes = { "integer", "float", "string", "boolean" };
std::vector<std::string> keywords = { "read", "write", "begin", "end", "declare", "set" };
std::vector<std::string> operators = { "=", "+", "-", "*", "/" };
std::string identifier = "[a-zA-Z][a-zA-Z0-9]*";
std::string number = "[0-9]+(\\.[0-9]+)?";
std::string semicolon = ";";

class Parser {
public:
    Parser(const std::vector<std::pair<std::string, std::string>>& tokens) : tokens(tokens), token_index(0) {}

    void parse() {
        program();
        if (current_token != "EOF") {
            throw std::runtime_error("Unexpected token: " + current_token);
        }
    }

private:
    void advance() {
        token_index++;
        if (token_index < tokens.size()) {
            current_token = tokens[token_index].first;
        }
        else {
            current_token = "EOF";
        }
    }

    void match(const std::string& expected_token) {
        if (current_token == expected_token) {
            advance();
        }
        else {
            throw std::runtime_error("Expected '" + expected_token + "', but got '" + current_token + "'.");
        }
    }

    void program() {
        match("begin");
        statements_block();
        match("end");
        std::cout << "Program Begin" << std::endl;
    }

    void statements_block() {
        while (current_token != "end") {
            statement();
        }
    }

    void statement() {
        if (current_token == "declare") {
            declare_statement();
        }
        else if (current_token == "set") {
            assign_statement();
        }
        else if (current_token == "read") {
            read_statement();
        }
        else if (current_token == "write") {
            write_statement();
        }
        else {
            throw std::runtime_error("Unexpected token: " + current_token);
        }
    }

    void declare_statement() {
        match("declare");
        match("IDENTIFIER");
        match(";");
        std::cout << "Declare statement: declare IDENTIFIER ;" << std::endl;
    }

    void assign_statement() {
        match("set");
        match("IDENTIFIER");
        match("=");
        expression();
        match(";");
        std::cout << "Assign statement: set IDENTIFIER = expression ;" << std::endl;
    }

    void expression() {
        match("IDENTIFIER");
        match("OPERATOR");
        match("IDENTIFIER");
    }

    void read_statement() {
        match("read");
        match("IDENTIFIER");
        match(";");
        std::cout << "Read statement: read IDENTIFIER ;" << std::endl;
    }

    void write_statement() {
        match("write");
        match("IDENTIFIER");
        match(";");
        std::cout << "Write statement: write IDENTIFIER ;" << std::endl;
    }

    const std::vector<std::pair<std::string, std::string>>& tokens;
    std::string current_token;
    int token_index;
};

std::vector<std::pair<std::string, std::string>> tokenize(const std::string& code, const std::string& token_regex) {
    std::vector<std::pair<std::string, std::string>> tokens;
    std::regex regex(token_regex);
    std::sregex_iterator it(code.begin(), code.end(), regex), end;
    while (it != end) {
        std::string token = it->str();
        std::string token_type;
        if (std::find(keywords.begin(), keywords.end(), token) != keywords.end()) {
            token_type = token + " (reserved word)";
        }
        else if (std::find(datatypes.begin(), datatypes.end(), token) != datatypes.end()) {
            token_type = "DATATYPE";
        }
        else if (std::regex_match(token, std::regex(identifier))) {
            token_type = "IDENTIFIER";
        }
        else if (std::regex_match(token, std::regex(number))) {
            token_type = "NUMBER";
        }
        else if (std::regex_match(token, std::regex(semicolon))) {
            token_type = "SEMI-COLON";
        }
        else {
            if (token == " ") {
                ++it;
                continue;
            }
            token_type = "UNKNOWN";
        }
        tokens.emplace_back(token, token_type);
        ++it;
    }
    return tokens;
}

int main() {
    std::string token_regex = "";
    for (const auto& op : operators) {
        token_regex += "\\" + op + "|";
    }
    token_regex += identifier + "|" + number + "|" + semicolon;

    std::ifstream codeFile("code.txt");
    std::string code((std::istreambuf_iterator<char>(codeFile)), std::istreambuf_iterator<char>());
    std::transform(code.begin(), code.end(), code.begin(), [](unsigned char c) { return std::tolower(c); });

    std::vector<std::pair<std::string, std::string>> tokens = tokenize(code, token_regex);
    for (const auto& token : tokens) {
        std::cout << token.first << " : " << token.second << std::endl;
    }

    Parser parser(tokens);
    try {
        parser.parse();
    }
    catch (const std::runtime_error& error) {
        std::cout << "Parser error: " << error.what() << std::endl;
    }

    return 0;
}
