#ifndef METRICSLIBRARY_METRIC_H
#define METRICSLIBRARY_METRIC_H

#include <iostream>

class BaseMetric {
public:
    virtual ~BaseMetric() = default;
    
    [[nodiscard]] virtual std::string getValue() const = 0;
    
    virtual void reset() = 0;
};

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template<Arithmetic T>
class Metric : public BaseMetric {
private:
    std::atomic<T> value{};

public:
    [[nodiscard]] std::string getValue() const override {
        return std::to_string(value.load());
    }
    
    void setValue(T val) {
        value.store(val, std::memory_order_relaxed);
    }
    
    void increment(T delta = 1) {
        value.fetch_add(delta, std::memory_order_relaxed);
    }
    
    void reset() override {
        value.store(0, std::memory_order_relaxed);
    }
};

#endif //METRICSLIBRARY_METRIC_H
