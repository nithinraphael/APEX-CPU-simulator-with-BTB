#pragma once

#include <memory>
using namespace std;

class CPUClock
{
  public:
    static CPUClock& instance()
    {
        static CPUClock instance;
        return instance;
    }

    CPUClock(const CPUClock&) = delete;
    CPUClock& operator=(const CPUClock&) = delete;

    void next()
    {
        time++;
    }

    uint32_t getClock() const
    {
        return time;
    }

  private:
    CPUClock() : time(0)
    {
    }

    uint32_t time;
};