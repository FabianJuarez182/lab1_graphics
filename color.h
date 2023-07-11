#pragma once
#include <iostream>

class Color {
public:
  uint8_t r;
  uint8_t g;
  uint8_t b;

  Color() : r(0), g(0), b(0){};
  Color(uint8_t red, uint8_t green, uint8_t blue) : r(red), g(green), b(blue) {}

  Color operator+(const Color& other) const {
    uint8_t newRed = static_cast<uint8_t>(r + other.r);
    uint8_t newGreen = static_cast<uint8_t>(g + other.g);
    uint8_t newBlue = static_cast<uint8_t>(b + other.b);

    // Clamping values to the range [0, 255]
    if (newRed < 0)
      newRed = 0;
    else if (newRed > 255)
      newRed = 255;

    if (newGreen < 0)
      newGreen = 0;
    else if (newGreen > 255)
      newGreen = 255;

    if (newBlue < 0)
      newBlue = 0;
    else if (newBlue > 255)
      newBlue = 255;

    return Color(newRed, newGreen, newBlue);
  }

  Color operator*(float scalar) const {
    uint8_t newRed = static_cast<uint8_t>(r * scalar);
    uint8_t newGreen = static_cast<uint8_t>(g * scalar);
    uint8_t newBlue = static_cast<uint8_t>(b * scalar);

    // Clamping values to the range [0, 255]
    if (newRed < 0)
      newRed = 0;
    else if (newRed > 255)
      newRed = 255;

    if (newGreen < 0)
      newGreen = 0;
    else if (newGreen > 255)
      newGreen = 255;

    if (newBlue < 0)
      newBlue = 0;
    else if (newBlue > 255)
      newBlue = 255;

    return Color(newRed, newGreen, newBlue);
  }

  friend std::ostream& operator<<(std::ostream& os, const Color& color) {
    os << "RGB: (" << static_cast<int>(color.r) << ", " << static_cast<int>(color.g) << ", " << static_cast<int>(color.b) << ")";
    return os;
  }
};
