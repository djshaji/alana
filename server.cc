#ifdef __linux__
#include "sync.h"

#include "server.h"

Server::Server() {
    IN
    // setup variables
    int port = 6906 ;
    port_ = port;
    buflen_ = 1024;
    buf_ = new char[buflen_+1];
    context = g_main_context_get_thread_default ();
    OUT
}

Server::~Server() {
    delete buf_;
}

void
Server::run() {
    IN
    // create and run the server
    create();
    //~ std::thread t (&Server::serve, this);
    serve();
    OUT
}

void
Server::create() {
    IN
    struct sockaddr_in server_addr;

    // setup socket address structure
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // create socket
    server_ = socket(PF_INET,SOCK_STREAM,0);
    if (!server_) {
        perror("socket");
        exit(-1);
    }

    // set socket to immediately reuse port when the application closes
    int reuse = 1;
    if (setsockopt(server_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt");
        exit(-1);
    }

    // call bind to associate the socket with our local address and
    // port
    if (bind(server_,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
        perror("bind");
        exit(-1);
    }

    // convert the socket to listen for incoming connections
    if (listen(server_,SOMAXCONN) < 0) {
        perror("listen");
        exit(-1);
    }
}

void
Server::close_socket() {
    IN
    shutdown(server_, SHUT_RD);
    close(server_);
    OUT
}

void
Server::serve() {
    IN
    // setup client
    int client;
    struct sockaddr_in client_addr;
    socklen_t clientlen = sizeof(client_addr);
    while (g_main_context_pending (context))
        g_main_context_iteration (context, true);

      // accept clients
    while ((client = accept(server_,(struct sockaddr *)&client_addr,&clientlen)) > 0) {
        while (g_main_context_pending (context))
            g_main_context_iteration (context, true);

        handle(client);
    }
    
    OUT
    close_socket();
}

void
Server::handle(int client) {
    IN
    // loop to handle all requests
    json toSend = presets->get_all_user_presets ();
    
    while (1) {
        // get a request
        string request = get_request(client);
        // break if client is done or an error occurred
        if (request.empty()) {
            LOGD ("[server] empty response recieved from client\n");
            //~ break;
        }
        // send response
        else {
            LOGD ("[server] request: %s\n", request.c_str ());
            while (request.find ("}}}") != string::npos)
                request.pop_back () ; ///| aargh!
            //~ LOGD ("[pop] request: %s\n", request.c_str ());
            json j = json::parse (request);
            Sync * _sync = (Sync *) sync ;
            /*
            if (! j.contains ("key") || j ["key"].get <int>() != _sync -> sec_key) {
                LOGD ("[key] %d %d\n", j ["key"].get <int>(), _sync -> sec_key);
                gtk_label_set_markup (_sync -> status, "Security key mismatch");
                return ;
            }
            */
            
            int how_many = presets->import_presets_from_json (j);
            char * ss = g_strdup_printf ("<span foreground=\"green\" weight=\"bold\" size=\"x-large\">Imported %d presets successfully</span>", how_many);
            gtk_label_set_markup (_sync -> header, ss);
            g_free (ss);
            
            LOGD ("[server] synced %d presets\n", how_many);
        }
        
        std::string toSendStr = toSend.dump () ;
        if (toSend.empty ()) {
            toSendStr = std::string ("{}");
        }
        
        bool success = send_response(client,toSendStr);
        // break if an error occurred
        if (not success) {
            HERE LOGD ("break if an error occurred\n");
            break;
        }
    }
    
    OUT
    close(client);
}

string
Server::get_request(int client) {
    IN
    string request = "";
    // read until we get a newline
    int count = -1 ;
    while (request.find("}}}") == string::npos) {
        count ++ ;
        
        int nread = recv(client,buf_,1024,0);
        if (nread < 0) {
            if (errno == EINTR) {
                LOGD ("the socket call was interrupted -- try again\n");
                continue;
            }
            else {
                HERE LOGD ("[%s]  an error occurred, so break out\n", strerror (errno));
                return std::string ("");
            }
        } else if (nread == 0) {
            LOGD ("the socket is closed\n");
            return std::string ("");
        }

        // be sure to use append in case we have binary data
        HERE LOGD ("[request %d:%d] %s\n", count, nread, buf_);
        request.append(buf_,nread);
        if (request.c_str ()[1] == '}')
            return std::string ("");
    }
    
    LOGD ("[server] read ended: \n");
    // a better server would cut off anything after the newline and
    // save it in a cache
    OUT
    return request;
}

bool
Server::send_response(int client, string response) {
    IN
    // prepare to send response
    response.append ("}\n");
    LOGD ("[server] send: %s", response.c_str ());
    const char* ptr = response.c_str();
    int nleft = response.length();
    int nleft_ = response.length();
    int nwritten;
    // loop to be sure it is all sent
    while (nleft) {
        if ((nwritten = send(client, ptr, nleft, 0)) < 0) {
            if (errno == EINTR) {
                OUT
                LOGD ("the socket call was interrupted -- try again\n");
                continue;
            } else {
                HERE LOGD ("[%s] an error occurred, so break out\n", strerror (errno));
                perror("write");
                OUT
                return false;
            }
        } else if (nwritten == 0) {
            LOGD ("the socket is closed\n");
            OUT
            return false;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }

    LOGD ("[send response] left %d of %d bytes\n", nleft, nleft_);
    OUT    
    //~ close_socket ();
    return true;
}

#endif
