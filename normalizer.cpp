#include <iostream>
#include <fstream>
#include <omp.h>
#include <vector>
#include <chrono>

typedef std::vector<uint8_t> pixel;
using namespace std;

pixel rgb_to_ycbcr(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t y = 0.299 * r + 0.587 * g + 0.114 * b;
    uint8_t cb = 128 - 0.168736 * r - 0.331264 * g + 0.5 * b;
    uint8_t cr = 128 + 0.5 * r - 0.418688 * g - 0.081312 * b;

    return {y, cb, cr};
}

pixel ycbcr_to_rgb(uint8_t y, uint8_t cb, uint8_t cr) {
    uint8_t r = y + 1.402 * (cr - 128);
    uint8_t g = y - 0.34414 * (cb - 128) - 0.71414 * (cr - 128);
    uint8_t b = y + 1.772 * (cb - 128);

    return {r, g, b};
}

int scale(int x, int lower, int upper) {
    if (upper == lower)
        return upper;
    return (x - lower) * 255 / (upper - lower);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Wrong number of args\nUsage: <num_threads> <input_file> <output_file>\n";
        return -1;
    }

    int threads = stoi(argv[1]);
    threads = (threads == 0) ? omp_get_max_threads() : threads;

    std::ifstream in_file(argv[2], std::ios::binary);
    if (!in_file.is_open()) {
      std::cerr << "Can't open input file\n";
      return -1;
    }

    if (in_file.get() != 'P' || in_file.get() != '6') {
        std::cerr << "Not ppm P6\n";
        return -1;
    }

    int w, h, max_color;
    in_file >> w >> h >> max_color; // read max_color to skip it

    vector<pixel> img(h * w);
    for (int i = 0; i < h * w; i++) {
        uint8_t r, g, b;
        in_file >> r >> g >> b;
        img[i] = {r, g, b};
    }

    uint8_t mx = 0;
    uint8_t mn = 255;
    auto start = chrono::steady_clock::now();

#pragma omp parallel for \
    schedule(static) reduction(max : mx) reduction(min : mn) \
    default(shared) num_threads(threads)
    for (int i = 0; i < w * h; ++i) {
        img[i] = rgb_to_ycbcr(img[i][0], img[i][1], img[i][2]);
        mx = max(mx, img[i][0]);
        mn = min(mn, img[i][0]);
    }

#pragma omp parallel for \
    schedule(static) default(shared) num_threads(threads)
    for (int i = 0; i < h * w; i++) {
        img[i] = ycbcr_to_rgb(scale(img[i][0], mn, mx), img[i][1], img[i][2]);
    }

    auto end = chrono::steady_clock::now();
    printf("\nTime (%i thread(s)): %.3f ms\n", threads, chrono::duration<float>(end - start).count() * 1000);


    std::ofstream out_file(argv[2], std::ios::binary);
    if (!out_file.is_open()) {
      std::cerr << "Can't open output file\n";
      return -1;
    }

    out_file << "P6\n" << w << " " << h << "\n" << 255 << "\n";
    for (int i = 0; i < h * w; ++i) {
        for (int k = 0; k < 3; ++k) {
            out_file << img[i][k];
        }
    }

    return 0;
}
