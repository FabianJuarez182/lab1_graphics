#pragma once

#include <vector>

// Incluimos el archivo de encabezado "color.h" que contiene la definición de la clase Color
#include "color.h"

// Tamaño de la pantalla
constexpr size_t screenWidth = 800;
constexpr size_t screenHeight = 600;

// Vector unidimensional para representar el framebuffer
std::vector<Color> framebuffer(screenWidth * screenHeight);

// Color base del framebuffer
Color clearColor{0, 0, 0};

Color currentColor{255, 255, 255};

// Función para limpiar el framebuffer y llenarlo con el color clearColor
void clear() {
  std::vector<Color> framebuffer(screenWidth * screenHeight);
  std::fill(framebuffer.begin(), framebuffer.end(), clearColor);
}

