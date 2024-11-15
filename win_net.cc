#include <winsock2.h>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/optional.hpp>
#include <iomanip>
#include <iostream>

namespace asio = boost::asio;
using asio::ip::tcp;
using namespace std::chrono_literals;
using boost::system::error_code;

void read_session(tcp::socket sock) {
    std::cout << "Connection established " << sock.remote_endpoint() << std::endl;

    std::array<char, 1000> buffer;

    for (error_code ec;;) {
        size_t n = sock.read_some(asio::buffer(buffer), ec);
        std::cout << "Received " << n << " " << quoted(std::string(buffer.data(), n)) << " (" << ec.message() << ")" << std::endl;

        if (ec.failed())
            break;
    }

    std::cout << "Connection closed" << std::endl;
}

class TheServer {
  public:
    TheServer(asio::any_io_executor ex, uint16_t port)   //
        : m_Acceptor{ex, tcp::endpoint{tcp::v4(), port}} //
    {
        m_Acceptor.set_option(tcp::acceptor::reuse_address(true));
        do_accept();
    }

  private:
    void do_accept() {
        m_Acceptor.async_accept([this](error_code ec, tcp::socket s) {
            if (!ec) {
                std::thread(read_session, std::move(s)).detach();
                do_accept(); // and immediately accept new connection(s)
            } else {
                std::cout << "Connection error (" << ec.message() << ")" << std::endl;
            }
        });
    }

    tcp::acceptor m_Acceptor;
};

constexpr uint16_t s_port = 1900;

void run_server() {
    asio::thread_pool ioc(1);
    TheServer        server(ioc.get_executor(), s_port);

    std::cout << "Press Enter to quit" << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    ioc.stop();
    ioc.join();
}

void run_client() {
    std::cout << "Starting client" << std::endl;

    asio::io_service ioc(1);

    try {
        tcp::socket m_Socket(ioc);
        m_Socket.connect({{}, s_port});

        std::cout << "Client connected" << std::endl;

        for (std::string msg : {"Hello World", "Bye World"}) {
            write(m_Socket, asio::buffer(msg));
            sleep(100);
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}

int main(int argc, char**) {
    if (argc>1)
        run_server();
    else
        run_client();
}

