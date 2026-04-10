#include "backend.h"

#include <QPainter>
#include <QColor>
#include <chrono>
#include <cmath>
#include <algorithm>

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
// Main: Harris & Lambda-Minus corner detector
// ---------------------------------------------------------------------------
HarrisResult Backend::runHarris(const QImage& input, double k, int blockSize, double threshold, bool useLambdaMinus)
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

    // ---- 5. Corner Response (Harris OR Lambda-Minus) ---------
    std::vector<std::vector<double>> R(H, std::vector<double>(W, 0.0));
    double maxR = 0.0;

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {

            if (useLambdaMinus) {
                double trace = Ixx[y][x] + Iyy[y][x];
                double diff = Ixx[y][x] - Iyy[y][x];
                double lambdaMinusValue = (trace - std::sqrt(diff * diff + 4.0 * Ixy[y][x] * Ixy[y][x])) / 2.0;
                R[y][x] = lambdaMinusValue;
            } else {
                double det   = Ixx[y][x] * Iyy[y][x] - Ixy[y][x] * Ixy[y][x];
                double trace = Ixx[y][x] + Iyy[y][x];
                R[y][x] = det - k * trace * trace;
            }

            if (R[y][x] > maxR) {
                maxR = R[y][x];
            }
        }
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