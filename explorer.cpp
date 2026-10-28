// explorer.cpp
#include "explorer.h"
#include "utils.h"

#include <dirent.h>     // opendir, readdir, closedir
#include <sys/stat.h>   // stat, lstat
#include <unistd.h>     // getcwd, chdir, readlink
#include <pwd.h>        // getpwuid  (owner name)
#include <grp.h>        // getgrgid  (group name)
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

namespace {

struct Entry {
    string name;
    struct stat st;
    bool isDir() const { return S_ISDIR(st.st_mode); }
};

bool matchesName(const string& filename, const string& pattern) {
    return filename.find(pattern) != string::npos;
}

void findRecursive(const string& dir, const string& pattern, int& count, int depth) {
    if (depth > 20) return;                       // safety limit
    DIR* d = opendir(dir.c_str());
    if (!d) return;                               // skip unreadable directories silently

    struct dirent* e;
    while ((e = readdir(d)) != nullptr) {
        string name = e->d_name;
        if (name == "." || name == "..") continue;
        string full = (dir == "/" ? "" : dir) + "/" + name;

        if (matchesName(name, pattern)) {
            cout << full << "\n";
            count++;
        }
        struct stat st;
        // lstat does NOT follow symlinks, so we avoid infinite loops
        if (lstat(full.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
            findRecursive(full, pattern, count, depth + 1);
    }
    closedir(d);
}

} // namespace

string Explorer::pwd() const {
    char buf[4096];
    if (getcwd(buf, sizeof(buf)) == nullptr) {
        perror("getcwd");
        return "?";
    }
    return string(buf);
}

bool Explorer::changeDir(const string& rawPath) {
    string path = rawPath;
    if (path.empty() || path == "~") {
        const char* home = getenv("HOME");
        path = home ? home : "/";
    } else if (path[0] == '~' && path.size() > 1 && path[1] == '/') {
        const char* home = getenv("HOME");
        path = string(home ? home : "") + path.substr(1);
    }

    if (chdir(path.c_str()) != 0) {
        cerr << "cd: " << path << ": " << strerror(errno) << "\n";
        return false;
    }
    return true;
}

void Explorer::list(bool showHidden) const {
    DIR* dir = opendir(".");
    if (!dir) {
        cerr << "ls: " << strerror(errno) << "\n";
        return;
    }

    vector<Entry> entries;
    struct dirent* de;
    while ((de = readdir(dir)) != nullptr) {
        string name = de->d_name;
        if (!showHidden && name[0] == '.') continue;

        Entry en;
        en.name = name;
        // lstat: describes the link itself if the entry is a symlink
        if (lstat(name.c_str(), &en.st) != 0) {
            cerr << "ls: cannot stat " << name << ": " << strerror(errno) << "\n";
            continue;
        }
        entries.push_back(en);
    }
    closedir(dir);

    // directories first, then alphabetical
    sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        if (a.isDir() != b.isDir()) return a.isDir();
        return a.name < b.name;
    });

    printf("%-11s %-10s %-17s %s\n", "PERMS", "SIZE", "MODIFIED", "NAME");
    printf("------------------------------------------------------------\n");
    for (const Entry& en : entries) {
        string perms = string(1, typeChar(en.st.st_mode)) + permString(en.st.st_mode);
        string size  = S_ISDIR(en.st.st_mode) ? "-" : sizeString(en.st.st_size);
        string name  = en.name + (en.isDir() ? "/" : "");
        printf("%-11s %-10s %-17s %s\n", perms.c_str(), size.c_str(),
               timeString(en.st.st_mtime).c_str(), name.c_str());
    }
    printf("\n%zu item(s)\n", entries.size());
}

void Explorer::info(const string& path) const {
    struct stat st;
    if (lstat(path.c_str(), &st) != 0) {
        cerr << "info: " << path << ": " << strerror(errno) << "\n";
        return;
    }

    struct passwd* pw = getpwuid(st.st_uid);
    struct group*  gr = getgrgid(st.st_gid);

    cout << "Name        : " << path << "\n";
    cout << "Type        : " << typeChar(st.st_mode) << "\n";
    cout << "Size        : " << st.st_size << " bytes (" << sizeString(st.st_size) << ")\n";
    cout << "Permissions : " << permString(st.st_mode) << "\n";
    cout << "Owner       : " << (pw ? pw->pw_name : to_string(st.st_uid)) << " (uid " << st.st_uid << ")\n";
    cout << "Group       : " << (gr ? gr->gr_name : to_string(st.st_gid)) << " (gid " << st.st_gid << ")\n";
    cout << "Inode       : " << st.st_ino << "\n";
    cout << "Hard links  : " << st.st_nlink << "\n";
    cout << "Blocks      : " << st.st_blocks << " (512-byte units)\n";
    cout << "Accessed    : " << timeString(st.st_atime) << "\n";
    cout << "Modified    : " << timeString(st.st_mtime) << "\n";
    cout << "Status chg  : " << timeString(st.st_ctime) << "\n";

    if (S_ISLNK(st.st_mode)) {
        char target[4096];
        ssize_t n = readlink(path.c_str(), target, sizeof(target) - 1);
        if (n >= 0) {
            target[n] = '\0';
            cout << "Link target : " << target << "\n";
        }
    }
}

void Explorer::find(const string& name) const {
    int count = 0;
    findRecursive(".", name, count, 0);
    cout << "\n" << count << " match(es) found\n";
}
