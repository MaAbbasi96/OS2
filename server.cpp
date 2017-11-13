#include "functions.h"

#define PIPE_NAME "./namedpipe"

using namespace std;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

char* get_ms_port(const char* buf){
    int i;
    char *result;
    result = (char*) malloc(6 * sizeof(char));
    for(i = 0; i < strlen(buf); i++){
        if(buf[i] == '/')
            break;
        result[i] = buf[i];
    }
    result[i] = '\0';
    return result;
}

char* get_ms_part_number(const char* buf){
    int i, j = 0;
    char *result;
    result = (char*) malloc(3 * sizeof(char));
    for(i = 0; i < strlen(buf); i++)
        if(buf[i] == '/')
            break;
    i++;
    for(i; i < strlen(buf); i++){
        result[j] = buf[i];
        j++;
    }
    result[j] = '\0';
    j = strlen(result);
    return result;
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
                functions::pipe_write(pfds[i][WRITE_FD], functions::int_to_str(sum_of_fine_in_file(tag, current_address)));
            else
                calculate_fine(tag, pfds[i][WRITE_FD], current_address);
            exit(0);
        }
    }

    //terminating processes
    for(int i = 0; i < directory.size(); i++){
        totalFine += atoi(functions::pipe_read(pfds[i][READ_FD]).c_str());
        wait(NULL);
    }
    functions::pipe_write(parent_write_fd, functions::int_to_str(totalFine));
    for(int i = 0; i < directory.size(); i++){
        close(pfds[i][WRITE_FD]);
        close(pfds[i][READ_FD]);
    }
}

int main(int argc, char* argv[]){
    if(argc != 2){
        cerr << "Where are the files? :|\n";
        exit(1);
    }

    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[1024];    // buffer for client data
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, k, rv;

    char ms_port[6], ms_pn[3];

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

    // create fifo
    mkfifo(PIPE_NAME, 0666);

    // create socket pair
    int sv[2];
    if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sv) < 0) {
       perror("socketpair");
       exit(1);
   }

    // create request handler
    int pipefd_read;
    if(!fork()){
        char buf[BUFSIZE];
        int clientfd;
        close(sv[0]);
        while(1){
            pipefd_read = open(PIPE_NAME, O_RDONLY);
            string result = functions::pipe_read(pipefd_read);
            if(result == "quit")
                exit(0);
            int size = functions::sock_fd_read(sv[1], buf, BUFSIZE, &clientfd);
            send(clientfd, result.c_str(), result.length(), 0);
        }
    }

    close(sv[1]);

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) {
        exit(2);
    }

    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);
    FD_SET(0, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    // main loop
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);

                    if (newfd == -1) {

                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
                        cout << "selectserver: new connection" << endl;
                    }
                } else {
                    // handle data from a client
                    int pipefd = open(PIPE_NAME, O_WRONLY);
                    if(i == 0){
                        string input;
                        cin >> input;
                        if(input == "quit"){
                            functions::pipe_write(pipefd, "quit");
                            close(pipefd);
                            wait(NULL);
                            exit(0);
                        }
                    }
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            cout << "selectserver: connection terminated\n";
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    } else {
                        cout << "Request from car tagged \"" << buf << "\"\n";
                        string startingDir = argv[1];
                        calculate_fine(atoi(buf), pipefd, startingDir);
                        char garbage[2] = {'1', '\0'};
                        int size = functions::sock_fd_write(sv[0], garbage, 1, i);
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
    return 0;
}
