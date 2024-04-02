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
#ifndef JITTERSTATISTICS_H_
#define JITTERSTATISTICS_H_
#include "Constants.h"

namespace sprocketRealtimeScheduler {

// Jitter data for a single frame.
struct FrameJitterEntry {
     // Base period time.
    double base_period_;

    // The average jitter value;
    double average_jitter_;

    // The absolute jitter value.
    double absolute_jitter_;

    // The early jitter value
    double early_;

    // The late jitter value
    double late_;
};

struct FrameJitterEntryData {
    FrameJitterEntry data_;

    // The best start time.
    double best_start_time_;

    // The worst start time.
    double  worst_start_time_;
};

// Data structure for thread start time jitter.
struct ThreadStartTimeJitterData {
    // The current start time.
    double current_start_time_;

    // The average start time.
    double average_start_time_;

    // The best start time.
    double best_start_time_;

    // The worst start time.
    double  worst_start_time_;

    // accumulated start times
    double total_start_time_;

    // The total number of passes that have run.
    unsigned int total_passes_run_;
};

struct ThreadStartTimeJitterEntryData {
    ThreadStartTimeJitterData data_;

    // The worst start jitter time to date.
    double worst_start_jitter_;

    // The best start jitter time to date.
    double best_start_jitter_;
};

struct JitterStatistics {
    // ===========================
    // = Frame Jitter Statistics =
    // ===========================
    FrameJitterEntry jitter_entries_[MAX_FRAMES];

    // The worst (latest) jitter of any frame to date.
    double worst_frame_jitter_;

    // The best (earliest) jitter of any frame to date.
    double best_frame_jitter_;

    // The accumulate total jitter to date.
    double accumulate_total_jitter_;

    // The last pass time (absolute).
    double last_pass_time_;

    // Flag to indicate that this is the first pass through of the jitter
    // calculations. This is done because at least two sets of delta values are
    // required for the jitter calculation.
    bool first_jitter_calc_pass_;

    // The number of times jitter data has been calculated.
    DWORD  jitter_calculation_count_;

    // =======================================
    // = Thread Start Time Jitter Statistics =
    // =======================================
    ThreadStartTimeJitterData thread_start_jitter_data_[MAX_FRAMES];

    // The worst start jitter time to date.
    double worst_start_jitter_;

    // The best start jitter time to date.
    double best_start_jitter_;
};

}   // namespace sprocketRealtimeScheduler

#endif  // JITTERSTATISTICS_H_
