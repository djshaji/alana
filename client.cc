#ifdef __linux__
#include "client.h"

Client::Client(string host, int port) {
    // setup variables
    host_ = host;
    port_ = port;
    buflen_ = 1024;
    buf_ = new char[buflen_+1];
}

Client::~Client() {
}

void Client::run() {
    // connect to the server and run echo program
    create();
    echo();
}

void
Client::create() {
    IN
    struct sockaddr_in server_addr;

    // use DNS to get IP address
    struct hostent *hostEntry;
    hostEntry = gethostbyname(host_.c_str());
    if (!hostEntry) {
        LOGD ("No such host name: %s" , host_.c_str ());
        return ;
    }

    LOGD ("[client] connect to %s:%d\n", host_.c_str (), port_);
    // setup socket address structure
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    memcpy(&server_addr.sin_addr, hostEntry->h_addr_list[0], hostEntry->h_length);

    // create socket
    server_ = socket(PF_INET,SOCK_STREAM,0);
    if (!server_) {
        HERE LOGD ("cannot create socket\n");
        perror("socket");
        exit(-1);
    }

    // connect to server
    if (connect(server_,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
        HERE LOGD ("cannot connect to server\n");
        perror("connect");
        exit(-1);
    }
}

void
Client::close_socket() {
    IN
    close(server_);
    OUT
}

void
Client::echo() {
    string line;
    
    // loop to handle user interface
    while (getline(cin,line)) {
        // append a newline
        line += "\n";
        // send request
        bool success = send_request(line);
        if (not success) {
            printf ("[68] break if an error occurred\n");
            break;
        }
        // get a response
        success = get_response();
        if (not success) {
            printf ("[73] break if an error occurred\n");
            break; 
        }
    }
    close_socket();
}

std::string
Client::send_preset(json j) {
    IN
    // loop to handle user interface
    string toSend = j.dump () ;
    if (j.empty ())
        toSend = string ("{}");
        
    bool success = send_request(toSend);
    if (not success) {
        printf ("[86] break if an error occurred\n");
    }
    // get a response
    string response = "";
    // read until we get a newline
    while (response.find("}}}") == string::npos) {
        int nread = recv(server_,buf_,1024,0);
        if (nread < 0) {
            if (errno == EINTR) {
                printf ("the socket call was interrupted -- try again\n");
                continue;
            }
            else {
                HERE LOGD ("an error occurred: %s\n", strerror (errno));
                return std::string ("");
            }
        } else if (nread == 0) {
            printf ("the socket is closed\n");
            return std::string ("");
        }
        // be sure to use append in case we have binary data
        response.append(buf_,nread);
        if (response.c_str ()[1] == '}')
            return std::string ("");
    }
    
    // a better client would cut off anything after the newline and
    // save it in a cache
    if (not success) {
        HERE LOGD ("[92] break if an error occurred\n");
    }

    //~ close_socket();
    OUT
    return response ;
}

bool
Client::send_request(string request) {
    IN
    ///| WARNING: we append another } to the message so that we know when to stop reading
    ///| HACKETY HACKETY HACK
    request.append ("}");
    printf ("----------------------- -|prepare to send request: %s |- ---------------------------\n", 
        request.c_str ());
    const char* ptr = request.c_str();
    int nleft = request.length();
    int nleft_ = request.length();
    int nwritten;
    // loop to be sure it is all sent
    while (nleft) {
        if ((nwritten = send(server_, ptr, nleft, 0)) < 0) {
            if (errno == EINTR) {
                LOGD ("the socket call was interrupted -- try again\n");
                continue;
            } else {
                HERE LOGD ("an error occurred: %s\n", strerror (errno));
                perror("write");
                return false;
            }
        } else if (nwritten == 0) {
            LOGD ("the socket is closed\n");
            return false;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    LOGD ("left %d / %d bytes to write\n", nleft, nleft_);
    OUT
    return true;
}

bool
Client::get_response() {
    IN
    string response = "";
    // read until we get a newline
    while (response.find("}}}") == string::npos) {
        int nread = recv(server_,buf_,1024,0);
        if (nread < 0) {
            if (errno == EINTR) {
                printf ("the socket call was interrupted -- try again\n");
                continue;
            }
            else {
                HERE LOGD ("[%s] an error occurred, so break out\n", strerror (errno));
                return "";
            }
        } else if (nread == 0) {
            printf ("the socket is closed\n");
            return "";
        }
        // be sure to use append in case we have binary data
        response.append(buf_,nread);
    }
    // a better client would cut off anything after the newline and
    // save it in a cache
    HERE LOGD ("[client] response: %s\n", response.c_str ());
    OUT
    return true;
}
#endif
