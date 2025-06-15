cmake_minimum_required(VERSION 3.30)
project(MetricsLibrary)

set(CMAKE_CXX_STANDARD 23)

add_executable(MetricsLibrary main.cpp
        metric.h
        metricsCollector.h
        utils.cpp
        utils.h)
