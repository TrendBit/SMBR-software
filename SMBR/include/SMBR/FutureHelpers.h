#pragma once
#include <future>
#include <vector>

class FutureHelpers
{
public:

    std::future <bool> combineFutures(std::vector<std::shared_future<bool>>& futures)
    {
        return std::async(std::launch::async, [&futures]() {
            for (auto& future : futures)
            {
                future.wait();
                if (!future.get())
                {
                    return false;
                }
            }
            return true;
        });
    }
};