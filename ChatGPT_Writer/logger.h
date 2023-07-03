#ifndef LOGGER_H
#define LOGGER_H

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#include "spdlog/spdlog.h"

class Logger {
  public:
    static void init();

    static std::shared_ptr<spdlog::logger> &getLogger() {
        return m_logger;
    }

    static std::shared_ptr<spdlog::logger> &getConsole() {
        return m_console;
    }

  private:
    static std::shared_ptr<spdlog::logger> m_logger;
    static std::shared_ptr<spdlog::logger> m_console;
};

#define DEBUG_MODE

#ifdef DEBUG_MODE
#define SPD_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(Logger::getLogger(), __VA_ARGS__); \
                         SPDLOG_LOGGER_CRITICAL(Logger::getConsole(), __VA_ARGS__)
#define SPD_ERROR(...) SPDLOG_LOGGER_ERROR(Logger::getLogger(), __VA_ARGS__); \
                      SPDLOG_LOGGER_ERROR(Logger::getConsole(), __VA_ARGS__)
#define SPD_WARN(...) SPDLOG_LOGGER_WARN(Logger::getLogger(), __VA_ARGS__); \
                     SPDLOG_LOGGER_WARN(Logger::getConsole(), __VA_ARGS__)
#define SPD_INFO(...) SPDLOG_LOGGER_INFO(Logger::getLogger(), __VA_ARGS__); \
                     SPDLOG_LOGGER_INFO(Logger::getConsole(), __VA_ARGS__)
#define SPD_TRACE(...) SPDLOG_LOGGER_TRACE(Logger::getLogger(), __VA_ARGS__); \
                      SPDLOG_LOGGER_TRACE(Logger::getConsole(), __VA_ARGS__)
#else
#define SPD_CRITICAL(...) Logger::getLogger()->critical(__VA_ARGS__)
#define SPD_ERROR(...) Logger::getLogger()->error(__VA_ARGS__)
#define SPD_WARN(...) Logger::getLogger()->warn(__VA_ARGS__)
#define SPD_INFO(...) Logger::getLogger()->info(__VA_ARGS__)
#define SPD_TRACE(...) Logger::getLogger()->trace(__VA_ARGS__)
#endif

#endif // LOGGER_H
