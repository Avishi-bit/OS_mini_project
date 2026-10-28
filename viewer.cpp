// viewer.cpp - uses raw POSIX system calls: open(), read(), close()
#include "viewer.h"

#include <fcntl.h>      // open
#include <unistd.h>     // read, close
#include <sys/stat.h>   // stat
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

static const int LINES_PER_PAGE = 20;

void viewFile(const string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        cerr << "view: " << path << ": " << strerror(errno) << "\n";
        return;
    }
    if (S_ISDIR(st.st_mode)) {
        cerr << "view: " << path << " is a directory (use cd)\n";
        return;
    }
    if (!S_ISREG(st.st_mode)) {
        cerr << "view: " << path << " is not a regular file\n";
        return;
    }

    // System call #1: open() returns a file descriptor (a small integer)
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        cerr << "view: cannot open " << path << ": " << strerror(errno) << "\n";
        return;
    }

    char buf[4096];
    ssize_t n;
    int lines = 0;
    bool firstChunk = true;

    // System call #2: read() copies bytes from the kernel into our buffer
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        // A null byte means the file is almost certainly binary, not text
        if (firstChunk) {
            firstChunk = false;
            if (memchr(buf, '\0', n) != nullptr) {
                cerr << "view: " << path << " looks like a binary file, not showing it\n";
                close(fd);
                return;
            }
        }
        for (ssize_t i = 0; i < n; i++) {
            cout << buf[i];
            if (buf[i] == '\n' && ++lines == LINES_PER_PAGE) {
                cout << "-- More -- (Enter = next page, q = quit): " << flush;
                string reply;
                getline(cin, reply);
                if (!reply.empty() && (reply[0] == 'q' || reply[0] == 'Q')) {
                    cout << "\n";
                    close(fd);
                    return;
                }
                lines = 0;
            }
        }
    }
    if (n < 0) cerr << "view: read error: " << strerror(errno) << "\n";
    cout << flush;

    // System call #3: close() releases the file descriptor
    close(fd);
}
