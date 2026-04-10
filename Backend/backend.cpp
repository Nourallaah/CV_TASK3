#include "backend.h"

#include <QPainter>
#include <QColor>
#include <chrono>
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ---------------------------------------------------------------------------
// Helpers: QImage <-> cv::Mat conversions (Used by Point 3)
// ---------------------------------------------------------------------------
static cv::Mat qimageToMat(const QImage& source) {
    QImage converted = source.convertToFormat(QImage::Format_RGB888);
    cv::Mat mat(converted.height(), converted.width(), CV_8UC3, 
                (void*)converted.bits(), converted.bytesPerLine());
    cv::Mat matCopy;
    cv::cvtColor(mat, matCopy, cv::COLOR_RGB2BGR);
    return matCopy;
}

static QImage matToQimage(const cv::Mat& source) {
    cv::Mat rgb;
    cv::cvtColor(source, rgb, cv::COLOR_BGR2RGB);
    QImage img((const uchar*)rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
    return img.copy();
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
Backend::Backend() {}

// ---------------------------------------------------------------------------
// Helper: convert QImage → 2-D double grid (grayscale, values 0–255)
// ---------------------------------------------------------------------------
std::vector<std::vector<double>>
Backend::toGrayGrid(const QImage& img)
{
    int W = img.width();
    int H = img.height();
    std::vector<std::vector<double>> g(H, std::vector<double>(W, 0.0));

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            QColor c(img.pixel(x, y));
            g[y][x] = 0.299 * c.red() + 0.587 * c.green() + 0.114 * c.blue();
        }
    }
    return g;
}

// ---------------------------------------------------------------------------
// Helper: box blur
// ---------------------------------------------------------------------------
std::vector<std::vector<double>>
Backend::boxBlur(const std::vector<std::vector<double>>& src, int radius)
{
    if (radius <= 0) return src;
    int H = (int)src.size();
    int W = (int)src[0].size();

    std::vector<std::vector<double>> tmp(H, std::vector<double>(W, 0.0));
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            double sum = 0.0;
            int cnt = 0;
            for (int dx = -radius; dx <= radius; ++dx) {
                int nx = std::clamp(x + dx, 0, W - 1);
                sum += src[y][nx];
                ++cnt;
            }
            tmp[y][x] = sum / cnt;
        }
    }

    std::vector<std::vector<double>> out(H, std::vector<double>(W, 0.0));
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            double sum = 0.0;
            int cnt = 0;
            for (int dy = -radius; dy <= radius; ++dy) {
                int ny = std::clamp(y + dy, 0, H - 1);
                sum += tmp[ny][x];
                ++cnt;
            }
            out[y][x] = sum / cnt;
        }
    }
    return out;
}

// ---------------------------------------------------------------------------
// Helper: 1D Separable Gaussian Blur (Required for SIFT)
// ---------------------------------------------------------------------------
std::vector<std::vector<double>> 
Backend::gaussianBlur(const std::vector<std::vector<double>>& src, double sigma) 
{
    int H = src.size();
    int W = src[0].size();
    int radius = std::max(1, (int)std::ceil(3.0 * sigma));
    std::vector<double> kernel(2 * radius + 1);
    double sum = 0.0;
    
    for (int i = -radius; i <= radius; ++i) {
        kernel[i + radius] = std::exp(-(i * i) / (2.0 * sigma * sigma));
        sum += kernel[i + radius];
    }
    for (double& k : kernel) k /= sum;

    std::vector<std::vector<double>> tmp(H, std::vector<double>(W, 0.0));
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            double val = 0.0;
            for (int i = -radius; i <= radius; ++i) {
                int nx = std::clamp(x + i, 0, W - 1);
                val += src[y][nx] * kernel[i + radius];
            }
            tmp[y][x] = val;
        }
    }

    std::vector<std::vector<double>> out(H, std::vector<double>(W, 0.0));
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            double val = 0.0;
            for (int i = -radius; i <= radius; ++i) {
                int ny = std::clamp(y + i, 0, H - 1);
                val += tmp[ny][x] * kernel[i + radius];
            }
            out[y][x] = val;
        }
    }
    return out;
}

// ---------------------------------------------------------------------------
// Helper: Grid Subtraction (Required for SIFT DoG)
// ---------------------------------------------------------------------------
std::vector<std::vector<double>> 
Backend::subtractGrids(const std::vector<std::vector<double>>& a, const std::vector<std::vector<double>>& b) 
{
    int H = a.size();
    int W = a[0].size();
    std::vector<std::vector<double>> out(H, std::vector<double>(W, 0.0));
    for(int y = 0; y < H; ++y) {
        for(int x = 0; x < W; ++x) {
            out[y][x] = a[y][x] - b[y][x];
        }
    }
    return out;
}

// ---------------------------------------------------------------------------
// Main: Harris corner detector
// ---------------------------------------------------------------------------
HarrisResult Backend::runHarris(const QImage& input, double k, int blockSize, double threshold)
{
    auto t0 = std::chrono::high_resolution_clock::now();
    auto gray = toGrayGrid(input.convertToFormat(QImage::Format_RGB32));
    int H = (int)gray.size();
    int W = (int)gray[0].size();

    std::vector<std::vector<double>> Ix(H, std::vector<double>(W, 0.0));
    std::vector<std::vector<double>> Iy(H, std::vector<double>(W, 0.0));

    for (int y = 1; y < H - 1; ++y) {
        for (int x = 1; x < W - 1; ++x) {
            Ix[y][x] = (-gray[y-1][x-1] + gray[y-1][x+1]
                        - 2*gray[y][x-1]   + 2*gray[y][x+1]
                        - gray[y+1][x-1] + gray[y+1][x+1]) / 8.0;

            Iy[y][x] = (-gray[y-1][x-1] - 2*gray[y-1][x] - gray[y-1][x+1]
                        + gray[y+1][x-1] + 2*gray[y+1][x]   + gray[y+1][x+1]) / 8.0;
        }
    }

    std::vector<std::vector<double>> Ixx(H, std::vector<double>(W, 0.0));
    std::vector<std::vector<double>> Iyy(H, std::vector<double>(W, 0.0));
    std::vector<std::vector<double>> Ixy(H, std::vector<double>(W, 0.0));

    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            Ixx[y][x] = Ix[y][x] * Ix[y][x];
            Iyy[y][x] = Iy[y][x] * Iy[y][x];
            Ixy[y][x] = Ix[y][x] * Iy[y][x];
        }

    int radius = std::max(1, blockSize / 2);
    Ixx = boxBlur(Ixx, radius);
    Iyy = boxBlur(Iyy, radius);
    Ixy = boxBlur(Ixy, radius);

    std::vector<std::vector<double>> R(H, std::vector<double>(W, 0.0));
    double maxR = 0.0;

    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x) {
            double det   = Ixx[y][x] * Iyy[y][x] - Ixy[y][x] * Ixy[y][x];
            double trace = Ixx[y][x] + Iyy[y][x];
            R[y][x] = det - k * trace * trace;
            if (R[y][x] > maxR) maxR = R[y][x];
        }

    double cutoff = threshold * maxR;
    std::vector<std::pair<int,int>> corners;

    for (int y = 1; y < H - 1; ++y) {
        for (int x = 1; x < W - 1; ++x) {
            if (R[y][x] < cutoff) continue;
            bool isMax = true;
            for (int dy = -1; dy <= 1 && isMax; ++dy)
                for (int dx = -1; dx <= 1 && isMax; ++dx)
                    if (!(dy == 0 && dx == 0) && R[y+dy][x+dx] >= R[y][x])
                        isMax = false;
            if (isMax) corners.emplace_back(x, y);
        }
    }

    QImage output = input.convertToFormat(QImage::Format_RGB32);
    QPainter painter(&output);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::red, 3));
    int markerRadius = std::max(4, std::min(W, H) / 100);
    for (auto& [cx, cy] : corners) {
        painter.drawEllipse(QPoint(cx, cy), markerRadius, markerRadius);
    }
    painter.end();

    auto t1 = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    HarrisResult result;
    result.outputImage  = output;
    result.cornerCount  = (int)corners.size();
    result.timeMs       = ms;
    return result;
}









// ---------------------------------------------------------------------------
// Main: SIFT Feature Descriptor Generation (From Scratch)
// ---------------------------------------------------------------------------
SiftResult Backend::runSift(const QImage& input)
{
    auto t0 = std::chrono::high_resolution_clock::now();

    // =======================================================================
    // STEP 1: Image Preparation
    // Convert the input image to a flat 2D grayscale grid (values 0-255).
    // SIFT operates purely on intensity (luminance) to find structural features.
    // =======================================================================
    auto gray = toGrayGrid(input.convertToFormat(QImage::Format_RGB32));
    int H = gray.size();
    int W = gray[0].size();

    // =======================================================================
    // STEP 2: Build the Gaussian Pyramid (Scale-Space)
    // We blur the image at progressively higher 'sigma' values. 
    // This simulates viewing the image from different distances, ensuring 
    // our features will be "Scale Invariant" (the 'SI' in SIFT).
    // =======================================================================
    double sigmas[5] = {1.0, 1.414, 2.0, 2.828, 4.0};
    std::vector<std::vector<std::vector<double>>> gaussians;
    for(int i = 0; i < 5; ++i) {
        gaussians.push_back(gaussianBlur(gray, sigmas[i]));
    }

    // =======================================================================
    // STEP 3: Difference of Gaussians (DoG)
    // By subtracting adjacent blurred images in the pyramid, we isolate the 
    // high-frequency details (edges and corners) that exist at that specific scale.
    // This is an efficient approximation of the Laplacian of Gaussian (LoG).
    // =======================================================================
    std::vector<std::vector<std::vector<double>>> dogs;
    for(int i = 0; i < 4; ++i) {
        dogs.push_back(subtractGrids(gaussians[i+1], gaussians[i]));
    }

    // =======================================================================
    // STEP 4: Extrema Detection & Edge Elimination
    // We search the DoG images for pixels that are the absolute local maximum 
    // or minimum compared to their 26 neighbors (3D scale-space).
    // =======================================================================
    std::vector<cv::KeyPoint> keypoints;
    for(int s = 1; s < 3; ++s) {           // Iterate through middle scales
        for(int y = 16; y < H - 16; ++y) { // 16px border to leave room for descriptors later
            for(int x = 16; x < W - 16; ++x) {
                
                double val = dogs[s][y][x];
                
                // 4A: Contrast Threshold - ignore weak responses (noise)
                if (std::abs(val) < 3.0) continue; 

                // 4B: 3D Scale-Space Extrema Check
                // Compare current pixel against 8 neighbors in current scale, 
                // 9 in the scale above, and 9 in the scale below (26 total).
                bool isMax = true, isMin = true;
                for(int ds = -1; ds <= 1; ++ds) {
                    for(int dy = -1; dy <= 1; ++dy) {
                        for(int dx = -1; dx <= 1; ++dx) {
                            if(ds == 0 && dy == 0 && dx == 0) continue; // Skip self
                            double n_val = dogs[s+ds][y+dy][x+dx];
                            if(val <= n_val) isMax = false;
                            if(val >= n_val) isMin = false;
                        }
                    }
                }

                // 4C: Hessian Edge Elimination (Lowe's Approach)
                // If the point is an extremum, we must ensure it's a corner, not a flat edge.
                // We use the Hessian matrix (trace and determinant) to check principal curvatures.
                if(isMax || isMin) {
                    // Compute 2nd order derivatives using finite differences
                    double dxx = dogs[s][y][x+1] + dogs[s][y][x-1] - 2*val;
                    double dyy = dogs[s][y+1][x] + dogs[s][y-1][x] - 2*val;
                    double dxy = (dogs[s][y+1][x+1] - dogs[s][y+1][x-1] - dogs[s][y-1][x+1] + dogs[s][y-1][x-1]) / 4.0;
                    
                    double tr = dxx + dyy;                // Trace
                    double det = dxx*dyy - dxy*dxy;       // Determinant
                    
                    if (det <= 0) continue; // Negative determinant means saddle point (invalid)
                    
                    // If the ratio of principal curvatures is > 10, it's an edge, so we reject it.
                    double r = 10.0;
                    if ((tr*tr)/det > ((r+1)*(r+1))/r) continue;

                    // Passed all tests! It is a valid scale-invariant keypoint.
                    cv::KeyPoint kp;
                    kp.pt.x = x;
                    kp.pt.y = y;
                    kp.size = sigmas[s] * 3.0; // Map scale to a visual radius for drawing
                    keypoints.push_back(kp);
                }
            }
        }
    }

    // =======================================================================
    // STEP 5: Build 128-D Descriptors
    // For every valid keypoint, we examine a 16x16 pixel window around it.
    // We calculate the gradient magnitude and direction for each pixel to build
    // a unique "fingerprint" (descriptor) for matching in Point 3.
    // =======================================================================
    cv::Mat descriptors(keypoints.size(), 128, CV_32F);
    int kpIdx = 0;

    for(auto& kp : keypoints) {
        int px = kp.pt.x;
        int py = kp.pt.y;
        std::vector<float> desc(128, 0.0f);
        
        // Loop over the 16x16 window surrounding the keypoint
        for(int dy = -8; dy < 8; ++dy) {
            for(int dx = -8; dx < 8; ++dx) {
                int nx = px + dx;
                int ny = py + dy;
                
                // Approximate the gradients (Gx and Gy) using adjacent pixels
                double gx = gaussians[1][ny][nx+1] - gaussians[1][ny][nx-1];
                double gy = gaussians[1][ny+1][nx] - gaussians[1][ny-1][nx];
                
                // Calculate magnitude and angle (0 to 360 degrees)
                double mag = std::sqrt(gx*gx + gy*gy);
                double theta = std::atan2(gy, gx) * 180.0 / M_PI;
                if(theta < 0) theta += 360.0;

                // Apply a Gaussian weighting function to the window.
                // This gives pixels closer to the center keypoint more importance.
                double gw = std::exp(-(dx*dx + dy*dy) / (2 * 4.0 * 4.0));
                mag *= gw;

                // Divide the 16x16 window into 16 smaller 4x4 sub-grids.
                int r = (dy + 8) / 4; // Sub-grid row (0 to 3)
                int c = (dx + 8) / 4; // Sub-grid col (0 to 3)
                
                // Quantize the angle into one of 8 directional bins (0°, 45°, 90°, etc.)
                int bin = (int)(theta / 45.0) % 8; 
                
                // Add the weighted magnitude to the correct bin in the 128-D array
                // (16 sub-grids * 8 bins = 128 dimensions)
                desc[(r*4 + c)*8 + bin] += mag;
            }
        }

        // ===================================================================
        // STEP 5B: Illumination Normalization
        // Normalize the vector to unit length to make the descriptor robust 
        // to global contrast changes (like a photo taken in different lighting).
        // ===================================================================
        float sum_sq = 0;
        for(float v : desc) sum_sq += v*v;
        sum_sq = std::sqrt(sum_sq);
        
        for(float& v : desc) {
            v /= (sum_sq + 1e-7);
            // Cap values at 0.2 to handle non-linear lighting (like camera saturation)
            if(v > 0.2f) v = 0.2f; 
        }
        
        // Re-normalize after capping
        sum_sq = 0;
        for(float v : desc) sum_sq += v*v;
        sum_sq = std::sqrt(sum_sq);
        
        // Load the pure C++ descriptor array into the OpenCV cv::Mat container.
        // We do this purely so the Point 3 matching code can process it smoothly.
        for(int i = 0; i < 128; ++i) {
            descriptors.at<float>(kpIdx, i) = desc[i] / (sum_sq + 1e-7);
        }
        kpIdx++;
    }

    // =======================================================================
    // STEP 6: Visualization and Return
    // Draw the keypoints directly onto the image using Qt.
    // =======================================================================
    QImage output = input.convertToFormat(QImage::Format_RGB32);
    QPainter painter(&output);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::green, 1)); // 1px solid green outline
    
    for(auto& kp : keypoints) {
        // Draw a circle scaled to the keypoint's detected pyramid scale
        painter.drawEllipse(QPoint(kp.pt.x, kp.pt.y), (int)kp.size, (int)kp.size);
    }
    painter.end();

    // Stop the timer
    auto t1 = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    // Package results
    SiftResult result;
    result.outputImage   = output;
    result.keypointCount = keypoints.size();
    result.timeMs        = ms;
    result.descriptors   = descriptors;
    result.keypoints     = keypoints;

    return result;
}











// ---------------------------------------------------------------------------
// Main: Feature Matching using SSD / NCC (Unchanged)
// ---------------------------------------------------------------------------
MatchResult Backend::matchFeatures(const QImage& img1, const QImage& img2, MatchMethod method, double ratioThreshold)
{
    auto t0 = std::chrono::high_resolution_clock::now();

    // ---- 1. Compute SIFT for both images ----------------------------------
    SiftResult s1 = runSift(img1);
    SiftResult s2 = runSift(img2);

    int rows1 = s1.descriptors.rows;
    int rows2 = s2.descriptors.rows;
    int cols  = s1.descriptors.cols;

    std::vector<cv::DMatch> goodMatches;

    // Optional: for SSD, standard Lowe's ratio test works on L2 distance, so square the threshold
    double ssdRatioThresh = ratioThreshold * ratioThreshold;

    // ---- 2. Iterate each descriptor in Image 1 ----------------------------
    for (int i = 0; i < rows1; ++i) {
        float* d1 = s1.descriptors.ptr<float>(i);

        double bestScore = (method == MatchMethod::SSD) ? std::numeric_limits<double>::max() : -std::numeric_limits<double>::max();
        double secondBestScore = (method == MatchMethod::SSD) ? std::numeric_limits<double>::max() : -std::numeric_limits<double>::max();
        int bestIdx = -1;

        // Compare against each descriptor in Image 2
        for (int j = 0; j < rows2; ++j) {
            float* d2 = s2.descriptors.ptr<float>(j);
            double score = 0.0;

            if (method == MatchMethod::SSD) {
                // Sum of Squared Differences
                for (int k = 0; k < cols; ++k) {
                    double diff = d1[k] - d2[k];
                    score += diff * diff;
                }
            } else {
                // Normalized Cross Correlation
                double mean1 = 0, mean2 = 0;
                for (int k = 0; k < cols; ++k) {
                    mean1 += d1[k];
                    mean2 += d2[k];
                }
                mean1 /= cols;
                mean2 /= cols;

                double num = 0, den1 = 0, den2 = 0;
                for (int k = 0; k < cols; ++k) {
                    double v1 = d1[k] - mean1;
                    double v2 = d2[k] - mean2;
                    num += v1 * v2;
                    den1 += v1 * v1;
                    den2 += v2 * v2;
                }
                if (den1 == 0 || den2 == 0) score = 0;
                else score = num / sqrt(den1 * den2);
            }

            // Keep track of best and second best match
            if (method == MatchMethod::SSD) {
                if (score < bestScore) {
                    secondBestScore = bestScore;
                    bestScore = score;
                    bestIdx = j;
                } else if (score < secondBestScore) {
                    secondBestScore = score;
                }
            } else { // NCC
                if (score > bestScore) {
                    secondBestScore = bestScore;
                    bestScore = score;
                    bestIdx = j;
                } else if (score > secondBestScore) {
                    secondBestScore = score;
                }
            }
        }

        // ---- 3. Filter matches --------------------------------------------
        bool isGood = false;
        if (bestIdx != -1) {
            if (method == MatchMethod::SSD) {
                // Ratio test for SSD
                if (bestScore < ssdRatioThresh * secondBestScore) {
                    isGood = true;
                }
            } else { // NCC
                // For NCC, we simply threshold on a high correlation value
                if (bestScore > 0.8) {
                    isGood = true;
                }
            }
        }

        if (isGood) {
            goodMatches.push_back(cv::DMatch(i, bestIdx, (float)bestScore));
        }
    }

    // ---- 4. Draw matches --------------------------------------------------
    cv::Mat img1Mat = qimageToMat(img1);
    cv::Mat img2Mat = qimageToMat(img2);
    cv::Mat matchMat;

    // Use solid green for match lines, and solid red for the keypoints
    cv::drawMatches(img1Mat, s1.keypoints, img2Mat, s2.keypoints, goodMatches, matchMat,
                    cv::Scalar(0, 255, 0), cv::Scalar(0, 0, 255), std::vector<char>(),
                    cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    // ---- 5. Measure time --------------------------------------------------
    auto t1 = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    MatchResult result;
    result.matchImage = matToQimage(matchMat);
    result.matchCount = (int)goodMatches.size();
    result.totalSourceFeatures = rows1;
    result.timeMs     = ms;

    return result;
}










































// #include "backend.h"

// #include <QPainter>
// #include <QColor>
// #include <chrono>
// #include <cmath>
// #include <algorithm>

// // ---------------------------------------------------------------------------
// // Helpers: QImage <-> cv::Mat conversions
// // ---------------------------------------------------------------------------
// static cv::Mat qimageToMat(const QImage& source) {
//     QImage converted = source.convertToFormat(QImage::Format_RGB888);
//     cv::Mat mat(converted.height(), converted.width(), CV_8UC3, 
//                 (void*)converted.bits(), converted.bytesPerLine());
//     cv::Mat matCopy;
//     cv::cvtColor(mat, matCopy, cv::COLOR_RGB2BGR);
//     return matCopy;
// }

// static QImage matToQimage(const cv::Mat& source) {
//     cv::Mat rgb;
//     cv::cvtColor(source, rgb, cv::COLOR_BGR2RGB);
//     QImage img((const uchar*)rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
//     return img.copy(); // Copy required to detach from cv::Mat memory
// }

// // ---------------------------------------------------------------------------
// // Constructor
// // ---------------------------------------------------------------------------
// Backend::Backend() {}

// // ---------------------------------------------------------------------------
// // Helper: convert QImage → 2-D double grid (grayscale, values 0–255)
// // ---------------------------------------------------------------------------
// std::vector<std::vector<double>>
// Backend::toGrayGrid(const QImage& img)
// {
//     int W = img.width();
//     int H = img.height();
//     std::vector<std::vector<double>> g(H, std::vector<double>(W, 0.0));

//     for (int y = 0; y < H; ++y) {
//         for (int x = 0; x < W; ++x) {
//             QColor c(img.pixel(x, y));
//             // standard luminance formula
//             g[y][x] = 0.299 * c.red() + 0.587 * c.green() + 0.114 * c.blue();
//         }
//     }
//     return g;
// }

// // ---------------------------------------------------------------------------
// // Helper: box blur (uniform average) with given radius
// // ---------------------------------------------------------------------------
// std::vector<std::vector<double>>
// Backend::boxBlur(const std::vector<std::vector<double>>& src, int radius)
// {
//     if (radius <= 0) return src;
//     int H = (int)src.size();
//     int W = (int)src[0].size();

//     // Horizontal pass
//     std::vector<std::vector<double>> tmp(H, std::vector<double>(W, 0.0));
//     for (int y = 0; y < H; ++y) {
//         for (int x = 0; x < W; ++x) {
//             double sum = 0.0;
//             int cnt = 0;
//             for (int dx = -radius; dx <= radius; ++dx) {
//                 int nx = std::clamp(x + dx, 0, W - 1);
//                 sum += src[y][nx];
//                 ++cnt;
//             }
//             tmp[y][x] = sum / cnt;
//         }
//     }

//     // Vertical pass
//     std::vector<std::vector<double>> out(H, std::vector<double>(W, 0.0));
//     for (int y = 0; y < H; ++y) {
//         for (int x = 0; x < W; ++x) {
//             double sum = 0.0;
//             int cnt = 0;
//             for (int dy = -radius; dy <= radius; ++dy) {
//                 int ny = std::clamp(y + dy, 0, H - 1);
//                 sum += tmp[ny][x];
//                 ++cnt;
//             }
//             out[y][x] = sum / cnt;
//         }
//     }
//     return out;
// }

// // ---------------------------------------------------------------------------
// // Main: Harris corner detector
// // ---------------------------------------------------------------------------
// HarrisResult Backend::runHarris(const QImage& input,
//                                 double k,
//                                 int    blockSize,
//                                 double threshold)
// {
//     auto t0 = std::chrono::high_resolution_clock::now();

//     // ---- 1. Grayscale grid ------------------------------------------------
//     auto gray = toGrayGrid(input.convertToFormat(QImage::Format_RGB32));
//     int H = (int)gray.size();
//     int W = (int)gray[0].size();

//     // ---- 2. Sobel gradients Ix, Iy ----------------------------------------
//     std::vector<std::vector<double>> Ix(H, std::vector<double>(W, 0.0));
//     std::vector<std::vector<double>> Iy(H, std::vector<double>(W, 0.0));

//     for (int y = 1; y < H - 1; ++y) {
//         for (int x = 1; x < W - 1; ++x) {
//             Ix[y][x] = (-gray[y-1][x-1] + gray[y-1][x+1]
//                         - 2*gray[y][x-1]   + 2*gray[y][x+1]
//                         - gray[y+1][x-1] + gray[y+1][x+1]) / 8.0;

//             Iy[y][x] = (-gray[y-1][x-1] - 2*gray[y-1][x] - gray[y-1][x+1]
//                         + gray[y+1][x-1] + 2*gray[y+1][x]   + gray[y+1][x+1]) / 8.0;
//         }
//     }

//     // ---- 3. Structure tensor products -------------------------------------
//     std::vector<std::vector<double>> Ixx(H, std::vector<double>(W, 0.0));
//     std::vector<std::vector<double>> Iyy(H, std::vector<double>(W, 0.0));
//     std::vector<std::vector<double>> Ixy(H, std::vector<double>(W, 0.0));

//     for (int y = 0; y < H; ++y)
//         for (int x = 0; x < W; ++x) {
//             Ixx[y][x] = Ix[y][x] * Ix[y][x];
//             Iyy[y][x] = Iy[y][x] * Iy[y][x];
//             Ixy[y][x] = Ix[y][x] * Iy[y][x];
//         }

//     // ---- 4. Gaussian smoothing (box blur approximation) ------------------
//     int radius = std::max(1, blockSize / 2);
//     Ixx = boxBlur(Ixx, radius);
//     Iyy = boxBlur(Iyy, radius);
//     Ixy = boxBlur(Ixy, radius);

//     // ---- 5. Harris response R = det(M) - k * trace(M)^2 -----------------
//     std::vector<std::vector<double>> R(H, std::vector<double>(W, 0.0));
//     double maxR = 0.0;

//     for (int y = 0; y < H; ++y)
//         for (int x = 0; x < W; ++x) {
//             double det   = Ixx[y][x] * Iyy[y][x] - Ixy[y][x] * Ixy[y][x];
//             double trace = Ixx[y][x] + Iyy[y][x];
//             R[y][x] = det - k * trace * trace;
//             if (R[y][x] > maxR) maxR = R[y][x];
//         }

//     // ---- 6. Threshold + non-maximum suppression (3×3 window) -------------
//     double cutoff = threshold * maxR;
//     std::vector<std::pair<int,int>> corners;

//     for (int y = 1; y < H - 1; ++y) {
//         for (int x = 1; x < W - 1; ++x) {
//             if (R[y][x] < cutoff) continue;
//             // local max check
//             bool isMax = true;
//             for (int dy = -1; dy <= 1 && isMax; ++dy)
//                 for (int dx = -1; dx <= 1 && isMax; ++dx)
//                     if (!(dy == 0 && dx == 0) && R[y+dy][x+dx] >= R[y][x])
//                         isMax = false;
//             if (isMax)
//                 corners.emplace_back(x, y);
//         }
//     }

//     // ---- 7. Draw corners on output image ----------------------------------
//     QImage output = input.convertToFormat(QImage::Format_RGB32);
//     QPainter painter(&output);
//     painter.setRenderHint(QPainter::Antialiasing); // Smooth edges
//     painter.setPen(QPen(Qt::red, 3));              // Solid Red 3px
//     int markerRadius = std::max(4, std::min(W, H) / 100);
//     for (auto& [cx, cy] : corners) {
//         painter.drawEllipse(QPoint(cx, cy), markerRadius, markerRadius);
//     }
//     painter.end();

//     // ---- 8. Timing --------------------------------------------------------
//     auto t1 = std::chrono::high_resolution_clock::now();
//     double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

//     HarrisResult result;
//     result.outputImage  = output;
//     result.cornerCount  = (int)corners.size();
//     result.timeMs       = ms;
//     return result;
// }

// // ---------------------------------------------------------------------------
// // Main: SIFT Feature Descriptor Generation
// // ---------------------------------------------------------------------------
// SiftResult Backend::runSift(const QImage& input)
// {
//     auto t0 = std::chrono::high_resolution_clock::now();

//     // ---- 1. Convert QImage to cv::Mat and Grayscale -----------------------
//     cv::Mat cvImage = qimageToMat(input);
//     cv::Mat grayImage;
//     cv::cvtColor(cvImage, grayImage, cv::COLOR_BGR2GRAY);

//     // ---- 2. Initialize SIFT and compute -----------------------------------
//     cv::Ptr<cv::SIFT> sift = cv::SIFT::create();
//     std::vector<cv::KeyPoint> keypoints;
//     cv::Mat descriptors;
    
//     sift->detectAndCompute(grayImage, cv::noArray(), keypoints, descriptors);

//     // ---- 3. Draw rich keypoints (shows scale and orientation) -------------
//     cv::Mat outputMat;
//     // Use bright green instead of random pastel colors to make it clear
//     cv::drawKeypoints(cvImage, keypoints, outputMat, cv::Scalar(0, 255, 0), 
//                       cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

//     // ---- 4. Timing --------------------------------------------------------
//     auto t1 = std::chrono::high_resolution_clock::now();
//     double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

//     SiftResult result;
//     result.outputImage   = matToQimage(outputMat);
//     result.keypointCount = (int)keypoints.size();
//     result.timeMs        = ms;
//     result.descriptors   = descriptors.clone();
//     result.keypoints     = keypoints;

//     return result;
// }

// // ---------------------------------------------------------------------------
// // Main: Feature Matching using SSD / NCC
// // ---------------------------------------------------------------------------
// MatchResult Backend::matchFeatures(const QImage& img1, const QImage& img2, MatchMethod method, double ratioThreshold)
// {
//     auto t0 = std::chrono::high_resolution_clock::now();

//     // ---- 1. Compute SIFT for both images ----------------------------------
//     SiftResult s1 = runSift(img1);
//     SiftResult s2 = runSift(img2);

//     int rows1 = s1.descriptors.rows;
//     int rows2 = s2.descriptors.rows;
//     int cols  = s1.descriptors.cols;

//     std::vector<cv::DMatch> goodMatches;

//     // Optional: for SSD, standard Lowe's ratio test works on L2 distance, so square the threshold
//     double ssdRatioThresh = ratioThreshold * ratioThreshold;

//     // ---- 2. Iterate each descriptor in Image 1 ----------------------------
//     for (int i = 0; i < rows1; ++i) {
//         float* d1 = s1.descriptors.ptr<float>(i);

//         double bestScore = (method == MatchMethod::SSD) ? std::numeric_limits<double>::max() : -std::numeric_limits<double>::max();
//         double secondBestScore = (method == MatchMethod::SSD) ? std::numeric_limits<double>::max() : -std::numeric_limits<double>::max();
//         int bestIdx = -1;

//         // Compare against each descriptor in Image 2
//         for (int j = 0; j < rows2; ++j) {
//             float* d2 = s2.descriptors.ptr<float>(j);
//             double score = 0.0;

//             if (method == MatchMethod::SSD) {
//                 // Sum of Squared Differences
//                 for (int k = 0; k < cols; ++k) {
//                     double diff = d1[k] - d2[k];
//                     score += diff * diff;
//                 }
//             } else {
//                 // Normalized Cross Correlation
//                 double mean1 = 0, mean2 = 0;
//                 for (int k = 0; k < cols; ++k) {
//                     mean1 += d1[k];
//                     mean2 += d2[k];
//                 }
//                 mean1 /= cols;
//                 mean2 /= cols;

//                 double num = 0, den1 = 0, den2 = 0;
//                 for (int k = 0; k < cols; ++k) {
//                     double v1 = d1[k] - mean1;
//                     double v2 = d2[k] - mean2;
//                     num += v1 * v2;
//                     den1 += v1 * v1;
//                     den2 += v2 * v2;
//                 }
//                 if (den1 == 0 || den2 == 0) score = 0;
//                 else score = num / sqrt(den1 * den2);
//             }

//             // Keep track of best and second best match
//             if (method == MatchMethod::SSD) {
//                 if (score < bestScore) {
//                     secondBestScore = bestScore;
//                     bestScore = score;
//                     bestIdx = j;
//                 } else if (score < secondBestScore) {
//                     secondBestScore = score;
//                 }
//             } else { // NCC
//                 if (score > bestScore) {
//                     secondBestScore = bestScore;
//                     bestScore = score;
//                     bestIdx = j;
//                 } else if (score > secondBestScore) {
//                     secondBestScore = score;
//                 }
//             }
//         }

//         // ---- 3. Filter matches --------------------------------------------
//         bool isGood = false;
//         if (bestIdx != -1) {
//             if (method == MatchMethod::SSD) {
//                 // Ratio test for SSD
//                 if (bestScore < ssdRatioThresh * secondBestScore) {
//                     isGood = true;
//                 }
//             } else { // NCC
//                 // For NCC, we simply threshold on a high correlation value
//                 if (bestScore > 0.8) {
//                     isGood = true;
//                 }
//             }
//         }

//         if (isGood) {
//             goodMatches.push_back(cv::DMatch(i, bestIdx, (float)bestScore));
//         }
//     }

//     // ---- 4. Draw matches --------------------------------------------------
//     cv::Mat img1Mat = qimageToMat(img1);
//     cv::Mat img2Mat = qimageToMat(img2);
//     cv::Mat matchMat;

//     // Use solid green for match lines, and solid red for the keypoints
//     cv::drawMatches(img1Mat, s1.keypoints, img2Mat, s2.keypoints, goodMatches, matchMat,
//                     cv::Scalar(0, 255, 0), cv::Scalar(0, 0, 255), std::vector<char>(),
//                     cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

//     // ---- 5. Measure time --------------------------------------------------
//     auto t1 = std::chrono::high_resolution_clock::now();
//     double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

//     MatchResult result;
//     result.matchImage = matToQimage(matchMat);
//     result.matchCount = (int)goodMatches.size();
//     result.totalSourceFeatures = rows1;
//     result.timeMs     = ms;

//     return result;
// }






































// #include "backend.h"

// #include <QPainter>
// #include <QColor>
// #include <chrono>
// #include <cmath>
// #include <algorithm>

// // ---------------------------------------------------------------------------
// // Constructor
// // ---------------------------------------------------------------------------
// Backend::Backend() {}

// // ---------------------------------------------------------------------------
// // Helper: convert QImage → 2-D double grid (grayscale, values 0–255)
// // ---------------------------------------------------------------------------
// std::vector<std::vector<double>>
// Backend::toGrayGrid(const QImage& img)
// {
//     int W = img.width();
//     int H = img.height();
//     std::vector<std::vector<double>> g(H, std::vector<double>(W, 0.0));

//     for (int y = 0; y < H; ++y) {
//         for (int x = 0; x < W; ++x) {
//             QColor c(img.pixel(x, y));
//             // standard luminance formula
//             g[y][x] = 0.299 * c.red() + 0.587 * c.green() + 0.114 * c.blue();
//         }
//     }
//     return g;
// }

// // ---------------------------------------------------------------------------
// // Helper: box blur (uniform average) with given radius
// // ---------------------------------------------------------------------------
// std::vector<std::vector<double>>
// Backend::boxBlur(const std::vector<std::vector<double>>& src, int radius)
// {
//     if (radius <= 0) return src;
//     int H = (int)src.size();
//     int W = (int)src[0].size();

//     // Horizontal pass
//     std::vector<std::vector<double>> tmp(H, std::vector<double>(W, 0.0));
//     for (int y = 0; y < H; ++y) {
//         for (int x = 0; x < W; ++x) {
//             double sum = 0.0;
//             int cnt = 0;
//             for (int dx = -radius; dx <= radius; ++dx) {
//                 int nx = std::clamp(x + dx, 0, W - 1);
//                 sum += src[y][nx];
//                 ++cnt;
//             }
//             tmp[y][x] = sum / cnt;
//         }
//     }

//     // Vertical pass
//     std::vector<std::vector<double>> out(H, std::vector<double>(W, 0.0));
//     for (int y = 0; y < H; ++y) {
//         for (int x = 0; x < W; ++x) {
//             double sum = 0.0;
//             int cnt = 0;
//             for (int dy = -radius; dy <= radius; ++dy) {
//                 int ny = std::clamp(y + dy, 0, H - 1);
//                 sum += tmp[ny][x];
//                 ++cnt;
//             }
//             out[y][x] = sum / cnt;
//         }
//     }
//     return out;
// }

// // ---------------------------------------------------------------------------
// // Main: Harris corner detector
// // ---------------------------------------------------------------------------
// HarrisResult Backend::runHarris(const QImage& input,
//                                 double k,
//                                 int    blockSize,
//                                 double threshold)
// {
//     auto t0 = std::chrono::high_resolution_clock::now();

//     // ---- 1. Grayscale grid ------------------------------------------------
//     auto gray = toGrayGrid(input.convertToFormat(QImage::Format_RGB32));
//     int H = (int)gray.size();
//     int W = (int)gray[0].size();

//     // ---- 2. Sobel gradients Ix, Iy ----------------------------------------
//     // Sobel-X kernel: [-1 0 1; -2 0 2; -1 0 1]
//     // Sobel-Y kernel: [-1 -2 -1; 0 0 0; 1 2 1]
//     std::vector<std::vector<double>> Ix(H, std::vector<double>(W, 0.0));
//     std::vector<std::vector<double>> Iy(H, std::vector<double>(W, 0.0));

//     for (int y = 1; y < H - 1; ++y) {
//         for (int x = 1; x < W - 1; ++x) {
//             Ix[y][x] = (-gray[y-1][x-1] + gray[y-1][x+1]
//                         - 2*gray[y][x-1]   + 2*gray[y][x+1]
//                         - gray[y+1][x-1] + gray[y+1][x+1]) / 8.0;

//             Iy[y][x] = (-gray[y-1][x-1] - 2*gray[y-1][x] - gray[y-1][x+1]
//                         + gray[y+1][x-1] + 2*gray[y+1][x]   + gray[y+1][x+1]) / 8.0;
//         }
//     }

//     // ---- 3. Structure tensor products -------------------------------------
//     std::vector<std::vector<double>> Ixx(H, std::vector<double>(W, 0.0));
//     std::vector<std::vector<double>> Iyy(H, std::vector<double>(W, 0.0));
//     std::vector<std::vector<double>> Ixy(H, std::vector<double>(W, 0.0));

//     for (int y = 0; y < H; ++y)
//         for (int x = 0; x < W; ++x) {
//             Ixx[y][x] = Ix[y][x] * Ix[y][x];
//             Iyy[y][x] = Iy[y][x] * Iy[y][x];
//             Ixy[y][x] = Ix[y][x] * Iy[y][x];
//         }

//     // ---- 4. Gaussian smoothing (box blur approximation) ------------------
//     int radius = std::max(1, blockSize / 2);
//     Ixx = boxBlur(Ixx, radius);
//     Iyy = boxBlur(Iyy, radius);
//     Ixy = boxBlur(Ixy, radius);

//     // ---- 5. Harris response R = det(M) - k * trace(M)^2 -----------------
//     std::vector<std::vector<double>> R(H, std::vector<double>(W, 0.0));
//     double maxR = 0.0;

//     for (int y = 0; y < H; ++y)
//         for (int x = 0; x < W; ++x) {
//             double det   = Ixx[y][x] * Iyy[y][x] - Ixy[y][x] * Ixy[y][x];
//             double trace = Ixx[y][x] + Iyy[y][x];
//             R[y][x] = det - k * trace * trace;
//             if (R[y][x] > maxR) maxR = R[y][x];
//         }

//     // ---- 6. Threshold + non-maximum suppression (3×3 window) -------------
//     double cutoff = threshold * maxR;
//     std::vector<std::pair<int,int>> corners;

//     for (int y = 1; y < H - 1; ++y) {
//         for (int x = 1; x < W - 1; ++x) {
//             if (R[y][x] < cutoff) continue;
//             // local max check
//             bool isMax = true;
//             for (int dy = -1; dy <= 1 && isMax; ++dy)
//                 for (int dx = -1; dx <= 1 && isMax; ++dx)
//                     if (!(dy == 0 && dx == 0) && R[y+dy][x+dx] >= R[y][x])
//                         isMax = false;
//             if (isMax)
//                 corners.emplace_back(x, y);
//         }
//     }

//     // ---- 7. Draw corners on output image ----------------------------------
//     QImage output = input.convertToFormat(QImage::Format_RGB32);
//     QPainter painter(&output);
//     painter.setPen(QPen(Qt::red, 3));
//     int markerRadius = std::max(4, std::min(W, H) / 100);
//     for (auto& [cx, cy] : corners)
//         painter.drawEllipse(QPoint(cx, cy), markerRadius, markerRadius);
//     painter.end();

//     // ---- 8. Timing --------------------------------------------------------
//     auto t1 = std::chrono::high_resolution_clock::now();
//     double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

//     HarrisResult result;
//     result.outputImage  = output;
//     result.cornerCount  = (int)corners.size();
//     result.timeMs       = ms;
//     return result;
// }