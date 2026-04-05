#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QLabel>
#include <QString>
#include <QApplication>
#include <QEvent>
#include <QMouseEvent>

// ─────────────────────────────────────────────────────────────────────────────
//  Stylesheet — bright white & blue theme
// ─────────────────────────────────────────────────────────────────────────────
static const char* APP_STYLE = R"(

/* ── Base ── */
QMainWindow, QWidget {
    background-color: #f0f5ff;
    color: #0a1a3a;
    font-family: 'Segoe UI', Arial, sans-serif;
    font-size: 13px;
}

/* ── Group boxes ── */
QGroupBox {
    background-color: #ffffff;
    border: 2px solid #b8d4ff;
    border-radius: 12px;
    margin-top: 20px;
    padding: 10px 8px 8px 8px;
    font-weight: bold;
    font-size: 12px;
    color: #1a50a0;
}
QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    padding: 2px 12px;
    background-color: #dceeff;
    border: 1px solid #90c0ff;
    border-radius: 6px;
    color: #0055cc;
}

/* ── Image preview labels ── */
QLabel#labelOriginal {
    background-color: #e8f2ff;
    border: 2px dashed #5599ee;
    border-radius: 10px;
    color: #6688bb;
    font-size: 15px;
    font-style: italic;
}
QLabel#labelOriginal:hover {
    background-color: #dceeff;
    border: 2px dashed #1a6fdf;
    color: #1a4090;
}
QLabel#labelResult {
    background-color: #f5faff;
    border: 2px dashed #b0ccf0;
    border-radius: 10px;
    color: #8899cc;
    font-size: 15px;
    font-style: italic;
}

/* ── File path label ── */
QLabel#filePathLabel {
    color: #4466aa;
    font-size: 12px;
    font-style: italic;
}

/* ── Results labels ── */
QLabel#labelCornerCount {
    color: #cc2244;
    font-weight: bold;
    font-size: 14px;
}
QLabel#labelTime {
    color: #007744;
    font-weight: bold;
    font-size: 14px;
}

/* ── General labels inside controls ── */
QGroupBox QLabel {
    color: #224488;
    font-weight: bold;
    font-size: 12px;
    background: transparent;
}

/* ── Spin boxes ── */
QDoubleSpinBox, QSpinBox {
    background-color: #ffffff;
    color: #0a1a3a;
    border: 1.5px solid #88bbff;
    border-radius: 7px;
    padding: 4px 8px;
    selection-background-color: #aaccff;
    font-size: 13px;
}
QDoubleSpinBox:hover, QSpinBox:hover {
    border: 1.5px solid #3388ff;
}
QDoubleSpinBox:focus, QSpinBox:focus {
    border: 2px solid #1166ee;
}

/* Up/down buttons */
QDoubleSpinBox::up-button, QSpinBox::up-button {
    subcontrol-origin: border;
    subcontrol-position: top right;
    width: 22px;
    background-color: #ddeeff;
    border-left: 1px solid #88bbff;
    border-bottom: 1px solid #88bbff;
    border-top-right-radius: 7px;
}
QDoubleSpinBox::up-button:hover, QSpinBox::up-button:hover {
    background-color: #aaccff;
}
QDoubleSpinBox::up-button:pressed, QSpinBox::up-button:pressed {
    background-color: #88aaee;
}
QDoubleSpinBox::down-button, QSpinBox::down-button {
    subcontrol-origin: border;
    subcontrol-position: bottom right;
    width: 22px;
    background-color: #ddeeff;
    border-left: 1px solid #88bbff;
    border-top: 1px solid #88bbff;
    border-bottom-right-radius: 7px;
}
QDoubleSpinBox::down-button:hover, QSpinBox::down-button:hover {
    background-color: #aaccff;
}
QDoubleSpinBox::down-button:pressed, QSpinBox::down-button:pressed {
    background-color: #88aaee;
}

/* Triangle arrows via SVG resources */
QDoubleSpinBox::up-arrow, QSpinBox::up-arrow {
    image: url(:/Resources/up_arrow.svg);
    width: 10px;
    height: 7px;
}
QDoubleSpinBox::down-arrow, QSpinBox::down-arrow {
    image: url(:/Resources/down_arrow.svg);
    width: 10px;
    height: 7px;
}
QDoubleSpinBox::up-arrow:disabled, QSpinBox::up-arrow:disabled,
QDoubleSpinBox::down-arrow:disabled, QSpinBox::down-arrow:disabled {
    image: none;
}

/* ── Remove button (red-tinted) ── */
QPushButton#removeButton {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 #ffffff, stop:1 #ffe0e6);
    color: #990022;
    border: 1.5px solid #ffaabb;
    border-radius: 8px;
    padding: 6px 14px;
    font-weight: bold;
    font-size: 13px;
}
QPushButton#removeButton:hover {
    background: #ffccd5;
    border-color: #ee4466;
    color: #770011;
}
QPushButton#removeButton:pressed { background: #ffaabb; }
QPushButton#removeButton:disabled {
    background: #f5f5f7;
    color: #aaaacc;
    border-color: #ddddee;
}

/* ── Run Harris button (bright blue) ── */
QPushButton#runHarrisButton {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 #3399ff, stop:1 #0055cc);
    color: #ffffff;
    border: 1.5px solid #0044bb;
    border-radius: 8px;
    padding: 10px 22px;
    font-weight: bold;
    font-size: 14px;
    letter-spacing: 0.4px;
}
QPushButton#runHarrisButton:hover {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 #55aaff, stop:1 #1166ee);
}
QPushButton#runHarrisButton:pressed {
    background: #0033aa;
}
QPushButton#runHarrisButton:disabled {
    background: #c8d8f0;
    color: #7788aa;
    border-color: #aabbd0;
}

/* ── Run SIFT button (green) ── */
QPushButton#runSiftButton {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 #33cc77, stop:1 #008844);
    color: #ffffff;
    border: 1.5px solid #006633;
    border-radius: 8px;
    padding: 10px 22px;
    font-weight: bold;
    font-size: 14px;
    letter-spacing: 0.4px;
}
QPushButton#runSiftButton:hover {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 #55ee99, stop:1 #11aa55);
}
QPushButton#runSiftButton:pressed {
    background: #005522;
}
QPushButton#runSiftButton:disabled {
    background: #c8f0d8;
    color: #77aa88;
    border-color: #aaddbb;
}


/* ── Menu / status bar ── */
QMenuBar {
    background-color: #dceeff;
    color: #1a3a7a;
    border-bottom: 1px solid #b0ccf0;
}
QMenuBar::item:selected { background: #aaccff; border-radius: 4px; }
QStatusBar {
    background-color: #dceeff;
    color: #3355aa;
    font-size: 12px;
    border-top: 1px solid #b0ccf0;
}
)";

// ─────────────────────────────────────────────────────────────────────────────
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setStyleSheet(APP_STYLE);

    // Install event filter on the original image label for double-click
    ui->labelOriginal->installEventFilter(this);

    statusBar()->showMessage("Ready — double-click the left panel to upload an image");
}

MainWindow::~MainWindow() { delete ui; }

// ─────────────────────────────────────────────────────────────────────────────
//  Event filter — catches double-click on labelOriginal
// ─────────────────────────────────────────────────────────────────────────────
bool MainWindow::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj == ui->labelOriginal &&
        ev->type() == QEvent::MouseButtonDblClick) {
        loadImage();
        return true;
    }
    return QMainWindow::eventFilter(obj, ev);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Key press — Delete clears the image
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::keyPressEvent(QKeyEvent* ev)
{
    if (ev->key() == Qt::Key_Delete && !currentImage.isNull()) {
        clearImage();
        return;
    }
    QMainWindow::keyPressEvent(ev);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Helpers
// ─────────────────────────────────────────────────────────────────────────────
QPixmap MainWindow::fitToLabel(const QImage& img, QLabel* lbl)
{
    return QPixmap::fromImage(img).scaled(
        lbl->width()  - 6,
        lbl->height() - 6,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation);
}

void MainWindow::loadImage()
{
    QString path = QFileDialog::getOpenFileName(
        this,
        tr("Open Image"),
        QString(),
        tr("Images (*.png *.jpg *.jpeg *.bmp *.tiff *.tif *.pgm *.ppm);;All files (*)"));

    if (path.isEmpty()) return;

    QImage img(path);
    if (img.isNull()) {
        QMessageBox::warning(this, tr("Load Error"),
            tr("Could not read the image.\nSupported: PNG, JPG, BMP, TIFF, PGM, PPM."));
        return;
    }

    currentImage = img;

    // Truncate long path for display
    QString display = path.length() > 90 ? "…" + path.right(87) : path;
    ui->filePathLabel->setText(display);

    ui->labelOriginal->setPixmap(fitToLabel(currentImage, ui->labelOriginal));
    ui->labelResult->clear();
    ui->labelResult->setText("Run the detector to see results here");
    ui->labelCornerCount->setText("Features detected: —");
    ui->labelTime->setText("Computation time: —");
    ui->removeButton->setEnabled(true);
    ui->runHarrisButton->setEnabled(true);
    ui->runSiftButton->setEnabled(true);

    statusBar()->showMessage(
        QString("Loaded: %1 × %2 px  |  %3  |  %4")
            .arg(img.width()).arg(img.height())
            .arg(img.isGrayscale() ? "Grayscale" : "Color")
            .arg(path));
}

void MainWindow::clearImage()
{
    currentImage = QImage();
    ui->labelOriginal->clear();
    ui->labelOriginal->setText("Double-click here to upload an image");
    ui->labelResult->clear();
    ui->labelResult->setText("Run the detector to see results here");
    ui->filePathLabel->setText("No image loaded — double-click the image panel to upload");
    ui->labelCornerCount->setText("Features detected: —");
    ui->labelTime->setText("Computation time: —");
    ui->removeButton->setEnabled(false);
    ui->runHarrisButton->setEnabled(false);
    ui->runSiftButton->setEnabled(false);
    statusBar()->showMessage("Image removed — double-click the left panel to upload a new one");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Slots
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::on_removeButton_clicked()
{
    clearImage();
}

void MainWindow::on_runHarrisButton_clicked()
{
    if (currentImage.isNull()) return;

    ui->runHarrisButton->setEnabled(false);
    ui->runHarrisButton->setText("Running...");
    statusBar()->showMessage("Running Harris corner detection…");
    QApplication::processEvents();

    double k         = ui->spinK->value();
    int    blockSize = ui->spinBlockSize->value();
    double threshold = ui->spinThreshold->value();

    HarrisResult res = backend.runHarris(currentImage, k, blockSize, threshold);

    ui->labelResult->setPixmap(fitToLabel(res.outputImage, ui->labelResult));
    ui->labelCornerCount->setText(
        QString("Corners detected: %1").arg(res.cornerCount));
    ui->labelTime->setText(
        QString("Computation time: %1 ms").arg(res.timeMs, 0, 'f', 2));

    ui->runHarrisButton->setText("Run Harris Detector");
    ui->runHarrisButton->setEnabled(true);

    statusBar()->showMessage(
        QString("Done — %1 corners  |  %2 ms  |  k=%3  block=%4  threshold=%5")
            .arg(res.cornerCount)
            .arg(res.timeMs, 0, 'f', 2)
            .arg(k, 0, 'f', 3)
            .arg(blockSize)
            .arg(threshold, 0, 'f', 3));
}

void MainWindow::on_runSiftButton_clicked()
{
    if (currentImage.isNull()) return;

    ui->runSiftButton->setEnabled(false);
    ui->runSiftButton->setText("Running SIFT...");
    statusBar()->showMessage("Running SIFT feature extraction…");
    QApplication::processEvents();

    SiftResult res = backend.runSift(currentImage);

    // Update the UI
    ui->labelResult->setPixmap(fitToLabel(res.outputImage, ui->labelResult));
    ui->labelCornerCount->setText(
        QString("Features detected: %1").arg(res.keypointCount));
    ui->labelTime->setText(
        QString("Computation time: %1 ms").arg(res.timeMs, 0, 'f', 2));

    ui->runSiftButton->setText("Run SIFT Descriptors");
    ui->runSiftButton->setEnabled(true);

    statusBar()->showMessage(
        QString("Done — %1 features  |  %2 ms")
            .arg(res.keypointCount)
            .arg(res.timeMs, 0, 'f', 2));
}




































// #include "mainwindow.h"
// #include "ui_mainwindow.h"

// #include <QFileDialog>
// #include <QMessageBox>
// #include <QPixmap>
// #include <QLabel>
// #include <QString>
// #include <QApplication>
// #include <QEvent>
// #include <QMouseEvent>

// // ─────────────────────────────────────────────────────────────────────────────
// //  Stylesheet — bright white & blue theme
// // ─────────────────────────────────────────────────────────────────────────────
// static const char* APP_STYLE = R"(

// /* ── Base ── */
// QMainWindow, QWidget {
//     background-color: #f0f5ff;
//     color: #0a1a3a;
//     font-family: 'Segoe UI', Arial, sans-serif;
//     font-size: 13px;
// }

// /* ── Group boxes ── */
// QGroupBox {
//     background-color: #ffffff;
//     border: 2px solid #b8d4ff;
//     border-radius: 12px;
//     margin-top: 20px;
//     padding: 10px 8px 8px 8px;
//     font-weight: bold;
//     font-size: 12px;
//     color: #1a50a0;
// }
// QGroupBox::title {
//     subcontrol-origin: margin;
//     subcontrol-position: top left;
//     padding: 2px 12px;
//     background-color: #dceeff;
//     border: 1px solid #90c0ff;
//     border-radius: 6px;
//     color: #0055cc;
// }

// /* ── Image preview labels ── */
// QLabel#labelOriginal {
//     background-color: #e8f2ff;
//     border: 2px dashed #5599ee;
//     border-radius: 10px;
//     color: #6688bb;
//     font-size: 15px;
//     font-style: italic;
// }
// QLabel#labelOriginal:hover {
//     background-color: #dceeff;
//     border: 2px dashed #1a6fdf;
//     color: #1a4090;
// }
// QLabel#labelResult {
//     background-color: #f5faff;
//     border: 2px dashed #b0ccf0;
//     border-radius: 10px;
//     color: #8899cc;
//     font-size: 15px;
//     font-style: italic;
// }

// /* ── File path label ── */
// QLabel#filePathLabel {
//     color: #4466aa;
//     font-size: 12px;
//     font-style: italic;
// }

// /* ── Results labels ── */
// QLabel#labelCornerCount {
//     color: #cc2244;
//     font-weight: bold;
//     font-size: 14px;
// }
// QLabel#labelTime {
//     color: #007744;
//     font-weight: bold;
//     font-size: 14px;
// }

// /* ── General labels inside controls ── */
// QGroupBox QLabel {
//     color: #224488;
//     font-weight: bold;
//     font-size: 12px;
//     background: transparent;
// }

// /* ── Spin boxes ── */
// QDoubleSpinBox, QSpinBox {
//     background-color: #ffffff;
//     color: #0a1a3a;
//     border: 1.5px solid #88bbff;
//     border-radius: 7px;
//     padding: 4px 8px;
//     selection-background-color: #aaccff;
//     font-size: 13px;
// }
// QDoubleSpinBox:hover, QSpinBox:hover {
//     border: 1.5px solid #3388ff;
// }
// QDoubleSpinBox:focus, QSpinBox:focus {
//     border: 2px solid #1166ee;
// }

// /* Up/down buttons */
// QDoubleSpinBox::up-button, QSpinBox::up-button {
//     subcontrol-origin: border;
//     subcontrol-position: top right;
//     width: 22px;
//     background-color: #ddeeff;
//     border-left: 1px solid #88bbff;
//     border-bottom: 1px solid #88bbff;
//     border-top-right-radius: 7px;
// }
// QDoubleSpinBox::up-button:hover, QSpinBox::up-button:hover {
//     background-color: #aaccff;
// }
// QDoubleSpinBox::up-button:pressed, QSpinBox::up-button:pressed {
//     background-color: #88aaee;
// }
// QDoubleSpinBox::down-button, QSpinBox::down-button {
//     subcontrol-origin: border;
//     subcontrol-position: bottom right;
//     width: 22px;
//     background-color: #ddeeff;
//     border-left: 1px solid #88bbff;
//     border-top: 1px solid #88bbff;
//     border-bottom-right-radius: 7px;
// }
// QDoubleSpinBox::down-button:hover, QSpinBox::down-button:hover {
//     background-color: #aaccff;
// }
// QDoubleSpinBox::down-button:pressed, QSpinBox::down-button:pressed {
//     background-color: #88aaee;
// }

// /* Triangle arrows via SVG resources */
// QDoubleSpinBox::up-arrow, QSpinBox::up-arrow {
//     image: url(:/Resources/up_arrow.svg);
//     width: 10px;
//     height: 7px;
// }
// QDoubleSpinBox::down-arrow, QSpinBox::down-arrow {
//     image: url(:/Resources/down_arrow.svg);
//     width: 10px;
//     height: 7px;
// }
// QDoubleSpinBox::up-arrow:disabled, QSpinBox::up-arrow:disabled,
// QDoubleSpinBox::down-arrow:disabled, QSpinBox::down-arrow:disabled {
//     image: none;
// }

// /* ── Remove button (red-tinted) ── */
// QPushButton#removeButton {
//     background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
//         stop:0 #ffffff, stop:1 #ffe0e6);
//     color: #990022;
//     border: 1.5px solid #ffaabb;
//     border-radius: 8px;
//     padding: 6px 14px;
//     font-weight: bold;
//     font-size: 13px;
// }
// QPushButton#removeButton:hover {
//     background: #ffccd5;
//     border-color: #ee4466;
//     color: #770011;
// }
// QPushButton#removeButton:pressed { background: #ffaabb; }
// QPushButton#removeButton:disabled {
//     background: #f5f5f7;
//     color: #aaaacc;
//     border-color: #ddddee;
// }

// /* ── Run Harris button (bright blue) ── */
// QPushButton#runHarrisButton {
//     background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
//         stop:0 #3399ff, stop:1 #0055cc);
//     color: #ffffff;
//     border: 1.5px solid #0044bb;
//     border-radius: 8px;
//     padding: 10px 22px;
//     font-weight: bold;
//     font-size: 14px;
//     letter-spacing: 0.4px;
// }
// QPushButton#runHarrisButton:hover {
//     background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
//         stop:0 #55aaff, stop:1 #1166ee);
// }
// QPushButton#runHarrisButton:pressed {
//     background: #0033aa;
// }
// QPushButton#runHarrisButton:disabled {
//     background: #c8d8f0;
//     color: #7788aa;
//     border-color: #aabbd0;
// }

// /* ── Menu / status bar ── */
// QMenuBar {
//     background-color: #dceeff;
//     color: #1a3a7a;
//     border-bottom: 1px solid #b0ccf0;
// }
// QMenuBar::item:selected { background: #aaccff; border-radius: 4px; }
// QStatusBar {
//     background-color: #dceeff;
//     color: #3355aa;
//     font-size: 12px;
//     border-top: 1px solid #b0ccf0;
// }
// )";

// // ─────────────────────────────────────────────────────────────────────────────
// MainWindow::MainWindow(QWidget *parent)
//     : QMainWindow(parent)
//     , ui(new Ui::MainWindow)
// {
//     ui->setupUi(this);
//     setStyleSheet(APP_STYLE);

//     // Install event filter on the original image label for double-click
//     ui->labelOriginal->installEventFilter(this);

//     statusBar()->showMessage("Ready — double-click the left panel to upload an image");
// }

// MainWindow::~MainWindow() { delete ui; }

// // ─────────────────────────────────────────────────────────────────────────────
// //  Event filter — catches double-click on labelOriginal
// // ─────────────────────────────────────────────────────────────────────────────
// bool MainWindow::eventFilter(QObject* obj, QEvent* ev)
// {
//     if (obj == ui->labelOriginal &&
//         ev->type() == QEvent::MouseButtonDblClick) {
//         loadImage();
//         return true;
//     }
//     return QMainWindow::eventFilter(obj, ev);
// }

// // ─────────────────────────────────────────────────────────────────────────────
// //  Key press — Delete clears the image
// // ─────────────────────────────────────────────────────────────────────────────
// void MainWindow::keyPressEvent(QKeyEvent* ev)
// {
//     if (ev->key() == Qt::Key_Delete && !currentImage.isNull()) {
//         clearImage();
//         return;
//     }
//     QMainWindow::keyPressEvent(ev);
// }

// // ─────────────────────────────────────────────────────────────────────────────
// //  Helpers
// // ─────────────────────────────────────────────────────────────────────────────
// QPixmap MainWindow::fitToLabel(const QImage& img, QLabel* lbl)
// {
//     return QPixmap::fromImage(img).scaled(
//         lbl->width()  - 6,
//         lbl->height() - 6,
//         Qt::KeepAspectRatio,
//         Qt::SmoothTransformation);
// }

// void MainWindow::loadImage()
// {
//     QString path = QFileDialog::getOpenFileName(
//         this,
//         tr("Open Image"),
//         QString(),
//         tr("Images (*.png *.jpg *.jpeg *.bmp *.tiff *.tif *.pgm *.ppm);;All files (*)"));

//     if (path.isEmpty()) return;

//     QImage img(path);
//     if (img.isNull()) {
//         QMessageBox::warning(this, tr("Load Error"),
//             tr("Could not read the image.\nSupported: PNG, JPG, BMP, TIFF, PGM, PPM."));
//         return;
//     }

//     currentImage = img;

//     // Truncate long path for display
//     QString display = path.length() > 90 ? "…" + path.right(87) : path;
//     ui->filePathLabel->setText(display);

//     ui->labelOriginal->setPixmap(fitToLabel(currentImage, ui->labelOriginal));
//     ui->labelResult->clear();
//     ui->labelResult->setText("Run the detector to see results here");
//     ui->labelCornerCount->setText("Corners detected: —");
//     ui->labelTime->setText("Computation time: —");
//     ui->removeButton->setEnabled(true);
//     ui->runHarrisButton->setEnabled(true);

//     statusBar()->showMessage(
//         QString("Loaded: %1 × %2 px  |  %3  |  %4")
//             .arg(img.width()).arg(img.height())
//             .arg(img.isGrayscale() ? "Grayscale" : "Color")
//             .arg(path));
// }

// void MainWindow::clearImage()
// {
//     currentImage = QImage();
//     ui->labelOriginal->clear();
//     ui->labelOriginal->setText("Double-click here to upload an image");
//     ui->labelResult->clear();
//     ui->labelResult->setText("Run the detector to see results here");
//     ui->filePathLabel->setText("No image loaded — double-click the image panel to upload");
//     ui->labelCornerCount->setText("Corners detected: —");
//     ui->labelTime->setText("Computation time: —");
//     ui->removeButton->setEnabled(false);
//     ui->runHarrisButton->setEnabled(false);
//     statusBar()->showMessage("Image removed — double-click the left panel to upload a new one");
// }

// // ─────────────────────────────────────────────────────────────────────────────
// //  Slots
// // ─────────────────────────────────────────────────────────────────────────────
// void MainWindow::on_removeButton_clicked()
// {
//     clearImage();
// }

// void MainWindow::on_runHarrisButton_clicked()
// {
//     if (currentImage.isNull()) return;

//     ui->runHarrisButton->setEnabled(false);
//     ui->runHarrisButton->setText("Running...");
//     statusBar()->showMessage("Running Harris corner detection…");
//     QApplication::processEvents();

//     double k         = ui->spinK->value();
//     int    blockSize = ui->spinBlockSize->value();
//     double threshold = ui->spinThreshold->value();

//     HarrisResult res = backend.runHarris(currentImage, k, blockSize, threshold);

//     ui->labelResult->setPixmap(fitToLabel(res.outputImage, ui->labelResult));
//     ui->labelCornerCount->setText(
//         QString("Corners detected: %1").arg(res.cornerCount));
//     ui->labelTime->setText(
//         QString("Computation time: %1 ms").arg(res.timeMs, 0, 'f', 2));

//     ui->runHarrisButton->setText("Run Harris Detector");
//     ui->runHarrisButton->setEnabled(true);

//     statusBar()->showMessage(
//         QString("Done — %1 corners  |  %2 ms  |  k=%3  block=%4  threshold=%5")
//             .arg(res.cornerCount)
//             .arg(res.timeMs, 0, 'f', 2)
//             .arg(k, 0, 'f', 3)
//             .arg(blockSize)
//             .arg(threshold, 0, 'f', 3));
// }
