// https://www.manning.com/books/functional-programming-in-c-plus-plus

#include <iostream>
#include <functional>
#include <memory>

#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/basic_streambuf.hpp>


// { "FirstURL": "https://isocpp.org/", "Text": "Standard C++" }
// curl -X POST -d" {\"FirstURL\": \"https://isocpp.org/\", \"Text\": \"Standard C++\" }" http://localhost:42042/

using boost::asio::ip::tcp;
using namespace std;


template <typename SourceMessageType,    
          typename MessageType>    
class actor {
public:
    using value_type = MessageType;    

    void process_message(SourceMessageType&& message);    

    template <typename EmitFunction>
    void set_message_handler(EmitFunction emit);    

private:
    std::function<void(MessageType&&)> m_emit;    
};


template <typename EmitFunction>
class session:
    public std::enable_shared_from_this<session<EmitFunction>> {
public:
    session(tcp::socket&& socket, EmitFunction emit)
        : m_socket(std::move(socket))
        , m_emit(emit)
    {
    }

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

namespace detail {
    template <typename Sender,
              typename Function,
              typename MessageType = typename Sender::value_type>
    class sink_impl {
    public:
        sink_impl(Sender&& sender, Function function)
            : m_sender(std::move(sender))
            , m_function(function)
        {
            m_sender.set_message_handler(    
                [this](MessageType&& message)    
                {    
                    process_message(    
                        std::move(message));    
                }    
            );
        }

        void process_message(MessageType&& message) const
        {
            std::invoke(m_function,
                        std::move(message));    
        }

    private:
        Sender m_sender;
        Function m_function;
    };
}

template <typename Sender, typename Function>
auto sink(Sender&& sender, Function&& function)
{
    return detail::sink_impl<Sender, Function>(
        std::forward<Sender>(sender),
        std::forward<Function>(function));
}

int main(int argc, char* argv[])
{
    boost::asio::io_service event_loop;    

    auto pipeline =
        sink(service(event_loop),    
             [](const auto& message) {    
                 std::cerr << message << std::endl;    
             });

    event_loop.run();    
}

