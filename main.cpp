#include <iostream>
#include <vector>
#include "framebuffer.h"
#include "point.h"
#include "color.h"
#include <fstream>
#include <string>
#include <vector>
#include <cmath>


struct Vertex2 {
    float x;
    float y;

    Vertex2(float xVal, float yVal) : x(xVal), y(yVal) {}
};

void writeInt(std::ofstream& file, int value) {
  file.put(value & 0xFF);
  file.put((value >> 8) & 0xFF);
  file.put((value >> 16) & 0xFF);
  file.put((value >> 24) & 0xFF);
}


// Function to write BMP image file
void renderBuffer()
{
    std::ofstream file("out.bmp", std::ios::binary);

    // BMP file header
    unsigned char fileHeader[] = {
        'B', 'M',  // Signature
        0, 0, 0, 0, // File size (will be filled later)
        0, 0,      // Reserved
        0, 0,      // Reserved
        54, 0, 0, 0 // Pixel data offset
    };

    // BMP info header
    unsigned char infoHeader[] = {
        40, 0, 0, 0,  // Info header size
        0, 0, 0, 0,   // Image width (will be filled later)
        0, 0, 0, 0,   // Image height (will be filled later)
        1, 0,         // Number of color planes
        24, 0,        // Bits per pixel (3 bytes)
        0, 0, 0, 0,   // Compression method
        0, 0, 0, 0,   // Image size (will be filled later)
        0, 0, 0, 0,   // Horizontal resolution (pixels per meter, not used)
        0, 0, 0, 0,   // Vertical resolution (pixels per meter, not used)
        0, 0, 0, 0,   // Number of colors in the palette (not used)
        0, 0, 0, 0    // Number of important colors (not used)
    };

    // Calculate some values
    size_t imageSize = screenWidth * screenHeight * 3;  // 3 bytes por píxel (BGR)
    int fileSize = imageSize + sizeof(fileHeader) + sizeof(infoHeader);

    // Fill in the file header
    *(int*)&fileHeader[2] = fileSize;          // File size
    *(int*)&fileHeader[10] = sizeof(fileHeader) + sizeof(infoHeader);  // Pixel data offset

    // Fill in the info header
    *(int*)&infoHeader[4] = screenWidth;             // Image width
    *(int*)&infoHeader[8] = screenHeight;            // Image height
    *(int*)&infoHeader[20] = imageSize;        // Image size

    // Write the headers to the file
    file.write(reinterpret_cast<char*>(fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<char*>(infoHeader), sizeof(infoHeader));

    // Write the pixel data
       // Datos de píxeles
    for (const auto& color : framebuffer) {
        file.put(color.b);
        file.put(color.g);
        file.put(color.r);
    }

    // Close the file
    file.close();
}


void point(const Vertex2& vertex, const Color& color) {
  if (vertex.x >= 0 && vertex.x < screenWidth && vertex.y >= 0 && vertex.y < screenHeight) {
    int pixelIndex = static_cast<int>(vertex.y) * screenWidth + static_cast<int>(vertex.x);
    framebuffer[pixelIndex] = color;
  }
}


void point2(int x, int y, const Color& color) {
    if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) {
        int pixelIndex = y * screenWidth + x;
        framebuffer[pixelIndex] = color;
    }
}

void drawLine(Vertex2 start, Vertex2 end, const Color& color) {
    int x0 = static_cast<int>(std::round(start.x));
    int y0 = static_cast<int>(std::round(start.y));
    int x1 = static_cast<int>(std::round(end.x));
    int y1 = static_cast<int>(std::round(end.y));

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        point2(x0, y0, color);

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void drawPolygon(const std::vector<Vertex2>& points, const Color& color) {
  if (points.size() < 2) {
    return;  // El polígono debe tener al menos 2 puntos
  }

  // Dibujar todas las líneas del polígono
  for (size_t i = 0; i < points.size() - 1; ++i) {
    drawLine(points[i], points[i + 1], color);
  }

  // Dibujar línea desde el último punto hasta el primer punto
  drawLine(points.back(), points.front(), color);
}




void fillPolygon(const std::vector<Vertex2>& vertices, const Color& color) {
  if (vertices.size() < 3) {
    return;  // El polígono debe tener al menos 3 vértices para poder rellenarlo
  }

  // Encontrar los límites del polígono (mínimo y máximo en X y Y)
  float minX = vertices[0].x;
  float maxX = vertices[0].x;
  float minY = vertices[0].y;
  float maxY = vertices[0].y;

  for (size_t i = 1; i < vertices.size(); ++i) {
    minX = std::min(minX, vertices[i].x);
    maxX = std::max(maxX, vertices[i].x);
    minY = std::min(minY, vertices[i].y);
    maxY = std::max(maxY, vertices[i].y);
  }

  // Iterar sobre cada píxel dentro de los límites y verificar si está dentro del polígono
  for (float y = minY; y <= maxY; ++y) {
    for (float x = minX; x <= maxX; ++x) {
      int crossings = 0;

      for (size_t i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++) {
        if (((vertices[i].y <= y) && (y < vertices[j].y)) || ((vertices[j].y <= y) && (y < vertices[i].y))) {
          if (x < ((vertices[j].x - vertices[i].x) * (y - vertices[i].y) / (vertices[j].y - vertices[i].y)) + vertices[i].x) {
            ++crossings;
          }
        }
      }

      if (crossings % 2 != 0) {
        point(Vertex2(x, y), color);
      }
    }
  }
}



void render() {
  std::vector<Vertex2> polygon = {
      Vertex2(413.0f, 177.0f),
      Vertex2(448.0f, 159.0f),
      Vertex2(502.0f, 88.0f),
      Vertex2(553.0f, 53.0f),
      Vertex2(535.0f, 36.0f),
      Vertex2(676.0f, 37.0f),
      Vertex2(660.0f, 52.0f),
      Vertex2(750.0f, 145.0f),
      Vertex2(761.0f, 179.0f),
      Vertex2(672.0f, 192.0f),
      Vertex2(659.0f, 214.0f),
      Vertex2(615.0f, 214.0f),
      Vertex2(632.0f, 230.0f),
      Vertex2(580.0f, 230.0f),
      Vertex2(597.0f, 215.0f),
      Vertex2(552.0f, 214.0f),
      Vertex2(517.0f, 144.0f),
      Vertex2(466.0f, 180.0f)
  };

  Color polygonColor(255, 255, 255);  // Color blanco para el polígono
  Color polygonFill(0,255,0);
  fillPolygon(polygon, polygonFill);
  drawPolygon(polygon, polygonColor);

  renderBuffer();

  std::vector<Vertex2> polygon5 = {
      Vertex2(682.0f, 175.0f),
      Vertex2(708.0f, 120.0f),
      Vertex2(735.0f, 148.0f),
      Vertex2(739.0f, 170.0f)
  };
  Color polygonColor5(255, 255, 255);  // Color blanco para el polígono
  Color polygonFill5(0,0,0);
  fillPolygon(polygon5, polygonFill5);
  drawPolygon(polygon5, polygonColor5);

  renderBuffer();
}

int main() {
  clear();
  render();
  return 0;
}
