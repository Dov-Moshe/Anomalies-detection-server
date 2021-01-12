/*
 * Server.h
 *
 *  Created on: Dec 13, 2020
 *      Author: Eli
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <pthread.h>
#include <sstream>
#include <string>
#include "CLI.h"
#include "commands.h"

using namespace std;

// edit your ClientHandler interface here:
class ClientHandler{
    public:
    virtual void handle(int clientID)=0;
};


// you can add helper classes here and implement on the cpp file

// this class is for reading from socket line by line
class GetSocket {
public:
    string get(int clientID) {
        char newChar[2];
        string line = "";
        while (read(clientID, newChar, 1) > 0) {
            if(newChar[0] == '\n')
                break;
            line += *newChar;
        }
        return line;
    }
};

// IO for socket
class SocketIO: public DefaultIO {
    int clientID;
    GetSocket get;
public:
    SocketIO(int clientID) {
        SocketIO::clientID = clientID;
    }
    virtual string read() {
        string line = get.get(clientID);
        return line;
    }
    virtual void write(string text) {
        const char* line = text.c_str();
        send(clientID, line, strlen(line), 0);
    }
    virtual void write(float f) {
        stringstream s;
        s << f;
        string num = s.str();
        send(clientID, num.c_str(), strlen(num.c_str()), 0);
    }
    virtual void read(float* f) {
        string line = get.get(clientID);
        *f = stof(line);
    }
};


// edit your AnomalyDetectionHandler class here
class AnomalyDetectionHandler:public ClientHandler{
	public:
    virtual void handle(int clientID){
        SocketIO std(clientID);
        CLI cli(&std);
        cli.start();
    }
};


// implement on Server.cpp
class Server {
	thread* t; // the thread to run the start() method in
	// you may add data members
	// file descriptor of the server
    int fd;
    // data of
    sockaddr_in server;
    sockaddr_in client;

public:
	Server(int port) throw (const char*);
	virtual ~Server();
	void start(ClientHandler& ch)throw(const char*);
	void stop();
};

#endif /* SERVER_H_ */
