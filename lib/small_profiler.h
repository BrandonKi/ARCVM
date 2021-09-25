/**
 * @file small_profiler.h
 * @author Brandon Kirincich
 * @brief small inline profiler that generates a json usable with chrome://tracing
 * @version 0.1
 * @date 2021-03-28
 *
 * @copyright
 * MIT License
 *
 * Copyright (c) 2021 Brandon Kirincich
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 */
#ifndef SMALL_PROFILER_H
#define SMALL_PROFILER_H

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <string>
#include <sstream>
#include <vector>
#include <array>

#if defined(__linux__)
#include <sys/types.h>
#include <unistd.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

#ifndef PROFILE_OUTPUT_FILE
#define PROFILE_OUTPUT_FILE "profile.json"
#endif

#define CONCAT(a, b) CONCAT_(a, b)
#define CONCAT_(a, b) a##b

#ifdef NO_PROFILE
    #define PROFILE() (static_cast<void>(0))
    #define PROFILE_SCOPE(x) PROFILE()
#else
    #define PROFILE() \
        small_profiler::internal_scoped_profiler CONCAT(_small_profiler_temp_, __COUNTER__) { __FUNCTION__ }
    #define PROFILE_SCOPE(x) \
        small_profiler::internal_scoped_profiler CONCAT(_small_profiler_temp_, __COUNTER__) { x }

namespace small_profiler {

    inline unsigned long long get_pid() {
        #if defined(__linux__)
        return getpid();
        #elif defined(_WIN32)
        return GetCurrentProcessId();
        #else
        return static_cast<unsigned long long>(-1);
        #endif
    }

    struct scope_info {
        const std::string name;
        const std::array<long long, 5> data;
    };

    class internal_stream_wrapper {
    public:
        std::stringstream stream{PROFILE_OUTPUT_FILE};
        std::vector<scope_info> scope_table{};

        internal_stream_wrapper() {
            stream << "{ \"traceEvents\": [";
        }

        ~internal_stream_wrapper() {

            for (const auto &entry : scope_table) {
                const auto line = std::string(
                                      R"({ "pid":)") +
                                  std::to_string(entry.data[0]) + std::string(R"(, "tid":)") + std::to_string(entry.data[1]) +
                                  std::string(R"(, "ts":)") + std::to_string(entry.data[2]) + std::string(R"(, "dur":)") + std::to_string(entry.data[3]) +
                                  std::string(R"(, "ph":"X", "name":")") + entry.name +
                                  std::string(R"(", "args":{ "ms":)") + std::to_string(entry.data[4]) + std::string("} },");
                stream << line;
            }

            stream.seekp(-1, std::ios_base::end);
            stream << "]}";
            std::ofstream out{PROFILE_OUTPUT_FILE};
            out << stream.str();
        }

        internal_stream_wrapper(const internal_stream_wrapper &) = delete;
        internal_stream_wrapper &operator=(const internal_stream_wrapper &) = delete;
        internal_stream_wrapper(const internal_stream_wrapper &&) = delete;
        internal_stream_wrapper &operator=(internal_stream_wrapper &&) = delete;
    };

    inline internal_stream_wrapper file;
    inline auto program_start = std::chrono::high_resolution_clock::now();

    class internal_scoped_profiler {
    public:
        explicit internal_scoped_profiler(std::string name_p):
            name_(std::move(name_p)), begin_(std::move(std::chrono::high_resolution_clock::now()))
        {

        }

        ~internal_scoped_profiler() {
            const auto end = std::chrono::high_resolution_clock::now();

            const auto pid = static_cast<long long>(small_profiler::get_pid());
            std::stringstream ss;
            ss << std::this_thread::get_id();
            const auto tid = static_cast<long long>(std::stoull(ss.str()));
            const auto ts = std::chrono::duration_cast<std::chrono::microseconds>(begin_ - program_start).count();
            const auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - begin_).count();
            const auto args = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin_).count();

            file.scope_table.emplace_back(std::move(scope_info{std::move(name_), std::move(std::array<long long, 5>{pid, tid, ts, dur, args})}));
        }

        internal_scoped_profiler(const internal_scoped_profiler &) = default;
        internal_scoped_profiler &operator=(const internal_scoped_profiler &) = default;
        internal_scoped_profiler(internal_scoped_profiler &&) = default;
        internal_scoped_profiler &operator=(internal_scoped_profiler &&) = default;

    private:
        std::string name_;
        std::chrono::time_point<std::chrono::high_resolution_clock> begin_;
    };

};
#endif
#endif