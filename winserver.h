# ifndef WINSERVER
# define WINSERVER
# include "presets.h"
#include <winsock2.h>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/optional.hpp>
#include <iomanip>
#include <iostream>
#include <iphlpapi.h>

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

using namespace std;
using json = nlohmann::json;

namespace asio = boost::asio;
using asio::ip::tcp;
using namespace std::chrono_literals;
using boost::system::error_code;

void read_session(Presets * p, GtkLabel *, tcp::socket sock) ;

class TheServer {
  public:
    Presets * presets ;
    GtkLabel * status ;
    TheServer(asio::any_io_executor ex, uint16_t port, GtkLabel * s)   //
        : m_Acceptor{ex, tcp::endpoint{tcp::v4(), port}} //
    {
        IN
        status = s ;
        m_Acceptor.set_option(tcp::acceptor::reuse_address(true));
        do_accept();
        OUT
    }

  private:
  
    void do_accept() {
        IN
        m_Acceptor.async_accept([this](boost::system::error_code ec, tcp::socket s) {
            if (!ec) {
                std::thread(read_session, presets, status, std::move(s)).detach();
                do_accept(); // and immediately accept new connection(s)
            } else {
                //~ std::cout << "Connection error (" << ec.message() << ")" << std::endl;
                LOGD ("connection error %s", ec.message ().c_str ());
            }
        });
        OUT
    }

    tcp::acceptor m_Acceptor;
};

class Client {
public:
    Presets * presets;
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

class Server {
public:
    Server();
    ~Server();
    
    Presets * presets ;
    GtkLabel * status ;
    void * sync ;
    void run();
    
    GMainContext * context ;
    GMainLoop * main_loop ;
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

string getMachineIPAddr() ;

class NET {
    public:
    vector <string> addresses;
    
    NET () {
        addresses.push_back (getMachineIPAddr ());
    }
} ;


# endif
