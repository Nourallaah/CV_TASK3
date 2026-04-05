#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QLabel>
#include <QKeyEvent>
#include "backend.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;
    void keyPressEvent(QKeyEvent* ev) override;

private slots:
    void on_removeButton_clicked();
    void on_runHarrisButton_clicked();

private:
    Ui::MainWindow *ui;
    Backend        backend;
    QImage         currentImage;

    void loadImage();          // shows file dialog and loads
    void clearImage();         // removes current image
    QPixmap fitToLabel(const QImage& img, QLabel* lbl);
};

#endif // MAINWINDOW_H
