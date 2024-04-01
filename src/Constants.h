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
#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define PLATFORM_WINDOWS      1
#define PLATFORM_WINDOWS_MSVC 2
#define PLATFORM_LINUX        3

#if (defined( __WIN32__ ) || defined( _WIN32 )) && !defined(__ANDROID__)
#  if defined(_MSC_VER)
#    define PLATFORM_TYPE PLATFORM_WINDOWS_MSVC
#  else
#    define PLATFORM_TYPE PLATFORM_WINDOWS_CORE
#  endif
#else
#    define PLATFORM_TYPE PLATFORM_LINUX
#endif

#if (PLATFORM_TYPE != PLATFORM_WINDOWS_MSVC)
using DWORD = unsigned int;
#endif

namespace sprocketRealtimeScheduler {

const int MAX_FRAMES = 32;

} // sprocketRealtimeScheduler

#endif  // CONSTANTS_H_
