#pragma once

#include <spdlog/spdlog.h>
#include <memory>

/**
 * @class Logger
 * @brief A static wrapper for the spdlog logging library.
 *
 * Provides a globally accessible singleton instance of a spdlog logger.
 * This class must be initialized with `init()` before use.
 */
class Logger {
public:
    static void init();
    static std::shared_ptr<spdlog::logger>& get();

private:
    static std::shared_ptr<spdlog::logger> s_logger;
};
