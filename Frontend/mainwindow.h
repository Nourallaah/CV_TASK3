#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QLabel>
#include <QKeyEvent>
#include <QResizeEvent>
#include "backend.h"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QButtonGroup;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;
    void keyPressEvent(QKeyEvent* ev) override;
    void resizeEvent(QResizeEvent* ev) override;   // re-fit images on resize

private slots:
    void on_removeButton_clicked();
    void on_runHarrisButton_clicked();
    void runLambdaMinusButton_clicked(); // <-- Changed name here
    void on_runSiftButton_clicked();
    void on_runMatchButton_clicked();

private:
    Ui::MainWindow *ui;
    Backend        backend;
    QButtonGroup* detectorGroup;

    // Original loaded images (always clean for matching)
    QImage currentImage1;
    QImage currentImage2;

    // What is currently displayed in each panel (may have features drawn on it)
    QImage displayImage1;
    QImage displayImage2;
    QImage displayMatchResult;

    void loadImage(int imageIndex);
    void clearImages();

    // Scale img to fit inside lbl bounds using KeepAspectRatio
    void showInLabel(const QImage& img, QLabel* lbl);

    // Re-fit all currently displayed images (called on resize)
    void reflowImages();

    // <-- Changed variable name here
    void runCornerDetector(bool useLambdaMinus);
};

#endif // MAINWINDOW_H