#include <atomic>
#include <thread>
#include <vector>

#include "metricsCollector.h"
#include "utils.h"

void demonstrate() {
    MetricsCollector collector("../metrics.log");
    
    collector.registerMetric<float>("cpu_usage");
    collector.registerMetric<int>("http_rps");
    collector.registerMetric<int>("global_counter");
    
    std::vector<std::thread> threads;
    
    threads.reserve(4);
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&collector]() {
            for (int j = 0; j < 100; ++j) {
                collector.setMetric("cpu_usage", simulateCPU(4));
                collector.setMetric("http_rps", simulateRPS(100, 300));
                collector.incrementMetric<int>("global_counter");
                collector.recordAll();
                collector.debugPrint();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }
    
    for (auto &t: threads) t.join();
    collector.resetAll();
}

int main() {
    demonstrate();
    return 0;
}
