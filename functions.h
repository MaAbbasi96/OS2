#ifndef FUNCTIOS_H
#define FUNCTIOS_H

#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <sstream>

#define BUFSIZE 2048
#define WRITE_FD 1
#define READ_FD 0

namespace functions{
    void read_directory(const std::string& dirName, std::vector<std::string>& directories);
    int is_regular_file(const char* path);
    void print_vector(const std::vector<std::string>& vec);
    std::string int_to_str(int x);
    void pipe_write(int fd, std::string str);
    std::string pipe_read(int fd);
}

#endif
