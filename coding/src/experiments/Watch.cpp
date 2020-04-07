#include <time.h>

class Watch {
    using UInt64 = unsigned long long;

public:
    Watch(clockid_t clock_type_ = CLOCK_MONOTONIC): clockType(clock_type_) { }

    void start()                       { startNanoseconds = nanoseconds(); isRunning = true; }
    void stop()                        { stopNanoseconds = nanoseconds(); isRunning = false; }
    void reset()                       { startNanoseconds = 0; stopNanoseconds = 0; isRunning = false; }
    UInt64 elapsedNanoseconds() const  { return isRunning ? nanoseconds() - startNanoseconds : stopNanoseconds - startNanoseconds; }
    UInt64 elapsedMicroseconds() const { return elapsedNanoseconds() / 1000U; }
    UInt64 elapsedMilliseconds() const { return elapsedNanoseconds() / 1000000UL; }
    double elapsedSeconds() const      { return static_cast<double>(elapsedNanoseconds()) / 1000000000ULL; }

private:
    UInt64 startNanoseconds = 0;
    UInt64 stopNanoseconds = 0;
    clockid_t clockType;
    bool isRunning = false;

    UInt64 clock_gettime_ns(clockid_t clock_type) const
    {
        struct timespec timeSpec;
        clock_gettime(clock_type, &timeSpec);
        /// Converts to nanoseconds by multiplying by 1e9
        return UInt64(timeSpec.tv_sec * 1000000000LL + timeSpec.tv_nsec);
    }

    UInt64 nanoseconds() const { return clock_gettime_ns(clockType); }
};