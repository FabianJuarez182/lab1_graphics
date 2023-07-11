#pragma once
#include "color.h"
#include <iostream>

struct Vertex {
  float x;
  float y;
  Color color;

  Vertex(float xCoord, float yCoord, const Color& c) : x(xCoord), y(yCoord), color(c) {}
};
