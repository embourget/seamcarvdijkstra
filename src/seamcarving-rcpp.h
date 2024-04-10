#include "Rcpp.h"
using namespace Rcpp;

#ifndef SEAM_CARVING
#define SEAM_CARVING
namespace seamcarving {
    NumericVector treat_img(NumericVector in_vec, int height, int width, int channels, bool verbose);
    double time_treat(NumericVector in_vec, int height, int width, int channels, bool verbose);
}

#endif //SEAM_CARVING