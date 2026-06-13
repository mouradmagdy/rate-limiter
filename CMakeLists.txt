cmake_minimum_required(VERSION 4.2)
project(rate_limiter)

set(CMAKE_CXX_STANDARD 20)

add_executable(rate_limiter main.cpp
        TokenBucket.cpp
        RateLimiter.cpp)
