// utils.cpp
#include "utils.h"
#include <cstdio>

char typeChar(mode_t mode) {
    if (S_ISDIR(mode))  return 'd';
    if (S_ISLNK(mode))  return 'l';
    if (S_ISREG(mode))  return '-';
    if (S_ISCHR(mode))  return 'c';
    if (S_ISBLK(mode))  return 'b';
    if (S_ISFIFO(mode)) return 'p';
    if (S_ISSOCK(mode)) return 's';
    return '?';
}

// st_mode stores permissions as bits. We test each bit with a bitwise AND.
std::string permString(mode_t mode) {
    std::string p(9, '-');
    if (mode & S_IRUSR) p[0] = 'r';
    if (mode & S_IWUSR) p[1] = 'w';
    if (mode & S_IXUSR) p[2] = 'x';
    if (mode & S_IRGRP) p[3] = 'r';
    if (mode & S_IWGRP) p[4] = 'w';
    if (mode & S_IXGRP) p[5] = 'x';
    if (mode & S_IROTH) p[6] = 'r';
    if (mode & S_IWOTH) p[7] = 'w';
    if (mode & S_IXOTH) p[8] = 'x';
    return p;
}

std::string timeString(time_t t) {
    char buf[32];
    struct tm* tmv = localtime(&t);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", tmv);
    return std::string(buf);
}

std::string sizeString(off_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    double size = static_cast<double>(bytes);
    int u = 0;
    while (size >= 1024.0 && u < 4) { size /= 1024.0; u++; }
    char buf[32];
    if (u == 0) snprintf(buf, sizeof(buf), "%lld %s", static_cast<long long>(bytes), units[u]);
    else        snprintf(buf, sizeof(buf), "%.1f %s", size, units[u]);
    return std::string(buf);
}
