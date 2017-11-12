#include "functions.h"

void functions::read_directory(const std::string& dirName, std::vector<std::string>& directories){
    DIR* dirPointer = opendir(dirName.c_str());
    struct dirent* dp;
    while((dp = readdir(dirPointer)) != NULL)
        if(dp->d_name[0] != '.')
            directories.push_back(dp->d_name);
    closedir(dirPointer);
}

int functions::is_regular_file(const char* path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void functions::print_vector(const std::vector<std::string>& vec){
    for(int i = 0; i < vec.size(); i++)
        std::cout << vec[i] << std::endl;
}
