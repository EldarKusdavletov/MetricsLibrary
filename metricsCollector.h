#ifndef METRICSLIBRARY_METRICSCOLLECTOR_H
#define METRICSLIBRARY_METRICSCOLLECTOR_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

#include "metric.h"

class MetricsCollector {
private:
    std::unordered_map<std::string, std::unique_ptr<BaseMetric>> metrics;
    mutable std::shared_mutex metricsMutex;
    std::mutex fileMutex;
    std::ofstream outputFile;
    
    static std::string currentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
        
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&in_time), "%F %T");
        oss << "." << std::setfill('0') << std::setw(3) << ms;
        return oss.str();
    }

public:
    explicit MetricsCollector(const std::string &file) {
        outputFile.open(file, std::ios::trunc);
        if (!outputFile.is_open()) throw std::runtime_error("Cannot open metrics file");
    }
    
    template<Arithmetic T>
    void registerMetric(const std::string &name) {
        std::unique_lock lock(metricsMutex);
        metrics[name] = std::make_unique<Metric<T >>();
    }
    
    template<Arithmetic T>
    void setMetric(const std::string &name, T value) {
        std::unique_lock lock(metricsMutex);
        auto *metric = dynamic_cast<Metric<T> *>(metrics.at(name).get());
        if (!metric) throw std::runtime_error("Metric type mismatch: " + name);
        metric->setValue(value);
    }
    
    template<Arithmetic T>
    void incrementMetric(const std::string &name, T delta = 1) {
        std::unique_lock lock(metricsMutex);
        auto *metric = dynamic_cast<Metric<T> *>(metrics.at(name).get());
        if (!metric) throw std::runtime_error("Metric type mismatch: " + name);
        metric->increment(delta);
    }
    
    void recordAll() {
        std::shared_lock lock(metricsMutex);
        std::lock_guard flock(fileMutex);
        outputFile << currentTimestamp();
        for (const auto &[name, metric]: metrics)
            outputFile << " \"" << name << "\": " << metric->getValue();
        outputFile << std::endl;
    }
    
    void resetAll() {
        std::unique_lock lock(metricsMutex);
        for (auto &[_, metric]: metrics)
            metric->reset();
    }
    
    void debugPrint() const {
        std::shared_lock lock(metricsMutex);
        std::cout << "=== Metrics Snapshot ===\n";
        for (const auto &[name, metric]: metrics)
            std::cout << "  " << name << ": " << metric->getValue() << "\n";
        std::cout << std::endl;
    }
};

#endif //METRICSLIBRARY_METRICSCOLLECTOR_H
