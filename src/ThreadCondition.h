/*
-----------------------------------------------------------------------------
This source file is part of Sprocket real-time scheduler.
GitHub : https://github.com/SwatKat1977/sprocketRealtimeScheduler

Copyright 2024 Sprocket real-time scheduler Development Team

    This program is free software : you can redistribute it and /or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.If not, see < https://www.gnu.org/licenses/>.

Acknowledgement:
    Based on Robust Services Core Gate class which can be found on GitHub:
    https://github.com/GregUtas/robust-services-core
-----------------------------------------------------------------------------
*/
#ifndef THREADCONDITION_H_
#define THREADCONDITION_H_
#include <atomic>
#include <condition_variable>   // NOLINT
#include <mutex>                // NOLINT

namespace sprocketRealtimeScheduler {

//  Time durations aliases for different magnitudes.
using timeout_secs = std::chrono::seconds;
using timeout_msecs = std::chrono::milliseconds;
using timeout_usecs = std::chrono::microseconds;
using timeout_nsecs = std::chrono::nanoseconds;

constexpr timeout_msecs TIMEOUT_IMMEDIATE = timeout_msecs(0);
constexpr timeout_msecs TIMEOUT_NEVER = timeout_msecs(UINT32_MAX);

constexpr timeout_msecs ZERO_SECONDS = timeout_msecs(0);
constexpr timeout_msecs ONE_SEC = timeout_msecs(1000);

class ThreadCondition {
 public:
    ThreadCondition() = default;
    ~ThreadCondition() = default;

    void Notify();

    //  Waits on the condition until TIMEOUT.  If TIMEOUT is TIMEOUT_NEVER,
    //  the thread will only unblock when the condition has been signalled.
    //
    std::cv_status WaitFor(const timeout_msecs &timeout);

 private:
    std::condition_variable condition_;
    std::mutex mutex_;
    std::atomic_bool flag_;
};

}   // namespace sprocketRealtimeScheduler

#endif  // THREADCONDITION_H_
