#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fstream>
#include "functions.h"

using namespace std;

void calculate_fine(int tag, int& totalFine,const string& dirName){
    vector<string> directory;
    functions::read_directory(dirName, directory);
    int pfds[directory.size()][2];
    for(int i = 0; i < directory.size(); i++){
        if(!fork()){
            //child process
        }
        else{
            //parent process
        }
    }
}

int main(){
    int totalFine = 0;
    string startingDir = "./dir";
    calculate_fine(12345, totalFine, startingDir);
    cout << totalFine << endl;
    return 0;
}
