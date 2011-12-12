#include "Complex.h"

#include <cmath>
#include <QDebug>

/*
 * see http://www.cs.caltech.edu/courses/cs11/material/cpp/donnie/cpp-ops.html
 */

Complex::Complex()
{
}

Complex::Complex(const Complex &other)
{
    this->mNumber = other.mNumber;
}

Complex::Complex(const float &real, const float &imaginary) :
    mNumber({real, imaginary})
{
}

Complex Complex::fromPowerPhase(const float &power, const float &phase)
{
    return Complex(power * cos(phase), power * sin(phase));
}

float Complex::abs() const
{

    return std::sqrt(mNumber.x * mNumber.x + mNumber.y * mNumber.y);
}

float Complex::phase() const
{
    // http://en.wikipedia.org/wiki/Atan2
    float result = atan2(imaginary(), real());
    return result;
}

float Complex::real() const
{
    return mNumber.x;
}

float Complex::imaginary() const
{
    return mNumber.y;
}

void Complex::setReal(const float &real)
{
    mNumber.x = (real);
}

void Complex::setImaginary(const float &imaginary)
{
    mNumber.y = (imaginary);
}

Complex &Complex::operator =(const Complex &rhs)
{
    // it is safe to skip self-assignment check here
    this->mNumber = rhs.mNumber;
    return *this;
}

Complex &Complex::operator+=(const Complex &rhs)
{
    this->mNumber.x += rhs.mNumber.x;
    this->mNumber.y += rhs.mNumber.y;
    return *this;
}

Complex &Complex::operator-=(const Complex &rhs)
{
    this->mNumber.x -= rhs.mNumber.x;
    this->mNumber.y -= rhs.mNumber.y;
    return *this;
}

Complex &Complex::operator*=(const Complex &rhs)
{
    // http://en.wikipedia.org/wiki/Complex_number#Multiplication_and_division
    vec2D result;
    result.x = (real() * rhs.real() - imaginary() * rhs.imaginary());
    result.y = (imaginary() * rhs.real() + real() * rhs.imaginary());
    this->mNumber = result;
    return *this;
}

Complex &Complex::operator*=(const float &rhs)
{
    this->mNumber.x *= rhs;
    this->mNumber.y *= rhs;
    return *this;
}

Complex &Complex::operator/=(const Complex &rhs)
{
    // http://en.wikipedia.org/wiki/Complex_number#Multiplication_and_division
    vec2D result;
    result.x = ((real() * rhs.real() + imaginary() * rhs.imaginary()) /
                (rhs.real() * rhs.real() + rhs.imaginary() * rhs.imaginary()));
    result.y = ((imaginary() * rhs.real() - real() * rhs.imaginary()) /
                (rhs.real() * rhs.real() + rhs.imaginary() * rhs.imaginary()));
    this->mNumber = result;
    return *this;
}

Complex &Complex::operator/=(const float &rhs)
{
    this->mNumber.x /= rhs;
    this->mNumber.y /= rhs;
    return *this;
}

const Complex Complex::operator +(const Complex &rhs) const
{
    return Complex(*this) += rhs;
}

const Complex Complex::operator -(const Complex &rhs) const
{
    return Complex(*this) -= rhs;
}

const Complex Complex::operator *(const Complex &rhs) const
{
    return Complex(*this) *= rhs;
}

const Complex Complex::operator *(const float &rhs) const
{
    return Complex(*this) *= rhs;
}

const Complex Complex::operator /(const Complex &rhs) const
{
    return Complex(*this) /= rhs;
}

const Complex Complex::operator /(const float &rhs) const
{
    return Complex(*this) /= rhs;
}

bool Complex::operator ==(const Complex &rhs) const
{
    return (this->mNumber.x == rhs.mNumber.x && this->mNumber.y == rhs.mNumber.y);
}

bool Complex::operator !=(const Complex &rhs) const
{
    return (this->mNumber.x != rhs.mNumber.x || this->mNumber.y != rhs.mNumber.y);
}

QDebug operator <<(QDebug &stream, const Complex &rhs)
{
    stream << "Complex(" << rhs.mNumber.x << "," << rhs.mNumber.y << ")";
    return stream.nospace();
}
