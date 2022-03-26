#pragma once

#include <functional>
#include <memory>

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/basic_streambuf.hpp>


using boost::asio::ip::tcp;
using namespace std;


template <typename EmitFunction>
class session:
    public std::enable_shared_from_this<session<EmitFunction>> {
public:
    session(tcp::socket&& socket, EmitFunction emit)
        : m_socket(std::move(socket))
        , m_emit(emit)
    { }

    void start()
    {
        do_read();
    }

private:
    using shared_session =
        std::enable_shared_from_this<session<EmitFunction>>;

    void do_read()
    {
        auto self = shared_session::shared_from_this();    

boost::asio::async_read_until(    
            m_socket, m_data, '\n',    
            [this, self](const error_code& error,
                         std::size_t size) {

                if (!error) {    
                    std::istream is(&m_data);    
                    std::string line;    
                    std::getline(is, line);    
                    m_emit(std::move(line));    

                    do_read();    
                }
            });
    }

    tcp::socket m_socket;
    boost::asio::streambuf m_data;
    EmitFunction m_emit;
};


template <typename Socket, typename EmitFunction>
auto make_shared_session(Socket&& socket, EmitFunction&& emit)
{
    return std::make_shared< session<EmitFunction> >(
            std::forward<Socket>(socket),
            std::forward<EmitFunction>(emit));
}
