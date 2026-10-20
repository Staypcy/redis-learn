#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include<map>

using namespace std;

std::map<std::string,std::vector<int>>ARITY = {
    {"PING", {0, 1}},
    {"ECHO", {1, 1}},
    {"COMMAND",{1,1}}
};

std::string check_arity(const std::string& cmd, const vector<string>& args){
    auto it = ARITY.find(cmd);
    if (it == ARITY.end()) {
        return "ERR unknown command '" + cmd + "'";
    }
    const auto& p = it->second;
    if (args.size() - 1 >= p[0] && args.size() - 1 <= p[1]) {
        return {};
    }
    return "ERR wrong number of arguments for '" + cmd + "' command";
}

vector<string> parseArgs(const string& line) {
    vector<string> args;
    string current;
    bool inQuotes = false;
    for (char ch : line) {
        if (ch == '"' && !inQuotes) { inQuotes = true; }
        else if (ch == '"' && inQuotes) { inQuotes = false; }
        else if (ch == ' ' && !inQuotes) {
            if (!current.empty()) { args.push_back(current); current.clear(); }
        } else { current += ch; }
    }
    if (!current.empty()) args.push_back(current);
    return args;
}

string toUpper(string s) {
    for (auto& c : s) c = toupper(c);
    return s;
}

// RESP 格式序列化器
std::string simple_string(const std::string& s){
    return "+"+s+"\r\n";
}

std::string error(const std::string& msg){
    return "-"+msg+"\r\n";
}

std::string integers(int n){
    return ":" + std::to_string(n)+"\r\n";
}

std::string bulk_string(const std::string& s){
    if(s.empty()){
        return "$-1\r\n";
    }
    return "$"+std::to_string(s.size())+"\r\n"+s+"\r\n";
}

string encodeBulkString(const string& s) {
    return bulk_string(s);
}

string handleCommand(const vector<string>& args) {
    if (args.empty()) return {};

    string cmd = toUpper(args[0]);
    std::string check = check_arity(cmd,args);

    if (cmd == "PING") {

        if(!check.empty()) return error(check);

        if(args.size() == 1) return simple_string("PONG");

        else if(args.size() == 2) return encodeBulkString(args[1]);

    }else if(cmd == "ECHO"){

        if(!check.empty()) return error(check);

        return encodeBulkString(args[1]);
    
    }else if(cmd == "COMMAND"){

        if(!check.empty()) return error(check);
        
        if(args[1] == "DOCS") return simple_string("OK");

        return error("ERR unknow command argument of '"+ cmd + " '");

    }
    return error("ERR unknown command '"+cmd+"'");
}

int main() {
    string line;
    while (getline(cin, line)) {
        // trim trailing \r
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        auto args = parseArgs(line);
        cout << handleCommand(args);
        cout.flush();
    }
    return 0;
}
