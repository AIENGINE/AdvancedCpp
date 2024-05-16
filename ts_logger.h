#ifndef TS_LOGGER_H
#define TS_LOGGER_H

#include <bits/stdc++.h>

namespace utilities 
{
    enum class log_level: int 
    {
        off = 0,
        error,
        warning,
        info,
        default_level = info,
        debug,
        _last
    };

    class logger 
    {
    public:
        static logger& get_instance(); // for singleton class 

        logger(const logger&) = delete;
        logger& operator=(const logger&) = delete;
        logger(logger&&) = delete;
        logger& operator=(logger&&) = delete;

        void set_level(log_level level);
        log_level get_level() const;
        void shutdown();

        template<typename... Args>
        void write(log_level level, Args&&... args) 
        {
            if (level <= get_level()) 
            {
                std::ostringstream oss;
                (oss << ... << std::forward<Args>(args));
                write_impl(level, oss.str());
            }
        }

    private:
        logger();
        ~logger();

        void write_impl(log_level level, const std::string& msg);

        struct impl;
        std::unique_ptr<impl> pimpl;
    };
}

#endif // TS_LOGGER_H