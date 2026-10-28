# Simple File Explorer (C++ / POSIX)

A terminal-based file explorer written in C++ for an Operating Systems course project. It lets you navigate directories, inspect file metadata, search for files, and view file contents, using POSIX system calls directly instead of high-level wrappers.

**Author:** [Your Name]
**Course:** [Course Name and Code]
**Institution:** [Your College]

## Features

- List directory contents with type, permissions, size, and last modified time
- Navigate with `cd` (absolute paths, relative paths, `..`, and `~`)
- View text files page by page (20 lines per page)
- Show detailed file metadata: inode, owner, group, hard links, blocks, and timestamps
- Recursive search by file name
- Detects and refuses to display binary files
- Clear error messages taken from the OS (`Permission denied`, `No such file or directory`, ...)
- Handles paths that contain spaces

## Requirements

- Linux, macOS, or Windows with WSL
- `g++` (C++11 or later) and `make`

On Ubuntu or WSL: `sudo apt install build-essential`

> The program relies on POSIX APIs (`dirent.h`, `unistd.h`, `sys/stat.h`), so it will not compile with plain MinGW or MSVC on Windows. Use WSL.

## Build and Run

```bash
make
./explorer
```

To clean build files:

```bash
make clean
```

Manual compile, if `make` is unavailable:

```bash
g++ -std=c++11 -Wall -Wextra -o explorer main.cpp explorer.cpp viewer.cpp utils.cpp
```

## Commands

| Command | Description |
|---|---|
| `ls` | List the current directory |
| `ls -a` | List including hidden files |
| `cd <path>` | Change directory (`cd ..` goes up, `cd ~` goes home) |
| `pwd` | Print the current directory |
| `view <file>` | Show file contents, one page at a time (Enter = next page, `q` = quit) |
| `info <path>` | Show detailed metadata for a file or directory |
| `find <text>` | Search recursively for names containing `<text>` |
| `help` | Show the command list |
| `exit` | Quit |

## Example Session

```
[/home/user/project]$ ls
PERMS       SIZE       MODIFIED          NAME
------------------------------------------------------------
drwxr-xr-x  -          2026-10-28 08:55  sub/
-rw-r--r--  6 B        2026-10-28 08:55  a.txt

2 item(s)

[/home/user/project]$ info a.txt
Name        : a.txt
Type        : -
Size        : 6 bytes (6 B)
Permissions : rw-r--r--
Owner       : user (uid 1000)
...
```

## Project Structure

```
.
├── main.cpp        # Command loop: read, parse, dispatch
├── explorer.h/.cpp # Explorer class: ls, cd, pwd, info, find
├── viewer.h/.cpp   # viewFile(): paged file viewer using open/read/close
├── utils.h/.cpp    # Helpers: permission string, time and size formatting
└── Makefile
```

## OS Concepts and System Calls Used

| Feature | System / library calls | OS concept |
|---|---|---|
| List directory | `opendir`, `readdir`, `closedir` | A directory is a file mapping names to inodes |
| File metadata | `lstat`, `stat` | The inode stores size, type, permissions, owner, and timestamps |
| Permissions | bit tests on `st_mode` (`S_IRUSR`, `S_IWGRP`, ...) | Unix owner/group/other permission model |
| Navigation | `chdir`, `getcwd` | Each process has its own working directory |
| View file | `open`, `read`, `close` | File descriptors; user mode to kernel mode transitions |
| Owner names | `getpwuid`, `getgrgid` | User and group IDs resolved through the system databases |
| Symlinks | `lstat`, `readlink` | Symbolic links are separate files that store a path |
| Search | recursion with `lstat` | Directory tree traversal; symlinks are not followed to avoid loops |
| Errors | `errno`, `strerror` | System calls report failure through `errno` |

## Testing

Cases covered during development:

| Test | Expected result |
|---|---|
| `cd nope` | `No such file or directory` error, directory unchanged |
| `view <directory>` | Error saying it is a directory |
| `view <binary file>` | Refuses and reports it as binary |
| `view` on a file longer than 20 lines | Paged output, `q` quits early |
| `cd` into a folder with no permission | `Permission denied` error |
| Names with spaces (`cd my folder`) | Works |
| `find` with no matches | `0 match(es) found` |

## Limitations and Future Work

- No file operations yet (copy, move, delete, create)
- Command-line interface only, with no GUI
- Search matches substrings only, with no wildcards or regex
- No sorting options beyond directories first, then alphabetical

## License

For educational use.
