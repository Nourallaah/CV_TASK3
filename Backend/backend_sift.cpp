#include "backend.h"

#include <QPainter>
#include <chrono>
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ---------------------------------------------------------------------------
// Helper: 1D Separable Gaussian Blur
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
// Helper: Grid Subtraction
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
// Main: SIFT Feature Descriptor Generation
// ---------------------------------------------------------------------------
SiftResult Backend::runSift(const QImage& input)
{
    auto t0 = std::chrono::high_resolution_clock::now();

    auto gray = toGrayGrid(input.convertToFormat(QImage::Format_RGB32));
    int H = gray.size();
    int W = gray[0].size();

    double sigmas[5] = {1.0, 1.414, 2.0, 2.828, 4.0};
    std::vector<std::vector<std::vector<double>>> gaussians;
    for(int i = 0; i < 5; ++i) {
        gaussians.push_back(gaussianBlur(gray, sigmas[i]));
    }

    std::vector<std::vector<std::vector<double>>> dogs;
    for(int i = 0; i < 4; ++i) {
        dogs.push_back(subtractGrids(gaussians[i+1], gaussians[i]));
    }

    std::vector<cv::KeyPoint> keypoints;
    for(int s = 1; s < 3; ++s) {
        for(int y = 16; y < H - 16; ++y) {
            for(int x = 16; x < W - 16; ++x) {

                double val = dogs[s][y][x];
                if (std::abs(val) < 3.0) continue;

                bool isMax = true, isMin = true;
                for(int ds = -1; ds <= 1; ++ds) {
                    for(int dy = -1; dy <= 1; ++dy) {
                        for(int dx = -1; dx <= 1; ++dx) {
                            if(ds == 0 && dy == 0 && dx == 0) continue;
                            double n_val = dogs[s+ds][y+dy][x+dx];
                            if(val <= n_val) isMax = false;
                            if(val >= n_val) isMin = false;
                        }
                    }
                }

                if(isMax || isMin) {
                    double dxx = dogs[s][y][x+1] + dogs[s][y][x-1] - 2*val;
                    double dyy = dogs[s][y+1][x] + dogs[s][y-1][x] - 2*val;
                    double dxy = (dogs[s][y+1][x+1] - dogs[s][y+1][x-1] - dogs[s][y-1][x+1] + dogs[s][y-1][x-1]) / 4.0;

                    double tr = dxx + dyy;
                    double det = dxx*dyy - dxy*dxy;

                    if (det <= 0) continue;

                    double r = 10.0;
                    if ((tr*tr)/det > ((r+1)*(r+1))/r) continue;

                    cv::KeyPoint kp;
                    kp.pt.x = x;
                    kp.pt.y = y;
                    kp.size = sigmas[s] * 3.0;
                    keypoints.push_back(kp);
                }
            }
        }
    }

    cv::Mat descriptors(keypoints.size(), 128, CV_32F);
    int kpIdx = 0;

    for(auto& kp : keypoints) {
        int px = kp.pt.x;
        int py = kp.pt.y;
        std::vector<float> desc(128, 0.0f);

        // 1. Assign dominant orientation to the keypoint
        double dominant_theta = 0.0;
        double max_weight = -1.0;
        std::vector<double> ori_hist(36, 0.0);

        for(int dy = -8; dy < 8; ++dy) {
            for(int dx = -8; dx < 8; ++dx) {
                int nx = px + dx;
                int ny = py + dy;

                if (nx < 1 || nx >= W - 1 || ny < 1 || ny >= H - 1) continue;

                double gx = gaussians[1][ny][nx+1] - gaussians[1][ny][nx-1];
                double gy = gaussians[1][ny+1][nx] - gaussians[1][ny-1][nx];
                
                double mag = std::sqrt(gx*gx + gy*gy);
                double th = std::atan2(gy, gx) * 180.0 / M_PI;
                if(th < 0) th += 360.0;
                
                double gw = std::exp(-(dx*dx + dy*dy) / (2 * 4.0 * 4.0));
                
                int bin = (int)(th / 10.0) % 36;
                ori_hist[bin] += mag * gw;
            }
        }
        
        for(int i = 0; i < 36; ++i) {
            if(ori_hist[i] > max_weight) {
                max_weight = ori_hist[i];
                dominant_theta = i * 10.0 + 5.0; // Center of the bin
            }
        }

        // 2. Compute rotation-invariant descriptor
        double cos_t = std::cos(dominant_theta * M_PI / 180.0);
        double sin_t = std::sin(dominant_theta * M_PI / 180.0);

        for(int dy = -8; dy < 8; ++dy) {
            for(int dx = -8; dx < 8; ++dx) {
                // Rotate the patch coordinates
                int rot_x = (int)std::round(dx * cos_t - dy * sin_t);
                int rot_y = (int)std::round(dx * sin_t + dy * cos_t);
                
                int nx = px + rot_x;
                int ny = py + rot_y;

                if (nx < 1 || nx >= W - 1 || ny < 1 || ny >= H - 1) continue;

                double gx = gaussians[1][ny][nx+1] - gaussians[1][ny][nx-1];
                double gy = gaussians[1][ny+1][nx] - gaussians[1][ny-1][nx];

                double mag = std::sqrt(gx*gx + gy*gy);
                double theta = std::atan2(gy, gx) * 180.0 / M_PI;
                if(theta < 0) theta += 360.0;
                
                // Adjust gradient angle relative to the dominant orientation
                theta -= dominant_theta;
                if(theta < 0) theta += 360.0;
                
                double gw = std::exp(-(dx*dx + dy*dy) / (2 * 4.0 * 4.0));
                mag *= gw;

                int r = (dy + 8) / 4;
                int c = (dx + 8) / 4;
                int bin = (int)(theta / 45.0) % 8;

                desc[(r*4 + c)*8 + bin] += mag;
            }
        }

        float sum_sq = 0;
        for(float v : desc) sum_sq += v*v;
        sum_sq = std::sqrt(sum_sq);

        for(float& v : desc) {
            v /= (sum_sq + 1e-7);
            if(v > 0.2f) v = 0.2f;
        }

        sum_sq = 0;
        for(float v : desc) sum_sq += v*v;
        sum_sq = std::sqrt(sum_sq);

        for(int i = 0; i < 128; ++i) {
            descriptors.at<float>(kpIdx, i) = desc[i] / (sum_sq + 1e-7);
        }
        kpIdx++;
    }

    QImage output = input.convertToFormat(QImage::Format_RGB32);
    QPainter painter(&output);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::green, 1));

    for(auto& kp : keypoints) {
        painter.drawEllipse(QPoint(kp.pt.x, kp.pt.y), (int)kp.size, (int)kp.size);
    }
    painter.end();

    auto t1 = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    SiftResult result;
    result.outputImage   = output;
    result.keypointCount = keypoints.size();
    result.timeMs        = ms;
    result.descriptors   = descriptors;
    result.keypoints     = keypoints;

    return result;
}