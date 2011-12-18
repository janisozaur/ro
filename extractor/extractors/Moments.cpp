#include "Moments.h"

#include <cmath>

Moments::Moments()
{
}

float Moments::mean(const QVector<float> &values) const
{
    float result = 0;
    const int size = values.size();
    for (int i = 0; i < size; i++) {
        result += values.at(i);
    }
    result /= size;
    return result;
}

float Moments::variance(const QVector<float> &values, const float &_mean) const
{
    return moment(values, 2, _mean);
}

float Moments::moment(const QVector<float> &values, const int &power,
                      const float &_mean) const
{
    float result = 0;
    const int size = values.size();
    for (int i = 0; i < size; i++) {
        const float diff = values.at(i) - _mean;
        float temp = 1;
        for (int j = 0; j < power; j++) {
            temp *= diff;
        }
        result += temp;
    }
    result /= size;
    return result;
}

float Moments::skewness(const QVector<float> &values, const float &_mean,
                        const float &_variance) const
{
    const float mi3 = moment(values, 3, _mean);
    const float den = sqrt(_variance * _variance * _variance);
    return mi3 / den;
}

float Moments::kurtosis(const QVector<float> &values, const float &_mean,
                        const float &_variance) const
{
    const float mi4 = moment(values, 4, _mean);
    const float den = sqrt(_variance);
    return mi4 / den - 3;
}

float Moments::weightedMean(const QVector<float> &values) const
{
    float result = 0;
    const int size = values.size();
    int weightSum = 0;
    for (int i = 0; i < size; i++) {
        const int weight = values.at(i);
        result += i * weight;
        weightSum += weight;
    }
    result /= weightSum;
    return result;
}
