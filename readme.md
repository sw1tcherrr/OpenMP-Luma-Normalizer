# OpenMP-Luma-Normalizer
##### Automatic luma correction in `YCbCr.601` color space
Input file – an image in `PPM (P6)` format, `RGB` color space, pixels’ values are in `[0; 255]`.

The image may have poor contrast: not the entire range of values is used, but only part of it. For example, if the darkest parts of the image have the contrast value of 20 instead of 0.

The program changes the pixel values in such a way as to obtain the maximum contrast (value range `[0; 255]`) without changing the chromaticity. This is achieved by adjusting the contrast in the Y channel in the `YCbCr.601` color space. 

Output file – new image in `PPM (P6)`.

Algorithm is paralleled with OpenMP, number of threads is passed as argument.

#### Compilation

Tested with `g++` with `-fopenmp` flag

#### Usage

```
normalizer <num_threads> <input_file> <output_file>
```

`num_threads = 0` means maximum possible number

#### Report

There is a report (in Russian) with 

- OpenMP description
- algorithm description
- comparison between paralleled and unparalleled versions
- comparison between different OpenMP modes
- example pictures
