#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

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
std::string simple_string(std::string s){
    return "+"+s+"\r\n";
}

std::string error(std::string msg){
    return "-"+msg+"\r\n";
}

std::string integers(int n){
    return ":" + std::to_string(n)+"\r\n";
}

std::string bulk_string(std::string s){
    if(s.empty()){
        return "$-1\r\n";
    }
    return "$"+std::to_string(s.size())+"\r\n"+s+"\r\n";
}

string encodeBulkString(const string& s) {
    return bulk_string(s);
}

string handleCommand(const vector<string>& args) {
    string cmd = toUpper(args[0]);

    if (cmd == "PING") {
        if(args.size() == 1){
            return simple_string("PONG");
        }else{
            return encodeBulkString(args[1]);
        }
    }else if(cmd == "ECHO" && args.size() == 2){
        //"$<len>\r\n<message>\r\n"
        return encodeBulkString(args[1]);
    }else if(cmd == "COMMAND" && args[1] == "DOCS"){
        return simple_string("OK");
    }

    return error("ERR unknown command '"+args[0]+"'");
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
