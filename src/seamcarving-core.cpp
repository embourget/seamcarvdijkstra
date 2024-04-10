#include "seamcarving-core.h"
using namespace seamcarvingcore;

#include <cmath>
#include <list>
#include <iostream>
#include <string>
#include <climits>

Logger Logger::print(std::string str) {
    if (verbose) {
        std::cout << str << std::endl;
    }
    return *this;
}
Logger Logger::push(std::string str) {
    if (verbose) {
        std::cout << str;
    }
    return *this;
}
Logger Logger::print(int i) {
    if (verbose) {
        std::cout << i << std::endl;
    }
    return *this;
}
Logger Logger::push(int i) {
    if (verbose) {
        std::cout << i;
    }
    return *this;
}
Logger Logger::endl() {
    if (verbose) {
        std::cout << std::endl;
    }
    return *this;
}

void to_grey(unsigned char* in, unsigned char* out, int channels, int size) {

    int reading_index = 0;
    for (int i = 0; i < size; i++) {
        *(out + i) = (unsigned char)(( *(in + reading_index)
                + *(in + reading_index + 1) + *(in + reading_index + 2))/3.0);
        reading_index += channels;
    }
}

unsigned char get_pixel(unsigned char* grey_img, int x, int y, int width, int height) {

  if (x < 0) { return 0; }
  if (x >= width) { return 0; }
  if (y < 0) { return 0; }
  if (y >= height) { return 0; }

  return  *(grey_img + (y * width) + x);
}

unsigned char sobel(unsigned char* grey_img, int x, int y, int width, int height) {
  
  int vertical_sobel = get_pixel(grey_img, x + 1, y + 1, width, height) 
                     + get_pixel(grey_img, x + 1, y - 1, width, height) 
                     + 2 * get_pixel(grey_img, x + 1, y, width, height) 
                     - get_pixel(grey_img, x - 1, y + 1, width, height) 
                     - 2 * get_pixel(grey_img, x - 1, y, width, height) 
                     - get_pixel(grey_img, x - 1, y - 1, width, height) 
                     ;

  int horizontal_sobel = get_pixel(grey_img, x + 1, y + 1, width, height) 
                       + get_pixel(grey_img, x - 1, y + 1, width, height) 
                       + 2 * get_pixel(grey_img, x, y + 1, width, height) 
                       - get_pixel(grey_img, x + 1, y - 1, width, height) 
                       - 2 * get_pixel(grey_img, x, y - 1, width, height) 
                       - get_pixel(grey_img, x - 1, y - 1, width, height) 
                       ;
  
  return (unsigned char) pow( pow( vertical_sobel, 2) + pow( horizontal_sobel, 2), 0.5);

}

void to_energies(unsigned char* grey_img, int width, int height) {
    int size = width * height;
    unsigned char* buf = new unsigned char[size];
    std::fill(buf, buf + size, 0);
    int index = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            *(buf + index) = sobel(grey_img, x, y, width, height);
            index++;
        }
    }

    for (int i = 0; i < size; i++) {
        *(grey_img + i) = *(buf + i);
    }
    delete[] buf;
}

void insert(std::list<int>* queue, int to_insert, int* cost) {
  
  if (queue->empty()) {
    queue->push_front(to_insert);
  } else {
    int new_cost = *(cost + to_insert);
    if (new_cost < *(cost + queue->front()) ) {
      queue->push_front(to_insert);
    } else {
      auto it = std::next(queue->begin(), 1);
      while (it != queue->end() && ( *(cost + *it) <= new_cost) ) {
        it = std::next(it, 1);
      }
      queue->insert(it, to_insert);
    }
  }
}

void dijkstra(unsigned char* energies, int width, int height, int* path, Logger log) {
    
    int size = width * height;

    int* cost = new int[size];
    char* origins = new char[size];
  
    log.print("allocate matrixes done.");
    std::fill(cost, cost + size, INT_MAX);
    std::fill(origins, origins + size, 0);

    log.print("initialise matrixes done.");
    std::list<int> v{};
    for (int i = 0; i < width; i++) {
      *(cost + i) = *(energies + i);
      insert(&v, i, cost);
    }
    log.print("dijkstra initialise done.");
    log.push(v.size()).print(" elements in queue");
    
    bool stop = false;
    int x, y;
    int index;
    while (!stop && !v.empty()) {
      index = v.front();
      v.pop_front();

      x = index % width;
      y = index / width;
      int prev_cost, new_index, new_cost;
      
      if ( y == height - 1 ) {
        stop = true;
      } else {
        
        prev_cost = *(cost + index);
        for (int j = -1; j < 2; j++) {
          if (0 <= x + j && x + j < width) {
            new_index = index + width + j;
            new_cost = prev_cost + *(energies + new_index);
            if (new_cost < *(cost + new_index)) {
              *(cost + new_index) = new_cost;
              *(origins + new_index) = j;
              insert(&v, new_index, cost);
            }
          }
        }
        
      }
      
    }
    log.print("dijkstra exploration done.");
    
    
    int min_last_line = INT_MAX;
    index = 0;
    int last_line_index = size - width;
    for (int i = 0; i < width; i ++) {
      if (*(cost + last_line_index + i) < min_last_line) {
        min_last_line = *(cost + last_line_index + i);
        index = i;
      }
    }
    delete[] cost;
    for (int i = height - 1; i >= 0; i--) {
      *(path + i) = index;
      index = index - *(origins + (i * width) + index);
    }
    delete[] origins;
    log.print("dijkstra done.");
    
}

void cut(unsigned char* in, unsigned char* out, 
    int height, int width, int channels, int* path) {

    int in_index = 0;
    int out_index = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (*(path + y) != x) {
                for (int i = 0; i < channels; i++) {
                    *(out + out_index + i) = *(in + in_index + i);
                }
                out_index += channels;
            }
            in_index += channels;
        }
    }
}

//' apply Seam-Carving logic
//'
//' @description core logic function, to apply Seam-Carving to an image
//' @description not to be used directly,
//' @description either use functions in seamcarving-rcpp, or include in other module
//' @param[in] in unsigned char* containing the input image
//' @param[out] out unsigned char* for the output image
//' @param height int number of lines
//' @param width int number of columns
//' @param channels int number of bytes per pixel
//' @param log seamcarvingcore::Logger for logging progress
void seamcarvingcore::treat(unsigned char* in, unsigned char* out, 
            int height, int width, int channels, Logger log) {
    
    int size = width * height;

    unsigned char* grey_img = new unsigned char[size];
    if (grey_img == NULL) {
        log.push("error: unable to allocate array grey_img with size: ").print(size);
        exit(1);
    }
    std::fill(grey_img, grey_img + size, 0);
    to_grey(in, grey_img, channels, size);
    
    to_energies(grey_img, width, height);

    int* path = new int[height];
    if (path == NULL) {
        log.push("error: unable to allocate array path with size: ").print(height);
        exit(1);
    }
    std::fill(path, path + height, 0);
    dijkstra(grey_img, width, height, path, log);
    delete[] grey_img;

    cut(in, out, height, width, channels, path);
    delete[] path;
}