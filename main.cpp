#include <atomic>
#include <thread>
#include <vector>

#include "metricsCollector.h"
#include "utils.h"

void demonstrate(int durationSeconds) {
    MetricsCollector collector("../metrics.log");
    
    collector.registerMetric<float>("cpu_usage");
    collector.registerMetric<int>("http_rps");
    collector.registerMetric<int>("global_counter");
    collector.registerMetric<int>("thread_num");
    
    collector.start();
    
    std::vector<std::thread> threads;
    threads.reserve(4);
    
    auto startTime = std::chrono::steady_clock::now();
    
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&collector, i, startTime, durationSeconds]() {
            while (std::chrono::steady_clock::now() - startTime < std::chrono::seconds(durationSeconds)) {
                collector.setMetric("cpu_usage", simulateCPU(4));
                collector.setMetric("http_rps", simulateRPS(100, 300));
                collector.incrementMetric<int>("global_counter");
                collector.setMetric("thread_num", i + 1);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(durationSeconds));
    collector.stop();
    
    for (auto &t : threads) t.join();
}

int main() {
    demonstrate(20);
    return 0;
}