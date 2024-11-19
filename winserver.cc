#ifndef __linux__
#include "winserver.h"

asio::thread_pool ioc(1);


void read_session(Presets * p, GtkLabel * status, tcp::socket sock) {
    IN
    LOGD ("Connection established");

    std::array<char, 1000> buffer;
    std::string str ("");

    for (boost::system::error_code ec;;) {
        size_t n = sock.read_some(asio::buffer(buffer), ec);
        LOGD( "Received %d bytes: %s (%s)", n, std::string(buffer.data(), n).c_str (), ec.message().c_str ());

        str.append (std::string(buffer.data(), n));
        if (ec.failed() || str.find("}}}") != string::npos)
            break;
    }

    str.pop_back () ;
    LOGD ("[server] got request: %s", str.c_str ());
    json j = json::parse (str);
    
    json our = p -> get_all_user_presets ();
    std::string msg = our.dump ().append (string ("}")) ;
    LOGD ("[server] send message: %s", msg.c_str ());
    write(sock, asio::buffer(msg));

    int how_many = p -> import_presets_from_json (j);
    char * ss = g_strdup_printf ("<span foreground=\"green\" weight=\"bold\" size=\"x-large\">Imported %d presets successfully</span>", how_many);
    LOGD("[server] %s\n", ss);
    gtk_label_set_markup (status, ss);
    g_free (ss);
    LOGD ("Connection closed");
    OUT
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
    std::array<char, 1000> buffer;
    std::string str ("");

    try {
        tcp::socket m_Socket(ioc);
        ///| todo enter hostname here
        boost::asio::ip::tcp::endpoint endpoint(
            boost::asio::ip::address::from_string(host_), 6906);
        
        m_Socket.connect(endpoint);

        LOGD ("Client connected");

        ///| todo: add extra } here
        std::string msg = j.dump ().append (string ("}")) ;
        LOGD ("[client] send message: %s", msg.c_str ());
        write(m_Socket, asio::buffer(msg));
        std::this_thread::sleep_for(100ms);

        LOGV ("[client] reading server response ...");
        for (boost::system::error_code ec;;) {
            size_t n = m_Socket.read_some(asio::buffer(buffer), ec);
            LOGD( "Received %d bytes: %s (%s)", n, std::string(buffer.data(), n).c_str (), ec.message().c_str ());

            str.append (std::string(buffer.data(), n));
            if (ec.failed() || str.find("}}}") != string::npos)
                break;
        }

        LOGV (str.c_str ());
        str.pop_back () ;

    } catch (std::exception& e) {
        LOGD("Exception: %s",e.what());
    }
    
    return str ;
    OUT
}

void Server::close_socket () {
    g_main_loop_quit (main_loop);
    ioc.stop();
    ioc.join();
}

void Client::close_socket () {
    
}

Server::Server () {
}

void Server::run () {
    IN
    TheServer        server(ioc.get_executor(), 6906, status);    
    main_loop = g_main_loop_new (null, true);
    g_main_loop_run (main_loop);
    OUT
}

#endif
