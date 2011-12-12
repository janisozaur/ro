#ifndef COMPLEX_H
#define COMPLEX_H

#include <QVector2D>

class QDebug;

struct vec2D {
    float x;
    float y;
};

class Complex
{
public:
    Complex();
    Complex(const Complex &other);
    Complex(const float &real, const float &imaginary);

    static Complex fromPowerPhase(const float &power, const float &phase);

    float abs() const;
    float real() const;
    float imaginary() const;
    float phase() const;
    void setReal(const float &real);
    void setImaginary(const float &imaginary);

    Complex &operator =(const Complex &rhs);
    Complex &operator+=(const Complex &rhs);
    Complex &operator-=(const Complex &rhs);
    Complex &operator*=(const Complex &rhs);
    Complex &operator*=(const float &rhs);
    Complex &operator/=(const Complex &rhs);
    Complex &operator/=(const float &rhs);
    const Complex operator +(const Complex &rhs) const;
    const Complex operator -(const Complex &rhs) const;
    const Complex operator *(const Complex &rhs) const;
    const Complex operator *(const float &rhs) const;
    const Complex operator /(const Complex &rhs) const;
    const Complex operator /(const float &rhs) const;
    bool operator==(const Complex &rhs) const;
    bool operator!=(const Complex &rhs) const;
    friend QDebug operator<<(QDebug &stream, const Complex &rhs);

private:
    vec2D mNumber;
};

#endif // COMPLEX_H
