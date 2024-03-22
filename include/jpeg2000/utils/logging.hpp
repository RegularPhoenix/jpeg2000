#pragma once

#include <spdlog/spdlog.h>

#include <memory>
#include <string>

#include <cxxabi.h>

namespace kimp::jpeg2000::utils {

trait TWithLogging {
public:
    template <typename T>
    TWithLogging(const T* t) : TWithLogging{ GetTypeName(t) } {}

    TWithLogging(const std::string& loggerName);

protected:
    std::shared_ptr<spdlog::logger> Logger_;

private:
    template <typename T>
    static auto GetTypeName(const T* t) -> std::string {
        return std::unique_ptr<char, decltype(&std::free)> {
            abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr)
            , &std::free
        }.get();
    }
};

} // namespace kimp::jpeg2000:utils

