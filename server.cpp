#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include "functions.h"

#define BUFSIZE 2048
#define WRITE_FD 1
#define READ_FD 0

using namespace std;

string int_to_str(int x){
    stringstream res;
    res << x;
    return res.str();
}

void pipe_write(int fd, string str){
    write(fd, str.c_str(), str.length());
}

string pipe_read(int fd){
    char buf[BUFSIZE];
    int length;
    length = read(fd, buf, BUFSIZE);
    buf[length] = '\0';
    return string(buf);
}

int sum_of_fine_in_file(int tag, string fileName){
    int totalFine = 0, temp_tag, fine;
    string garbage;
    fstream file;
    file.open(fileName.c_str());
    while(file >> temp_tag >> garbage >> fine)
        if(tag == temp_tag)
            totalFine += fine;
    file.close();
    cout << fileName << ":" << totalFine << endl;
    return totalFine;
}

void calculate_fine(int tag, int parent_write_fd, string dirName){
    int totalFine = 0;
    vector<string> directory;
    string current_address;
    functions::read_directory(dirName, directory);
    int pfds[directory.size()][2];

    //forking
    for(int i = 0; i < directory.size(); i++){
        pipe(pfds[i]);
        current_address = dirName + '/' + directory[i];
        if(!fork()){
            if(functions::is_regular_file(current_address.c_str()) == 1)
                pipe_write(pfds[i][WRITE_FD], int_to_str(sum_of_fine_in_file(tag, current_address)));
            else
                calculate_fine(tag, pfds[i][WRITE_FD], current_address);
            exit(0);
        }
    }

    //terminating processes
    for(int i = 0; i < directory.size(); i++){
        totalFine += atoi(pipe_read(pfds[i][READ_FD]).c_str());
        wait(NULL);
    }
    pipe_write(parent_write_fd, int_to_str(totalFine));
}

int main(){
    string startingDir = "./dir";
    calculate_fine(12345, STDOUT_FILENO, startingDir);
    return 0;
}
