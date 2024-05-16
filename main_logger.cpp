#include "ts_logger.h"

int main() 
{
    auto& logger = utilities::logger::get_instance();
    logger.set_level(utilities::log_level::debug);

    // Start logging from multiple threads, multiple logger to a single writer thread
    std::vector<std::jthread> threads;
    for (int i = 0; i < 5; ++i) 
    {
        threads.emplace_back([&logger, i]() 
        {
            for (int j = 0; j < 10; ++j)  
            {
                logger.write(utilities::log_level::info, "Message from thread ", i, " iteration ", j);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }

    // press 'q' to shutdown logging
    char input;
    while (std::cin >> input) 
    {
        if (input == 'q') 
        {
            logger.shutdown();
            break;
        }
    }

    return 0;
}