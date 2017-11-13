#include "functions.h"

void functions::read_directory(const std::string& dirName, std::vector<std::string>& directories){
    DIR* dirPointer = opendir(dirName.c_str());
    struct dirent* dp;
    while((dp = readdir(dirPointer)) != NULL)
        if(dp->d_name[0] != '.')
            directories.push_back(dp->d_name);
    closedir(dirPointer);
}

int functions::is_regular_file(const char* path){
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

void functions::print_vector(const std::vector<std::string>& vec){
    for(int i = 0; i < vec.size(); i++)
        std::cout << vec[i] << std::endl;
}

std::string functions::int_to_str(int x){
    std::stringstream res;
    res << x;
    return res.str();
}

void functions::pipe_write(int fd, std::string str){
    write(fd, str.c_str(), str.length());
}

std::string functions::pipe_read(int fd){
    char buf[BUFSIZE];
    int length;
    length = read(fd, buf, BUFSIZE);
    buf[length] = '\0';
    return std::string(buf);
}

ssize_t functions::sock_fd_write(int sock, void *buf, ssize_t buflen, int fd){
    ssize_t     size;
    struct msghdr   msg;
    struct iovec    iov;
    union {
        struct cmsghdr  cmsghdr;
        char        control[CMSG_SPACE(sizeof (int))];
    } cmsgu;
    struct cmsghdr  *cmsg;

    iov.iov_base = buf;
    iov.iov_len = buflen;

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    if (fd != -1) {
        msg.msg_control = cmsgu.control;
        msg.msg_controllen = sizeof(cmsgu.control);

        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_len = CMSG_LEN(sizeof (int));
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;

        *((int *) CMSG_DATA(cmsg)) = fd;
    } else {
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
    }

    size = sendmsg(sock, &msg, 0);

    if (size < 0)
    return size;
}

ssize_t functions::sock_fd_read(int sock, void *buf, ssize_t bufsize, int *fd){
    ssize_t     size;

    if (fd) {
        struct msghdr   msg;
        struct iovec    iov;
        union {
            struct cmsghdr  cmsghdr;
            char        control[CMSG_SPACE(sizeof (int))];
        } cmsgu;
        struct cmsghdr  *cmsg;

        iov.iov_base = buf;
        iov.iov_len = bufsize;

        msg.msg_name = NULL;
        msg.msg_namelen = 0;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = cmsgu.control;
        msg.msg_controllen = sizeof(cmsgu.control);
        size = recvmsg (sock, &msg, 0);
        if (size < 0)
            exit(1);
        cmsg = CMSG_FIRSTHDR(&msg);
        if (cmsg && cmsg->cmsg_len == CMSG_LEN(sizeof(int))) {
            if (cmsg->cmsg_level != SOL_SOCKET)
                exit(1);
            if (cmsg->cmsg_type != SCM_RIGHTS)
                exit(1);

            *fd = *((int *) CMSG_DATA(cmsg));
        } else
            *fd = -1;
    } else {
        size = read (sock, buf, bufsize);
        if (size < 0)
            exit(1);
    }
    return size;
}
