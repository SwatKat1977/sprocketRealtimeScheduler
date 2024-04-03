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
-----------------------------------------------------------------------------
*/
#ifndef REALTIMETHREAD_H_
#define REALTIMETHREAD_H_
#include <thread>               // NOLINT
#include "Constants.h"
#include "ThreadStatistics.h"
#include "ThreadCondition.h"

namespace sprocketRealtimeScheduler {

class RealtimeThread : public std::thread {
 public:
    RealtimeThread();

    ~RealtimeThread() = default;

    // Thread condition to start the thread kill, it hasn't died until
    // thread_dead_condition_ has been set.
    ThreadCondition kill_thread_;

    // Thread condition to identify when the thread has been killed.
    ThreadCondition thread_dead_;

    void StartThread();
    void KillThread();

    void SuperviorThreadStartTime(double time) {
        supervior_thread_start_time_ = time; }
    double SuperviorThreadStartTime() { return supervior_thread_start_time_; }
    void SuperviorThreadStopTime(double time) {
        supervior_thread_stop_time_ = time; }
    double SuperviorThreadStopTime() { return supervior_thread_stop_time_; }

    void ZeroJitterTimes();
    void ZeroFrameTimes();
    void ZeroThreadStartJitter();

    FrameTimingDataEntry GetTimingData(DWORD frame_no);
    FrameJitterEntryData GetJitterStatistics(DWORD frame_no);
    ThreadStartTimeJitterEntryData GetThreadStartJitterData(DWORD frame_no);

    DWORD IncrementCurrentFrame();
    DWORD CurrentFrame();

 protected:
    ThreadStatistics statistics_;
    double cpu_ticks_per_second_;
    DWORD current_frame_;
    DWORD frame_mask_;
    double supervior_thread_start_time_;
    double supervior_thread_stop_time_;
    double wanted_frame_period_seconds_;

    double SnapshotTimestamp();
    double CalculateFrameJitter(int frame);
    void CalculateFrameTimings(int current_frame, double frame_start,
                               double frame_end);
    void CalculateTheadStartJitter(int current_frame);

    virtual double ThreadLoop();
};

}   // namespace sprocketRealtimeScheduler

#endif  // REALTIMETHREAD_H_
