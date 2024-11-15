#pragma once

#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <string>
#include <gtk/gtk.h>
#include "log.h"
#include "json.hpp"
#include "presets.h"

using namespace std;

class Server {
public:
    Server();
    ~Server();

    Presets * presets ;
    void * sync ;
    void run();
    
    GMainContext * context ;
    void create();
    void close_socket();
    void serve();
    void handle(int);
    string get_request(int);
    bool send_response(int, string);

    int port_;
    int server_;
    int buflen_;
    char* buf_;
};

