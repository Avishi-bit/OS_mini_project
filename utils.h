// utils.h - small helpers that turn raw OS metadata into readable text
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sys/stat.h>
#include <ctime>

char typeChar(mode_t mode);               // 'd' directory, 'l' symlink, '-' regular file, ...
std::string permString(mode_t mode);      // e.g. "rwxr-xr--"
std::string timeString(time_t t);         // e.g. "2026-10-28 14:32"
std::string sizeString(off_t bytes);      // e.g. "1.5 KB"

#endif
