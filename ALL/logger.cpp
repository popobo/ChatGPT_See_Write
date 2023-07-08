#include "logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

std::shared_ptr<spdlog::logger> Logger::m_logger{nullptr};
std::shared_ptr<spdlog::logger> Logger::m_console{nullptr};

void Logger::init() {
    spdlog::set_pattern("%^[%T] %n %! %#: %v%$");

    m_console = spdlog::stdout_color_mt("ChatGPT_Writer");
    m_console->set_level(spdlog::level::trace);

    m_logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
    m_logger->set_level(spdlog::level::trace);

    spdlog::flush_every(std::chrono::seconds(1));
}
