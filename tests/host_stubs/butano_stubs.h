/**
 * Simple Butano stand-ins so host-side unit tests can compile production
 * helpers without pulling the real devkitARM dependencies.
 */
#pragma once

#include <cmath>
#include <utility>

namespace bn {

class fixed {
 public:
  constexpr fixed() = default;
  constexpr fixed(float value) : _value(value) {}
  constexpr fixed(int value) : _value(static_cast<float>(value)) {}

  constexpr operator float() const { return _value; }

  [[nodiscard]] constexpr int integer() const {
    return static_cast<int>(_value);
  }

  fixed& operator+=(fixed other) {
    _value += other._value;
    return *this;
  }

  fixed& operator-=(fixed other) {
    _value -= other._value;
    return *this;
  }

  fixed& operator*=(fixed other) {
    _value *= other._value;
    return *this;
  }

  fixed& operator/=(fixed other) {
    _value /= other._value;
    return *this;
  }

  friend fixed operator+(fixed lhs, fixed rhs) {
    return fixed(lhs._value + rhs._value);
  }

  friend fixed operator-(fixed lhs, fixed rhs) {
    return fixed(lhs._value - rhs._value);
  }

  friend fixed operator*(fixed lhs, fixed rhs) {
    return fixed(lhs._value * rhs._value);
  }

  friend fixed operator*(fixed lhs, int rhs) { return fixed(lhs._value * rhs); }

  friend fixed operator*(int lhs, fixed rhs) { return fixed(lhs * rhs._value); }

  friend fixed operator/(fixed lhs, fixed rhs) {
    return fixed(lhs._value / rhs._value);
  }

 private:
  float _value = 0;
};

template <typename First, typename Second>
using pair = std::pair<First, Second>;

class fixed_point {
 public:
  constexpr fixed_point(fixed x = 0, fixed y = 0) : _x(x), _y(y) {}

  [[nodiscard]] constexpr fixed x() const { return _x; }
  [[nodiscard]] constexpr fixed y() const { return _y; }

 private:
  fixed _x;
  fixed _y;
};

inline fixed abs(fixed value) {
  return fixed(std::fabs(static_cast<float>(value)));
}

inline fixed degrees_atan2(int y, int x) {
  constexpr float kRadToDeg = 180.0f / 3.1415926535f;
  return fixed(std::atan2(static_cast<float>(y), static_cast<float>(x)) *
               kRadToDeg);
}

inline pair<fixed, fixed> degrees_sin_and_cos(fixed degrees) {
  constexpr float kDegToRad = 3.1415926535f / 180.0f;
  float radians = static_cast<float>(degrees) * kDegToRad;
  return {fixed(std::sin(radians)), fixed(std::cos(radians))};
}

}  // namespace bn
