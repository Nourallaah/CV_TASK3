#ifndef BACKEND_H
#define BACKEND_H

#include <QImage>
#include <QString>
#include <vector>

struct HarrisResult {
    QImage outputImage;   // original image with corners drawn
    int    cornerCount;   // number of corners detected
    double timeMs;        // computation time in milliseconds
};

class Backend
{
public:
    Backend();

    // Run Harris corner detection on the given image.
    // k         - Harris sensitivity constant (typically 0.04–0.06)
    // blockSize - size of the Gaussian smoothing window (odd, 3–9)
    // threshold - fraction of max response to use as cut-off (0.0–1.0)
    HarrisResult runHarris(const QImage& input,
                           double k         = 0.04,
                           int    blockSize = 3,
                           double threshold = 0.01);

private:
    // Convert any QImage format to a 2-D grayscale float grid [0,255]
    std::vector<std::vector<double>> toGrayGrid(const QImage& img);

    // Box blur on a 2-D grid; radius = blockSize/2
    std::vector<std::vector<double>> boxBlur(
        const std::vector<std::vector<double>>& src, int radius);
};

#endif // BACKEND_H