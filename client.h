#pragma once

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <string>


#include "presets.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

class Client {
public:
    Client(string host, int port);
    ~Client();

    void run();
    std::string send_preset(json j) ;
    virtual void create();
    virtual void close_socket();
    void echo();
    bool get_response();
    
private:
    bool send_request(string);
    
    string host_;
    int port_;
    int server_;
    int buflen_;
    char* buf_;
};
