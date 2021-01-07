
#include "Server.h"

Server::Server(int port)throw (const char*) {
    fd = socket(AF_INET, SOCK_STREAM,0);
    if(fd < 0)
        throw "socket failed";

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    if(bind(fd, (struct sockaddr*)&server, sizeof(server)) < 0)
        throw "bind failure";

    if(listen(fd,3))
        throw "listen failure";
}

void Server::start(ClientHandler& ch)throw(const char*){
    t = new thread([&ch, this]() {
        while (true) {
            socklen_t clientSize = sizeof(client);
            int nawClient;

            /************* for timeout ************/
            fd_set selectFd;
            timeval timeout;
            // time of timeout
            timeout.tv_sec = 0;
            timeout.tv_usec = 100000;
            // set the file descriptor
            FD_ZERO(&selectFd);
            FD_SET(fd, &selectFd);
            // if the return value is 0. then timeout occurs
            if(select(32, &selectFd,0,0,&timeout) == 0)
                break;
            /*************************************/

            nawClient = accept(fd, (struct sockaddr*)&client, &clientSize);

            if (nawClient < 0)
                throw "accept failure";

            ch.handle(nawClient);
            close(nawClient);
        }
        close(fd);
    });
}

void Server::stop(){
	t->join(); // do not delete this!
}

Server::~Server() {
}

