#pragma once

#include <functional>

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

