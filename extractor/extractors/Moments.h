#ifndef MOMENTS_H
#define MOMENTS_H

#include <QVector>

class Moments
{
public:
    Moments();
    float mean(const QVector<float> &values) const;
    float weightedMean(const QVector<float> &values) const;
    float variance(const QVector<float> &values, const float &_mean) const;
    float moment(const QVector<float> &values, const int &power,
                 const float &_mean) const;
    float skewness(const QVector<float> &values, const float &_mean,
                   const float &_variance) const;
    float kurtosis(const QVector<float> &values, const float &_mean, const float &_variance) const;
};

#endif // MOMENTS_H
