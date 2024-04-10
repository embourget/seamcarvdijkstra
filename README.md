# seamcarvdijkstra

## Description

R package to apply Seam-Carving with Dijkstra path-finding to an image.<br/>
Implementation is in C++.

## Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [Credits](#credits)
- [License](#license)

## Installation

Package not (yet) on CRAN repositories, only installation from GitHub available:
 - `devtools::install_github("embourget/seamcarvdijkstra")`

 Alternatively, using `githubinstall`
  - (if `githubinstall` is not installed) `install.packages("githubinstall")`
  - `library(githubinstall)`
  - `gh_install_packages("seamcarvdijkstra")` or `githubinstall("seamcarvdijkstra")`
  - press "y"

## Usage

### R functions

#### Rcpp interface

The package exports two functions to use C++ functions directly thanks to Rcpp.

 - `treat_img ( in_vec, height, width, channels, verbose )`<br/> function to apply Seam-Carving to an image, returns a Rcpp::NumericVector> containing the reduced image. Parameters: 
    - `Rcpp::NumericVector in_vec` containing the image
    - `int height` number of lines
    - `int width` number of columns
    - `int channels` number of bytes per pixels
    - `bool verbose` set to `true` to print progress to console
 - `time_treat( in_vec, height, width, channels, verbose )` <br/> function to time the application of Seam-Carving to an image, returns the computing time in milliseconds. Parameters: 
    - `Rcpp::NumericVector in_vec` containing the image
    - `int height` number of lines
    - `int width` number of columns
    - `int channels` number of bytes per pixels
    - `bool verbose` set to `true` to print progress to console

We will call img the result of `recolorize.readImage`<br/>
In both cases:
 - `c(height, width, channels)` are the result of `dim(img)`
 - `in_vec` is expected to be formatted the result of : `dim(img) = c(height * width * channels)`

For use in C++, these to functions are in `namespace seamcarving` provided by `seamcarving-rcpp.h`.

#### Convenience functions

The package also exports two convenience functions for easier use.

 - `treat ( img, verbose )` <br/> Convenience function for treat_img. Parameters: 
    - `img` array containg the image
    - `verbose` set to `TRUE` to print progress to console
 - `timedrun( img, verbose )` <br/> Convenience function for time_treat. Parameters: 
    - `img` array containg the image
    - `verbose` set to `TRUE` to print progress to console

In both cases, the parameter `img` is expected to be formatted as returned by `recolorize.readImage`.
### C++ functions and classes

If you wish to include the C++ module in your module, include `seamcarving-core.h`. The package contains

- `seamcarvingcore::treat(unsigned char* in, unsigned char* out, int height, int width, int channels, seamcarvingcore::Logger log)` core logic function. This will convert the image to grey-scale, apply a Sobel filter, then path-find using Dijkstra for the lowest constrast path, then recut the image. Parameters: 
    - `unsigned char* in` input pointer
    - `unsigned char* out` output pointer
    - `int height` number of lines
    - `int width` number of columns
    - `int channels` number of bytes per pixels
    - `seamcarvingcore::Logger log` logger for outputting progress
 - `class seamcarvingcore::Logger` convenience class to print: only print to `std::cout` if `Logger.verbose` is set to `true`. Methods:
    - `push(std::string str)` and `push(int i)` to add element to buffer
    - `print(std::string str)` and `print(int i)` to add element to buffer and print buffer
    - `endl()` to print buffer

## Credits

 - Emmeline BOURGET (emmeline.bourget@gmail.com)
 - Mikael KETEMA (mikael.ketema26@gmail.com)
 - Birahim TEWE (Birahimtewe01@gmail.com)

## License

GNU GENERAL PUBLIC LICENSE (GPL-3)
