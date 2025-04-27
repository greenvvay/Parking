#pragma once

#include <chrono>

inline std::pair<uint8_t, uint8_t> getCurrentTime() {

    const auto now = std::chrono::system_clock::now();
    
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm localTime = *std::localtime(&nowTime);
    
    return {
        static_cast<uint8_t>(localTime.tm_hour),
        static_cast<uint8_t>(localTime.tm_min)
    };
}

inline int getDayOfWeekCount() {
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm localTime = *std::localtime(&nowTime);
    
    return localTime.tm_wday == 0 ? 7 : localTime.tm_wday;
}