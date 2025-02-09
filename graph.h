#include "lodepng.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iostream>

int number_rasters[10][5] = {
    {
        0b11111,
        0b10001,
        0b10001,
        0b10001,
        0b11111,
    },

    {
        0b00100,
        0b01100,
        0b00100,
        0b00100,
        0b11111,
    },
    {
        0b11111,
        0b00001,
        0b11111,
        0b10000,
        0b11111,
    },
    {
        0b11111,
        0b00001,
        0b11111,
        0b00001,
        0b11111,
    },
    {
        0b10001,
        0b10001,
        0b11111,
        0b00001,
        0b00001,
    },
    {
        0b11111,
        0b10000,
        0b11111,
        0b00001,
        0b11111,
    },
    {
        0b11111,
        0b10000,
        0b11111,
        0b10001,
        0b11111,
    },
    {
        0b11111,
        0b10001,
        0b00010,
        0b00100,
        0b00100,
    },
    {
        0b11111,
        0b10001,
        0b11111,
        0b10001,
        0b11111,
    },
    {
        0b11111,
        0b10001,
        0b11111,
        0b00001,
        0b11111,
    },

};

void pixel(std::vector<unsigned char> &image, int width, int height, int x,
           int y, unsigned char r, unsigned char g, unsigned char b,
           unsigned char a) {
  if (x >= 0 && x < width && y >= 0 && y < height) {
    image[4 * width * y + 4 * x + 0] = r;
    image[4 * width * y + 4 * x + 1] = g;
    image[4 * width * y + 4 * x + 2] = b;
    image[4 * width * y + 4 * x + 3] = a;
  }
}

void draw_rect(std::vector<unsigned char> &image, int width, int height, int x,
               int y, int w, int h, unsigned char r, unsigned char g,
               unsigned char b, unsigned char a) {
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      pixel(image, width, height, x + i, y + j, r, g, b, a);
    }
  }
}

void draw_circle(std::vector<unsigned char> &image, int width, int height,
                 int x, int y, int radius, unsigned char r, unsigned char g,
                 unsigned char b, unsigned char a) {
  for (int i = -radius; i <= radius; i++) {
    for (int j = -radius; j <= radius; j++) {
      if (i * i + j * j <= radius * radius) {
        int x_ = x + i;
        int y_ = y + j;
        if (x_ >= 0 && x_ < width && y_ >= 0 && y_ < height) {
          pixel(image, width, height, x_, y_, r, g, b, a);
        }
      }
    }
  }
}

void draw_line(std::vector<unsigned char> &image, int width, int height, int x1,
               int y1, int x2, int y2, unsigned char r, unsigned char g,
               unsigned char b, unsigned char a) {
  int dx = x2 - x1;
  int dy = y2 - y1;
  int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
  float xinc = dx / (float)steps;
  float yinc = dy / (float)steps;
  float x = x1;
  float y = y1;
  for (int i = 0; i <= steps; i++) {
    int x_ = x;
    int y_ = y;
    if (x_ >= 0 && x_ < width && y_ >= 0 && y_ < height) {
      pixel(image, width, height, x_, y_, r, g, b, a);
    }
    x += xinc;
    y += yinc;
  }
}

void draw_number(std::vector<unsigned char> &image, int width, int height,
                 int x, int y, int number, unsigned char r, unsigned char g,
                 unsigned char b, unsigned char a) {
  int *raster = number_rasters[number];
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      if (raster[i] & (1 << j)) {

        pixel(image, width, height, x + (4 - j), y + i, r, g, b, a);
      }
    }
  }
}

std::vector<unsigned char> generate_png(int width, int height,
                                        std::vector<int> data) {
  std::chrono::time_point start = std::chrono::high_resolution_clock::now();

  int size = width * height;
  int vertical_offset = 50;
  int horizontal_offset = 50;
  for (int i = 0; i < data.size(); i++) {
    data[i] += vertical_offset;
  }

  std::vector<unsigned char> image(size * 4);
  // clear image with black
  for (int i = 0; i < size; i++) {
    pixel(image, width, height, i % width, i / width, 0, 0, 0, 255);
  }

  // draw x and y axis

  int o = 10; // offset
  for (int o = 10; o < 13; o++) {
    draw_line(image, width, height, o, o, width - o, o, 255, 255, 255, 255);
    draw_line(image, width, height, width - o, o, width - o, height - o, 255,
              255, 255, 255);
  }

  // draw lines from datapoint to datapoint
  for (int i = 0; i < data.size() - 1; i++) {
    int x1 = i * width / data.size() + horizontal_offset;
    int y1 = height - data[i];
    int x2 = (i + 1) * width / data.size() + horizontal_offset;
    int y2 = height - data[i + 1];
    draw_line(image, width, height, x1, y1, x2, y2, 155, 155, 155, 255);
    // draw line to indicate the day on the x axis
    draw_line(image, width, height, x1, height - 10, x1, height - 15, 255, 255,
              255, 255);
  }

  // draw circles at places of datapoints
  for (int i = 0; i < data.size(); i++) {
    int x = i * width / data.size() + horizontal_offset;
    int y = height - data[i];
    draw_circle(image, width, height, x, y, 4, 255, 255, 255, 255);
    draw_circle(image, width, height, x, y, 3, 255, 75, 75, 255);
  }

  // draw numbers
  for (int i = 0; i < data.size(); i++) {
    int o = 10; // digit offset
    int font_size = 5;
    int gap = 2;
    int x = i * width / data.size() + horizontal_offset;
    int y = height - data[i];
    int number = data[i] - vertical_offset;
    if (number < 10) {
      draw_rect(image, width, height, x + o - 1, y + o - 1, font_size + 2,
                o + 2, 255, 255, 255, 255);
      draw_number(image, width, height, x + o, y + o, number, 0, 0, 0, 255);
    }
    if (number < 100) {
      draw_rect(image, width, height, x + o - 1, y + o - 1,
                gap + font_size * 2 + 2, font_size + 2, 255, 255, 255, 255);
      draw_number(image, width, height, x + o, y + o, number / 10, 0, 0, 0,
                  255);
      draw_number(image, width, height, x + o + gap + font_size, y + o,
                  number % 10, 0, 0, 0, 255);
    } else {
      draw_rect(image, width, height, x + o - 1, y + o - 1,
                font_size * 3 + gap * 2 + 2, font_size + 2, 255, 255, 255, 255);
      draw_number(image, width, height, x + 1 * o, y + o, number / 100, 0, 0, 0,
                  255);
      draw_number(image, width, height, x + o + gap + font_size, y + o,
                  (number / 10) % 10, 0, 0, 0, 255);
      draw_number(image, width, height, x + o + gap * 2 + font_size * 2, y + o,
                  number % 10, 0, 0, 0, 255);
    }
  }

  std::vector<unsigned char> png;
  unsigned error = lodepng::encode(png, image, width, height);
  if (error) {
    std::cerr << "encoder error " << error << ": " << lodepng_error_text(error)
              << std::endl;
  }
  std::chrono::time_point end = std::chrono::high_resolution_clock::now();
  long nanos =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  float sec = (float)nanos / 1e9;

  debug("Generated PNG in " + std::to_string(sec) + " seconds");

  return png;
}

std::vector<unsigned char> rescale(const std::vector<unsigned char> &in, int w0,
                                   int h0, int w1, int h1, bool smooth) {
  int numchannels = in.size() / (w0 * h0);
  std::vector<unsigned char> out(w1 * h1 * numchannels);
  if (smooth) {
    // box filter.
    std::vector<unsigned char> temp(w1 * h0 * numchannels);
    for (int c = 0; c < numchannels; c++) {
      for (int x = 0; x < w1; x++) {
        float xaf = x * 1.0 * w0 / w1;
        float xbf = (x + 1.0) * w0 / w1;
        int xa = (int)xaf;
        int xb = (int)xbf;
        double norm = 1.0 / (xbf - xaf);
        xaf -= std::floor(xaf);
        xbf -= std::floor(xbf);
        for (int y = 0; y < h0; y++) {
          int index1 = x * numchannels + y * w1 * numchannels;
          double val = 0;
          for (int x0 = xa; x0 <= xb; x0++) {
            int index0 = x0 * numchannels + y * w0 * numchannels;
            double v = 1;
            if (x0 == xa)
              v -= xaf;
            if (x0 == xb)
              v -= (1 - xbf);
            val += v * in[index0 + c];
          }
          temp[index1 + c] = val * norm;
        }
      }
      for (int y = 0; y < h1; y++) {
        float yaf = y * 1.0 * h0 / h1;
        float ybf = (y + 1.0) * h0 / h1;
        int ya = (int)yaf;
        int yb = (int)ybf;
        double norm = 1.0 / (ybf - yaf);
        yaf -= std::floor(yaf);
        ybf -= std::floor(ybf);
        for (int x = 0; x < w1; x++) {
          int index1 = x * numchannels + y * w1 * numchannels;
          double val = 0;
          for (int y0 = ya; y0 <= yb; y0++) {
            int index0 = x * numchannels + y0 * w1 * numchannels;
            double v = 1;
            if (y0 == ya)
              v -= yaf;
            if (y0 == yb)
              v -= (1 - ybf);
            val += v * temp[index0 + c];
          }
          out[index1 + c] = val * norm;
        }
      }
    }
  } else {
    for (int y = 0; y < h1; y++) {
      int y0 = (int)((y + 0.5) * h0 / h1 - 0.5);
      for (int x = 0; x < w1; x++) {
        int x0 = (int)((x + 0.5) * w0 / w1 - 0.5);
        int index0 = x0 * numchannels + y0 * w0 * numchannels;
        int index1 = x * numchannels + y * w1 * numchannels;
        for (int c = 0; c < numchannels; c++) {
          out[index1 + c] = in[index0 + c];
        }
      }
    }
  }
  return out;
}
