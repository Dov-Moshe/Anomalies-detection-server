
#include "Server.h"

Server::Server(int port)throw (const char*) {
    fd = socket(AF_INET, SOCK_STREAM,0);
    if(fd < 0)
        throw "socket failed";
    // filling the struct of the server
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    if(bind(fd, (struct sockaddr*)&server, sizeof(server)) < 0)
        throw "bind failure";

    if(listen(fd,3))
        throw "listen failure";
}

void Server::start(ClientHandler& ch)throw(const char*){

    // get client in thread
    t = new thread([&ch, this]() {
        while (true) {
            socklen_t clientSize = sizeof(client);
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
            // accept client
            int nawClient = accept(fd, (struct sockaddr*)&client, &clientSize);

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

