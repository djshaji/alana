#include <stdlib.h>
#include <unistd.h>

#include <iostream>

#include "client.h"

using namespace std;

int
main(int argc, char **argv)
{
    int option;

    // setup default arguments
    int port = 6906;
    string host = "localhost";

    // process command line options using getopt()
    // see "man 3 getopt"
    while ((option = getopt(argc,argv,"h:p:")) != -1) {
        switch (option) {
            case 'p':
                port = atoi(optarg);
                break;
            case 'h':
                host = optarg;
                break;
            default:
                cout << "client [-h host] [-p port]" << endl;
                exit(EXIT_FAILURE);
        }
    }

    Client client = Client(host, port);
    std::ifstream ifs(argv [1]);
    json j = json::parse(ifs);
    j ["key"] = 3817 ;
    client.create ();
    cout << client.send_preset (j) << endl;

    //~ client.get_response ();
}

