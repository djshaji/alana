#include "winserver.h"

asio::thread_pool ioc(1);


void read_session(tcp::socket sock) {
    std::cout << "Connection established " << sock.remote_endpoint() << std::endl;

    std::array<char, 1000> buffer;

    for (boost::system::error_code ec;;) {
        size_t n = sock.read_some(asio::buffer(buffer), ec);
        std::cout << "Received " << n << " " << quoted(std::string(buffer.data(), n)) << " (" << ec.message() << ")" << std::endl;

        if (ec.failed())
            break;
    }

    std::cout << "Connection closed" << std::endl;
}

void Client::create () {
    IN
    
    OUT
}

Client::Client (string host, int port) {
    IN
    host_ = host ;
    port_ = port ;
    OUT
}

std::string Client::send_preset (json j) {
    IN
    asio::io_service ioc(1);

    try {
        tcp::socket m_Socket(ioc);
        ///| todo enter hostname here
        m_Socket.connect({{}, port_});

        std::cout << "Client connected" << std::endl;

        ///| todo: add extra } here
        std::string msg = j.dump ().append (string ("}")) ;
        write(m_Socket, asio::buffer(msg));
        std::this_thread::sleep_for(100ms);
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    
    return string ("");
    OUT
}

void Server::close_socket () {
    ioc.stop();
    ioc.join();
}

void Client::close_socket () {
}

Server::Server () {
    
}

void Server::run () {
    TheServer        server(ioc.get_executor(), 6906);    
}

