#include "backend.h"

#include <QImage>
#include <chrono>
#include <cmath>
#include <limits>
#include <opencv2/opencv.hpp>

// ---------------------------------------------------------------------------
// Helpers: QImage <-> cv::Mat conversions
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
// Main: Feature Matching using SSD / NCC
// ---------------------------------------------------------------------------
MatchResult Backend::matchFeatures(const QImage& img1, const QImage& img2, MatchMethod method, double ratioThreshold)
{
    auto t0 = std::chrono::high_resolution_clock::now();

    SiftResult s1 = runSift(img1);
    SiftResult s2 = runSift(img2);

    int rows1 = s1.descriptors.rows;
    int rows2 = s2.descriptors.rows;
    int cols  = s1.descriptors.cols;

    std::vector<cv::DMatch> goodMatches;
    double ssdRatioThresh = ratioThreshold * ratioThreshold;

    for (int i = 0; i < rows1; ++i) {
        float* d1 = s1.descriptors.ptr<float>(i);

        double bestScore = (method == MatchMethod::SSD) ? std::numeric_limits<double>::max() : -std::numeric_limits<double>::max();
        double secondBestScore = (method == MatchMethod::SSD) ? std::numeric_limits<double>::max() : -std::numeric_limits<double>::max();
        int bestIdx = -1;

        for (int j = 0; j < rows2; ++j) {
            float* d2 = s2.descriptors.ptr<float>(j);
            double score = 0.0;

            if (method == MatchMethod::SSD) {
                for (int k = 0; k < cols; ++k) {
                    double diff = d1[k] - d2[k];
                    score += diff * diff;
                }
            } else {
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

        bool isGood = false;
        if (bestIdx != -1) {
            if (method == MatchMethod::SSD) {
                if (bestScore < ssdRatioThresh * secondBestScore) {
                    isGood = true;
                }
            } else { // NCC
                if (bestScore > 0.8) {
                    isGood = true;
                }
            }
        }

        if (isGood) {
            goodMatches.push_back(cv::DMatch(i, bestIdx, (float)bestScore));
        }
    }

    cv::Mat img1Mat = qimageToMat(img1);
    cv::Mat img2Mat = qimageToMat(img2);
    cv::Mat matchMat;

    cv::drawMatches(img1Mat, s1.keypoints, img2Mat, s2.keypoints, goodMatches, matchMat,
                    cv::Scalar(0, 255, 0), cv::Scalar(0, 0, 255), std::vector<char>(),
                    cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    auto t1 = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    MatchResult result;
    result.matchImage = matToQimage(matchMat);
    result.matchCount = (int)goodMatches.size();

    result.totalSourceFeatures = rows1;
    result.timeMs     = ms;

    return result;
}