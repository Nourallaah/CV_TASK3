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
    cv::Mat descriptors;                 // Packaged for Point 3 Matching
    std::vector<cv::KeyPoint> keypoints; // Packaged for Point 3 Matching
};

enum class MatchMethod {
    SSD,
    NCC
};

struct MatchResult {
    QImage matchImage;
    int matchCount;
    int totalSourceFeatures;
    double timeMs;
};

class Backend
{
public:
    Backend();

    // Run Harris corner detection on the given image.
    // I changed 'useShiTomasi' to 'useLambdaMinus' right here:
    HarrisResult runHarris(const QImage& input,
                           double k         = 0.04,
                           int    blockSize = 3,
                           double threshold = 0.01,
                           bool   useLambdaMinus = false);

    // Run SIFT feature detection and descriptor generation (PURE C++)
    SiftResult runSift(const QImage& input);

    // Match features between two images
    MatchResult matchFeatures(const QImage& img1, const QImage& img2, MatchMethod method, double ratioThreshold = 0.8);

private:
    // Convert any QImage format to a 2-D grayscale float grid [0,255]
    std::vector<std::vector<double>> toGrayGrid(const QImage& img);

    // Box blur on a 2-D grid
    std::vector<std::vector<double>> boxBlur(
        const std::vector<std::vector<double>>& src, int radius);

    // Pure C++ Gaussian Blur for SIFT scale-space
    std::vector<std::vector<double>> gaussianBlur(
        const std::vector<std::vector<double>>& src, double sigma);

    // Grid subtraction for Difference of Gaussians (DoG)
    std::vector<std::vector<double>> subtractGrids(
        const std::vector<std::vector<double>>& a, const std::vector<std::vector<double>>& b);
};

#endif // BACKEND_H