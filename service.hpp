#pragma once

#include <iostream>
#include <functional>

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/basic_streambuf.hpp>

#include "session.hpp"

using boost::asio::ip::tcp;
using namespace std;


class service {
public:
    using value_type = std::string;    

    explicit service(
            boost::asio::io_service& service, unsigned short port = 42042) : 
               m_acceptor(service, tcp::endpoint(tcp::v4(), port)), m_socket(service)    
    { }

    service(const service& other) = delete;    
    service(service&& other) = default;    

    template <typename EmitFunction>
    void set_message_handler(EmitFunction emit)    
    {    
        m_emit = emit;    
        do_accept();    
    }

private:
    void do_accept()
    {
        m_acceptor.async_accept(m_socket, [this](const std::error_code& error) {
                if (!error) {
                    make_shared_session(std::move(m_socket), m_emit)->start();
                } else {
                    std::cerr << error.message() << std::endl;
                }
                // Listening to another client
                do_accept();
            });
    }

    tcp::acceptor m_acceptor;
    tcp::socket m_socket;
    std::function<void(std::string&&)> m_emit;
};

