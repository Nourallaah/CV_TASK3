#ifndef BACKEND_H
#define BACKEND_H

#include <QImage>
#include <QString>
#include <vector>
#include <opencv2/opencv.hpp>

struct HarrisResult {
    QImage outputImage;   // original image with corners drawn
    int    cornerCount;   // number of corners detected
    double timeMs;        // computation time in milliseconds
};

struct SiftResult {
    QImage outputImage;                  // Image with SIFT keypoints drawn
    int    keypointCount;                // Number of features detected
    double timeMs;                       // Computation time in ms
    cv::Mat descriptors;                 // We will need this later for Point 3 (Matching)
    std::vector<cv::KeyPoint> keypoints; // Stored keypoints
};

class Backend
{
public:
    Backend();

    // Run Harris corner detection on the given image.
    HarrisResult runHarris(const QImage& input,
                           double k         = 0.04,
                           int    blockSize = 3,
                           double threshold = 0.01);

    // Run SIFT feature detection and descriptor generation
    SiftResult runSift(const QImage& input);

private:
    // Convert any QImage format to a 2-D grayscale float grid [0,255]
    std::vector<std::vector<double>> toGrayGrid(const QImage& img);

    // Box blur on a 2-D grid; radius = blockSize/2
    std::vector<std::vector<double>> boxBlur(
        const std::vector<std::vector<double>>& src, int radius);
};

#endif // BACKEND_H