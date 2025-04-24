#include "tool.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

vector<char*> splitbuff(char* buff, char* d) {
    vector<char*> vec;
    char* p;
    p = strtok(buff, d);
    while (p != NULL) {
        vec.push_back(p);
        p = strtok(NULL, d);
    }
    return vec;
};

char* ltrim_lc(char* s) {
    char *p, *q;
    for (p = q = s; *p != '\0';) {
        if (*p == ' ')
            ++p;
        else
            *q++ = *p++;
    }
    *q = '\0';
    return s;
};

// 根据C++版本选择不同的实现
#if __cplusplus >= 202002L
    // C++20及以上版本实现 (使用std::format)
    #include <chrono>
    #include <format>

std::string getTodayString() {
    auto now = std::chrono::system_clock::now();
    return std::format("{:%Y%m%d}", now);
}

#elif __cplusplus >= 201703L
    // C++17实现 (使用std::put_time)
    #include <chrono>
    #include <iomanip>
    #include <sstream>

std::string getTodayString() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d");
    return oss.str();
}

#else
// C++11/14或更早版本实现
std::string getTodayString() {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d");
    return oss.str();
}

#endif
