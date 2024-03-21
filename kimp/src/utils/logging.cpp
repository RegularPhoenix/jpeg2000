#include <jpeg2000/utils/logging.hpp>

#include <spdlog/sinks/stdout_color_sinks.h>

namespace kimp::jpeg2000::utils {

TWithLogging::TWithLogging(const std::string& loggerName) {
    Logger_ = spdlog::get(loggerName);
    if (!Logger_) {
        Logger_ = spdlog::stderr_color_mt(loggerName);
    }
}

} // namespace kimp::jpeg2000:utils
