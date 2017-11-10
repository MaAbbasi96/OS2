#include "functions.h"

void read_directory(const std::string& dirName, std::vector<std::string>& directories){
    DIR* dirPointer = opendir(dirName.c_str());
    struct dirent* dp;
    while((dp = readdir(dirPointer)) != NULL)
        if(dp->d_name != "." && dp->d_name != "..")
            directories.push_back(dp->d_name);
    closedir(dirPointer);
}

int is_regular_file(const char* path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}
