#pragma once
#include <memory>
#include <stdint.h>

template <typename F>
class scope_guard {
    F f;
    bool active;
public:
    explicit scope_guard(F f) : f(std::move(f)), active(true) {}

    ~scope_guard() {
        if (active) f();
    }

    void dismiss() { active = false; }

    scope_guard(scope_guard&&);
    scope_guard(const scope_guard&);
    scope_guard& operator=(const scope_guard&);
};

template <typename F>
scope_guard<F> make_scope_guard(F&& f) {
    return scope_guard<F>(std::forward<F>(f));
}

#include <string>

std::string fourcc2string(uint32_t fourcc);

extern void LogDebug(const char* file, int line, const char* msg, ...);

#define MW_LOG(msg, ...) LogDebug(__FILE__, __LINE__, msg, ##__VA_ARGS__)

#define MW_LOG_HR_ERROR(hr, operation) MW_LOG("D3D operation '%s' failed with error: 0x%08X\n", operation, hr);

