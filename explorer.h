// explorer.h - directory navigation and listing
#ifndef EXPLORER_H
#define EXPLORER_H

#include <string>

class Explorer {
public:
    std::string pwd() const;                         // current working directory
    bool changeDir(const std::string& path);         // cd
    void list(bool showHidden = false) const;        // ls
    void info(const std::string& path) const;        // detailed stat() output
    void find(const std::string& name) const;        // recursive search by name
};

#endif
