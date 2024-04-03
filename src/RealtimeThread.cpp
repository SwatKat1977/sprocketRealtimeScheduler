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
#include "RealtimeThread.h"

namespace sprocketRealtimeScheduler {

RealtimeThread::RealtimeThread() : current_frame_(0),
    frame_mask_(0x80000000) {
}

void RealtimeThread::StartThread() {
    ZeroJitterTimes();
    ZeroFrameTimes();
    ZeroThreadStartJitter();

    while (kill_thread_.WaitFor(TIMEOUT_IMMEDIATE) ==
                                std::cv_status::timeout) {
        ThreadLoop();
    }

    // Acknowledge the thread is now dead.
    thread_dead_.Notify();
}

/*
This function should never be called from within the context of this thread as
it's unsafe, but instead be called from another thread context.
*/
void RealtimeThread::KillThread() {
    // Notify the thread that it needs to shutdown. This is the first part of a
    // two part proceess, the thread is only considered killed when
    // thread_dead_ is set.
    kill_thread_.Notify();

    // Wait until thread_dead has been set before considering it killed.
    thread_dead_.WaitFor(TIMEOUT_NEVER);
}

FrameTimingDataEntry RealtimeThread::GetTimingData(DWORD frame_no) {
    if (frame_no >= MAX_FRAMES) throw std::runtime_error("invalid frame");

    FrameTimingDataEntry entry;
    entry.data_ = statistics_.frame_data_[frame_no];
    entry.worst_frame_time_ = statistics_.worst_frame_time_;
    entry.best_frame_time_ = statistics_.best_frame_time_;
    return entry;
}

FrameJitterEntryData RealtimeThread::GetJitterData(DWORD frame_no) {
    if (frame_no >= MAX_FRAMES) throw std::runtime_error("invalid frame");

    FrameJitterEntryData entry;
    entry.data_ = statistics_.jitter_data_[frame_no];
    entry.worst_start_time_ = statistics_.worst_frame_jitter_;
    entry.best_start_time_ = statistics_.best_frame_jitter_;
    return entry;
}

ThreadStartTimeJitterEntryData RealtimeThread::GetThreadStartJitterData(
    DWORD frame_no) {
    if (frame_no >= MAX_FRAMES) throw std::runtime_error("invalid frame");

    ThreadStartTimeJitterEntryData entry;
    entry.data_ = statistics_.thread_start_jitter_data_[frame_no];
    entry.worst_start_jitter_ = statistics_.worst_start_jitter_;
    entry.best_start_jitter_ = statistics_.best_start_jitter_;
    return entry;
}

DWORD RealtimeThread::IncrementCurrentFrame() {
    current_frame_++;
    frame_mask_ = frame_mask_ >> 1;

    if (current_frame_ >= MAX_FRAMES) {
        current_frame_ = 0;
        frame_mask_ = 0x80000000;
    }

    return current_frame_;
}

void RealtimeThread::ZeroFrameTimes() {
    statistics_.worst_frame_time_ = 0.0;
    statistics_.best_frame_time_ = 99999.0;

    for (int idx = 0; idx < MAX_FRAMES; idx++) {
        statistics_.frame_data_[idx].total_frames_run_ = 0;
        statistics_.frame_data_[idx].current_time_ = 0.0;
        statistics_.frame_data_[idx].average_time_ = 0.0;
        statistics_.frame_data_[idx].best_time_ = 0.0;
        statistics_.frame_data_[idx].worst_time_ = 0.0;
        statistics_.frame_data_[idx].total_time_ = 0.0;
    }
}

void RealtimeThread::ZeroJitterTimes() {
    statistics_.worst_frame_jitter_ = 0.0;
    statistics_.best_frame_jitter_ = 99999.0;
    statistics_.accumulate_total_jitter_ = 0.0;
    statistics_.last_pass_time_ = 0.0;
    statistics_.jitter_calculation_count_ = 0;
    statistics_.first_jitter_calc_pass_ = true;

    for (int idx = 0; idx < MAX_FRAMES; idx++) {
        statistics_.jitter_data_[idx].base_period_ = 0.0;
        statistics_.jitter_data_[idx].average_jitter_ = 0.0;
        statistics_.jitter_data_[idx].current_jitter_ = 0.0;
        statistics_.jitter_data_[idx].early_ = 0.0;
        statistics_.jitter_data_[idx].late_ = 0.0;
    }
}

void RealtimeThread::ZeroThreadStartJitter() {
    statistics_.worst_start_jitter_ = 0.0;
    statistics_.best_start_jitter_ = 99999.0;

    for (int idx = 0; idx < MAX_FRAMES; idx++) {
        statistics_.thread_start_jitter_data_[idx].total_passes_run_ = 0;
        statistics_.thread_start_jitter_data_[idx].current_start_time_ = 0.0;
        statistics_.thread_start_jitter_data_[idx].average_start_time_ = 0.0;
        statistics_.thread_start_jitter_data_[idx].best_start_time_ = 0.0;
        statistics_.thread_start_jitter_data_[idx].worst_start_time_ = 0.0;
        statistics_.thread_start_jitter_data_[idx].total_start_time_ = 0.0;
    }
}

void RealtimeThread::CalculateFrameTimings(int current_frame,
                                           double frame_start,
                                           double frame_end) {
    // Increment the running total for frames that have been run.
    statistics_.frame_data_[current_frame].total_frames_run_++;

    // Calculate frame delta time in seconds.
    double delta = (frame_end - frame_start);

    statistics_.frame_data_[current_frame].current_time_ = delta;
    statistics_.frame_data_[current_frame].total_time_+= delta;
    statistics_.frame_data_[current_frame].average_time_ =
        statistics_.frame_data_[current_frame].total_time_ /
        statistics_.frame_data_[current_frame].total_frames_run_;

    // Frame best time gets zero'd to 0.0, if it is at the initial value then
    // always set a best time.
    if (statistics_.frame_data_[current_frame].best_time_ == 0.0) {
        statistics_.frame_data_[current_frame].best_time_ =
            statistics_.frame_data_[current_frame].current_time_;
    }
    // Check to see if the best frame time needs updating.
    else if (statistics_.frame_data_[current_frame].current_time_ <
             statistics_.frame_data_[current_frame].best_time_) {
        statistics_.frame_data_[current_frame].best_time_ =
            statistics_.frame_data_[current_frame].current_time_;
    }

    // Check to see if the worst frame time needs updating.
    if (statistics_.frame_data_[current_frame].current_time_ >=
        statistics_.frame_data_[current_frame].worst_time_) {
        statistics_.frame_data_[current_frame].worst_time_ =
            statistics_.frame_data_[current_frame].current_time_;
    }

    // Store the worst and best frame jitter values.
    if (statistics_.frame_data_[current_frame].current_time_ >=
        statistics_.worst_frame_time_) {
        statistics_.worst_frame_time_ =
            statistics_.frame_data_[current_frame].current_time_;
    }
    else if(statistics_.frame_data_[current_frame].current_time_ <=
            statistics_.best_frame_time_) {
        statistics_.best_frame_time_ =
            statistics_.frame_data_[current_frame].current_time_;
    }
}

double RealtimeThread::CalculateFrameJitter(int frame)
{
    double time_snapshot = TimestampSnapshot();

    if (statistics_.first_jitter_calc_pass_)
    {
        statistics_.first_jitter_calc_pass_  = false;
        statistics_.last_pass_time_ = time_snapshot;
        return time_snapshot;
    }

    double current_jitter = time_snapshot - statistics_.last_pass_time_;

    // Save the last pass time now, this is because the rest of the jitter
    // calculation will impact the jitter accuracy.
    statistics_.last_pass_time_ = time_snapshot;

    statistics_.jitter_data_[frame].base_period_ = current_jitter;

    // Calculate the jitter delta value.
    double delta_jitter = current_jitter -  wanted_frame_period_seconds_;

    // Calculate the average jitter delta.
    statistics_.accumulate_total_jitter_ += delta_jitter;
    statistics_.jitter_calculation_count_++;
    statistics_.jitter_data_[frame].average_jitter_ =
        statistics_.accumulate_total_jitter_ / statistics_.jitter_calculation_count_;

    statistics_.jitter_data_[frame].current_jitter_ = delta_jitter;

    // Check if this is currently the earliest frame start time, if so, update.
    if (delta_jitter < statistics_.jitter_data_[frame].early_) {
        statistics_.jitter_data_[frame].early_ = delta_jitter;
    }
    // Check if this is currently the latest frame start time, if so, update.
    else if (delta_jitter > statistics_.jitter_data_[frame].late_) {
        statistics_.jitter_data_[frame].late_ = delta_jitter;
    }

    //remember the very worst and best jitter times
    if(current_jitter >= statistics_.worst_frame_jitter_) {

        statistics_.worst_frame_jitter_ = current_jitter;
    }
    else if (current_jitter <= statistics_.best_frame_jitter_) {
        statistics_.best_frame_jitter_ = current_jitter;
    }

    return time_snapshot;
}

void RealtimeThread::CalculateTheadStartJitter(int frame)
{
    double now = TimestampSnapshot();
    double delta;

    statistics_.thread_start_jitter_data_[frame].total_passes_run_++;

    // Calculate the start time delta (in seconds).
    delta = (now - supervior_thread_start_time_);

    statistics_.thread_start_jitter_data_[frame].current_start_time_ =
        delta;
    statistics_.thread_start_jitter_data_[frame].total_start_time_ +=
        delta;

    statistics_.thread_start_jitter_data_[frame].average_start_time_ =
       statistics_.thread_start_jitter_data_[frame].total_start_time_ /
       statistics_.thread_start_jitter_data_[frame].total_passes_run_;

    // Thread start best time gets zero'd to 0.0, if it is at the initial value then
    // always set a best time.
    if (statistics_.thread_start_jitter_data_[frame].best_start_time_ 
        == 0.0) {
        statistics_.thread_start_jitter_data_[frame].best_start_time_ =
            delta;
    }
    // Check to see if the best frame time needs updating.
    else if (delta < statistics_.thread_start_jitter_data_[frame].best_start_time_) {
        statistics_.thread_start_jitter_data_[frame].best_start_time_ = delta;
    }

    // Check to see if the worst frame time needs updating.
    if (delta >= statistics_.thread_start_jitter_data_[frame].worst_start_time_) {
        statistics_.thread_start_jitter_data_[frame].worst_start_time_ = delta;
    }

    // Store the worst and best frame jitter values.
    if(delta >= statistics_.worst_start_jitter_) {
        statistics_.worst_start_jitter_ = delta;
    }
    else if (delta <= statistics_.best_start_jitter_) {
        statistics_.best_start_jitter_ = delta;
    }
}

}   // namespace sprocketRealtimeScheduler
