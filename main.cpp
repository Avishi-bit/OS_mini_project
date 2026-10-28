// main.cpp - the command loop (read command -> parse -> call the right function)
#include "explorer.h"
#include "viewer.h"

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

static void printHelp() {
    cout << "Commands:\n"
         << "  ls            list current directory\n"
         << "  ls -a         list including hidden files\n"
         << "  cd <path>     change directory (cd .. goes up, cd ~ goes home)\n"
         << "  pwd           show current directory\n"
         << "  view <file>   show file contents, page by page\n"
         << "  info <path>   show detailed metadata (inode, owner, times...)\n"
         << "  find <text>   search recursively for names containing <text>\n"
         << "  help          show this message\n"
         << "  exit          quit\n";
}

// Remove spaces at both ends so "  cd   my folder  " works
static string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t");
    return s.substr(a, b - a + 1);
}

int main() {
    Explorer ex;
    cout << "Simple File Explorer (type 'help' for commands)\n";

    string line;
    while (true) {
        cout << "\n[" << ex.pwd() << "]$ " << flush;
        if (!getline(cin, line)) break;           // Ctrl+D ends the program
        line = trim(line);
        if (line.empty()) continue;

        // split into command (first word) and argument (everything after it)
        size_t sp = line.find(' ');
        string cmd = (sp == string::npos) ? line : line.substr(0, sp);
        string arg = (sp == string::npos) ? ""   : trim(line.substr(sp + 1));

        if (cmd == "ls") {
            ex.list(arg == "-a");
        } else if (cmd == "cd") {
            ex.changeDir(arg);
        } else if (cmd == "pwd") {
            cout << ex.pwd() << "\n";
        } else if (cmd == "view") {
            if (arg.empty()) cerr << "usage: view <file>\n";
            else viewFile(arg);
        } else if (cmd == "info") {
            if (arg.empty()) cerr << "usage: info <path>\n";
            else ex.info(arg);
        } else if (cmd == "find") {
            if (arg.empty()) cerr << "usage: find <text>\n";
            else ex.find(arg);
        } else if (cmd == "help") {
            printHelp();
        } else if (cmd == "exit" || cmd == "quit") {
            break;
        } else {
            cerr << "Unknown command: " << cmd << " (type 'help')\n";
        }
    }
    cout << "Goodbye!\n";
    return 0;
}
