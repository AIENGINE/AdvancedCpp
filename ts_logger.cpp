#include "ts_logger.h"

namespace utilities 
{
    struct logger::impl 
    {
        std::ofstream log_file;
        log_level level = log_level::default_level;
        std::queue<std::string> message_queue;
        std::mutex queue_mutex;
        std::condition_variable cv;
        std::jthread writer_thread;
        std::atomic<bool> shutdown_requested{false};

        impl() 
        {
            log_file.open("test_log.txt", std::ios::app);
            writer_thread = std::jthread([this] { writer_loop(); });
        }

        ~impl() 
        {
            shutdown();
            writer_thread.join();
            log_file.close();
        }

        void writer_loop() 
        {
            while (true) {
                std::unique_lock<std::mutex> lock(queue_mutex);
                cv.wait(lock, [this] { return !message_queue.empty() || shutdown_requested.load(); });

                if (shutdown_requested.load() && message_queue.empty()) {
                    break;
                }

                std::string message = std::move(message_queue.front());
                message_queue.pop();
                lock.unlock();

                log_file << message << std::endl;
                std::cout << "Writing " << message << " to the log file." << std::endl;
            }
        }

        void shutdown() 
        {
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                shutdown_requested.store(true);
            }
            cv.notify_one();
        }

        std::string get_current_time_string() 
        {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
            return ss.str();
        }

        std::string format_message(log_level level, const std::string& msg) 
        {
            std::stringstream ss;
            ss << "[" << get_current_time_string() << "] [" << static_cast<int>(level) << "] " << msg;
            return ss.str();
        }
    };

    logger::logger() : pimpl(std::make_unique<impl>()) {}

    logger::~logger() = default;

    logger& logger::get_instance() 
    {
        static logger instance;
        return instance;
    }

    void logger::set_level(log_level level) 
    {
        pimpl->level = level;
    }

    log_level logger::get_level() const 
    {
        return pimpl->level;
    }

    void logger::write_impl(log_level level, const std::string& msg) 
    {
        std::lock_guard<std::mutex> lock(pimpl->queue_mutex);
        pimpl->message_queue.push(pimpl->format_message(level, msg));
        pimpl->cv.notify_one();
    }

    void logger::shutdown() 
    {
        pimpl->shutdown();
    }
}