## usethis namespace: start
#' @useDynLib seamcarvdijkstra, .registration = TRUE
## usethis namespace: end
NULL
## usethis namespace: start
#' @importFrom Rcpp sourceCpp
## usethis name
NULL

#' Apply Seam-Carving image reduction to an image
#'
#' @description Seam-Carving with Dijkstra path-finding
#' @param img an array, with dimension c(height, width, channels), as returned by recolorize.readImage
#' @param verbose a boolean, set to TRUE to print progress to console
#' @return an array, with dimension c(height, width - 1, channels), containing the reduced image
#' @export
treat <- function( img, verbose ) {
  img_copy = img;

  dimensions = dim(img);
  height = dimensions[1];
  width = dimensions[2];
  channels = dimensions[3];

  dim(img_copy) = c(height * width * channels);
  cut_img = treat_img(img_copy, height, width, channels, verbose );

  width = width - 1;
  dim(cut_img) = c(height,width,channels);
  return (cut_img);
}

#' Time the Seam-Carving image reduction of an image
#'
#' @description time Seam-Carving with Dijkstra path-finding
#' @param img an array, with dimension c(height, width, channels), as returned by recolorize.readImage
#' @param verbose a boolean, set to TRUE to print progress to console
#' @return the computing time, in milliseconds
#' @export
timedrun <- function( img, verbose ) {
  img_copy = img;

  dimensions = dim(img);
  height = dimensions[1];
  width = dimensions[2];
  channels = dimensions[3];

  dim(img_copy) = c(height * width * channels);
  time = time_treat(img_copy, height, width, channels, verbose );
  return (time);
}
