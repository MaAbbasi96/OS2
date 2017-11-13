#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <string>

#define CONNECTINGPORT "4000" // the port client will be connecting to

#define MAXDATASIZE 1024 // max number of bytes we can get at once

using namespace std;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

char* make_info_for_server(const char* port,const char* part_number){
    int i;
    char *info;
    info = (char*) malloc(9 * sizeof(char));
    strcpy(info, port);
    strcat(info, "/");
    strcat(info, part_number);
    return info;
}

int main()
{
    int sockfd, numbytes, file_fd;
    char buf[MAXDATASIZE], info[8], ports[MAXDATASIZE], port[6], file_name[100];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("localhost", CONNECTINGPORT, &hints, &servinfo)) != 0) {
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {

            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);

            continue;
        }

        break;
    }

    if (p == NULL) {

        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    cout << "Connecting to " << s << endl;

    freeaddrinfo(servinfo); // all done with this structure

    string car_tag;
    cout << "Enter car tag: ";
    cin >> car_tag;

    send(sockfd, car_tag.c_str(), car_tag.length(), 0);
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) < 0)
        exit(1);
    buf[numbytes] = '\0';

    cout << "You have to pay " << buf << " tomans to the bloody government :|\n";
    close(sockfd);

    return 0;
}
