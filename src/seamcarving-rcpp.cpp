#include "seamcarving-rcpp.h"
using namespace seamcarving;

#include <iostream>
#include <chrono>
#include <string>

#include "seamcarving-core.h"
#include "Rcpp.h"
using namespace Rcpp;
using namespace seamcarvingcore;

void convert_input(const NumericVector& in_vec, unsigned char* in, int height, int width, int channels) {
    int index = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int i = 0; i < channels; i++) {
                *(in + index) = (int) (in_vec[ ( (i * width) + x ) * height + y ] * 255);
                index ++;
            }
        }
    }
}

//' Apply Seam-Carving image reduction to an image
//' @description Seam-Carving with Dijkstra path-finding
//' @description This is the C++ implementation, you can use the R convenience function treat( img, verbose )
//'
//' @param in_vec Rcpp::NumericVector input vector, with dimension c(height * width * channels)
//' @param height int number of rows in the image
//' @param width int number of columns in the image
//' @param channels int number of byte per pixel in the image
//' @param verbose bool set to true to print progress to console
//' @export
// [[Rcpp::export]]
NumericVector treat_img(const NumericVector& in_vec, int height, int width, int channels, bool verbose) {

    Logger log;
    log.verbose = verbose;
    log.push("received input vector with size ").print(in_vec.size());
    int in_size = height * width * channels;
    int out_size = height * (width - 1) * channels;
    if (in_vec.size() != in_size) {
        log.push("error: input vector size :").push(in_vec.size())
            .push(", expected: ").print(in_size);
        return NULL;
    }
    unsigned char* in = new unsigned char[in_size];
    if (in == NULL) {
        log.push("error: unable to allocate array in with size: ").print(in_size);
        return NULL;
    }
    std::fill(in, in + in_size, 0);
    convert_input(in_vec, in, height, width, channels);

    unsigned char* out = new unsigned char[out_size];
    if (out == NULL) {
        log.push("error: unable to allocate array out with size: ").print(out_size);
        return NULL;
    }
    std::fill(out, out + out_size, 0);
    NumericVector out_vec(out_size);

    treat(in, out, height, width, channels, log);
    delete[] in;
    int index = 0;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width - 1; x++) {
            for (int i = 0; i < channels; i++) {
                out_vec[ ( (i *( width - 1)) + x ) * height + y] = (float) (((int) (*(out + index))) / 255.0);
                index ++;
            }
        }
    }
    delete[] out;
    return out_vec;
}

//' Time the Seam-Carving image reduction of an image
//' @description Seam-Carving with Dijkstra path-finding
//' @description This is the C++ implementation, you can use the R convenience function timedrun( img, verbose )
//'
//' @param in_vec Rcpp::NumericVector input vector, with dimension c(height * width * channels)
//' @param height int number of rows in the image
//' @param width int number of columns in the image
//' @param channels int number of byte per pixel in the image
//' @param verbose bool set to true to print progress to console
//' @export
// [[Rcpp::export]]
double time_treat(const NumericVector& in_vec, int height, int width, int channels, bool verbose) {

    Logger log;
    log.verbose = verbose;
    log.push("received input vector with size ").print(in_vec.size());
    int in_size = height * width * channels;
    int out_size = height * (width - 1) * channels;
    if (in_vec.size() != in_size) {
        log.push("error: input vector size :").push(in_vec.size())
            .push(", expected: ").print(in_size);
        exit(1);
    }
    unsigned char* in = new unsigned char[in_size];
    if (in == NULL) {
        log.push("error: unable to allocate array in with size: ").print(in_size);
        exit(1);
    }
    std::fill(in, in + in_size, 0);
    convert_input(in_vec, in, height, width, channels);

    unsigned char* out = new unsigned char[out_size];
    if (out == NULL) {
        log.push("error: unable to allocate array out with size: ").print(out_size);
        exit(1);
    }
    std::fill(out, out + out_size, 0);
    
    //only time the treatment
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    treat(in, out, height, width, channels, log);
    end = std::chrono::system_clock::now();
    delete[] in;
    delete[] out;
    
    std::chrono::duration<double> elapsed_seconds = end - start;

    return elapsed_seconds.count() * 1000;
}