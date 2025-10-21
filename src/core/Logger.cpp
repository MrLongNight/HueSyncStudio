#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Logger::s_logger;

void Logger::init() {
    s_logger = spdlog::stdout_color_mt("HueSyncStudio");
    s_logger->set_level(spdlog::level::trace);
    s_logger->info("Logger initialized.");
}

std::shared_ptr<spdlog::logger>& Logger::get() {
    return s_logger;
}
