#pragma once

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
                    process_message(std::move(message));    
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
