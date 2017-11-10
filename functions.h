#ifndef FUNCTIOS_H
#define FUNCTIOS_H

#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>

void read_directory(const std::string& dirName, std::vector<std::string>& directories);
int is_regular_file(const char* path);

#endif
