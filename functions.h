#ifndef FUNCTIOS_H
#define FUNCTIOS_H

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include  <iostream>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <string>
#include <sys/wait.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>


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
    ssize_t sock_fd_write(int sock, void *buf, ssize_t buflen, int fd);
    ssize_t sock_fd_read(int sock, void *buf, ssize_t bufsize, int *fd);
}

#endif
