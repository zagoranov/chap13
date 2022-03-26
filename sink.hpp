#pragma once

#include "detail.hpp"

template <typename Sender, typename Function>
auto sink(Sender&& sender, Function&& function)
{
    return detail::sink_impl<Sender, Function>(
        std::forward<Sender>(sender),
        std::forward<Function>(function));
}
