// https://www.manning.com/books/functional-programming-in-c-plus-plus

// { "FirstURL": "https://isocpp.org/", "Text": "Standard C++" }
// curl -X POST -d" {\"FirstURL\": \"https://isocpp.org/\", \"Text\": \"Standard C++\" }" http://localhost:42042/

#include <iostream>

#include <boost/asio.hpp>

#include "service.hpp"
#include "sink.hpp"


using boost::asio::ip::tcp;
using namespace std;


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

