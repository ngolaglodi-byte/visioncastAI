#include "stdafx.h"
#include "utils.h"
#include <cstdarg>
#include <windows.h>
#include <time.h>

std::string fourcc2string(uint32_t fourcc) {
    std::string ret;
    ret += static_cast<char>(fourcc & 0xFF);
    ret += static_cast<char>((fourcc >> 8) & 0xFF);
    ret += static_cast<char>((fourcc >> 16) & 0xFF);
    ret += static_cast<char>((fourcc >> 24) & 0xFF);
    return ret;
}

// 获取文件名，去掉路径
inline const char* get_filename(const char* path) {
    const char* file = strrchr(path, '\\');
    if (file) {
        return file + 1;
    }
    file = strrchr(path, '/');
    if (file) {
        return file + 1;
    }
    return path;
}

void LogDebug(const char* file, int line, const char* msg, ...) {
    // 获取当前时间戳（纳秒级）
	time_t now = time(NULL);
	struct tm* tm_info = localtime(&now);

    // 格式化时间字符串
    char timestamp[64];
	sprintf(timestamp, "%04d-%2d-%2d %02d:%02d:%02d", tm_info->tm_year, tm_info->tm_mon, tm_info->tm_mday, tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);

    // 处理可变参数
    va_list args;
    va_start(args, msg);
    char message[10*1024];
    vsnprintf(message, sizeof(message), msg, args);
    va_end(args);

    // 组合完整日志信息
    char final_message[12*1024];
    sprintf(final_message, "%s %s\n", timestamp, message);

    // 输出到调试窗口
    OutputDebugStringA(final_message);

    // 输出到标准输出
    fprintf(stdout, "%s", final_message);
    fflush(stdout);
}

#define MW_LOG(msg, ...) LogDebug(__FILE__, __LINE__, msg, ##__VA_ARGS__)

#define MW_LOG_HR_ERROR(hr, operation) MW_LOG("D3D operation '%s' failed with error: 0x%08X\n", operation, hr);
