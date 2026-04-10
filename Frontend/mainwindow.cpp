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
QLabel#labelImage1, QLabel#labelImage2 {
    background-color: #e8f2ff;
    border: 2px dashed #5599ee;
    border-radius: 10px;
    color: #6688bb;
    font-size: 15px;
    font-style: italic;
}
QLabel#labelImage1:hover, QLabel#labelImage2:hover {
    background-color: #dceeff;
    border: 2px dashed #1a6fdf;
    color: #1a4090;
}
QLabel#labelMatchResult {
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

/* ── Spin boxes / Combo Boxes ── */
QDoubleSpinBox, QSpinBox, QComboBox {
    background-color: #ffffff;
    color: #0a1a3a;
    border: 1.5px solid #88bbff;
    border-radius: 7px;
    padding: 4px 8px;
    selection-background-color: #aaccff;
    font-size: 13px;
}
QDoubleSpinBox:hover, QSpinBox:hover, QComboBox:hover {
    border: 1.5px solid #3388ff;
}
QDoubleSpinBox:focus, QSpinBox:focus, QComboBox:focus {
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

/* ── Run buttons (bright blue/green) ── */
QPushButton#runHarrisButton, QPushButton#runSiftButton, QPushButton#runMatchButton {
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
QPushButton#runHarrisButton:hover, QPushButton#runSiftButton:hover, QPushButton#runMatchButton:hover {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1,
        stop:0 #55aaff, stop:1 #1166ee);
}
QPushButton#runHarrisButton:pressed, QPushButton#runSiftButton:pressed, QPushButton#runMatchButton:pressed {
    background: #0033aa;
}
QPushButton#runMatchButton {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #9933ff, stop:1 #5500cc);
    border: 1.5px solid #4400aa;
}
QPushButton#runMatchButton:hover {
    background: qlineargradient(x1:0,y1:0,x2:0,y2:1, stop:0 #aa55ff, stop:1 #6611ee);
}
QPushButton#runMatchButton:pressed {
    background: #4400aa;
}
QPushButton:disabled {
    background: #c8d8f0;
    color: #7788aa;
    border-color: #aabbd0;
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

    // --- NEW CODE: Create Two Side-by-Side Toggle Buttons ---

    // 1. Create the Lambda-Minus button and steal the CSS style from the Harris button
    QPushButton* runLambdaMinusButton = new QPushButton("Lambda-Minus (λ-)", this);
    runLambdaMinusButton->setObjectName("runHarrisButton");

    // 2. Make them checkable (toggle buttons)
    ui->runHarrisButton->setCheckable(true);
    runLambdaMinusButton->setCheckable(true);

    // 3. Group them so only ONE can be active at a time (exclusive)
    detectorGroup = new QButtonGroup(this);
    detectorGroup->setExclusive(true);
    detectorGroup->addButton(ui->runHarrisButton);
    detectorGroup->addButton(runLambdaMinusButton);

    // 4. Put them side-by-side in a Horizontal Layout
    if (ui->runHarrisButton->parentWidget() && ui->runHarrisButton->parentWidget()->layout()) {
        QBoxLayout* boxLayout = qobject_cast<QBoxLayout*>(ui->runHarrisButton->parentWidget()->layout());
        if (boxLayout) {
            int buttonIndex = boxLayout->indexOf(ui->runHarrisButton);

            QHBoxLayout* hLayout = new QHBoxLayout();
            hLayout->setContentsMargins(0, 0, 0, 0);
            hLayout->setSpacing(10); // Space between the two buttons

            // Move Harris button to the horizontal layout, then add Lambda-Minus next to it
            boxLayout->removeWidget(ui->runHarrisButton);
            hLayout->addWidget(ui->runHarrisButton);
            hLayout->addWidget(runLambdaMinusButton);

            // Insert them exactly where the old Harris button was
            boxLayout->insertLayout(buttonIndex, hLayout);
        }
    }

    // 5. Connect the new Lambda-Minus button to its click event
    connect(runLambdaMinusButton, &QPushButton::clicked, this, &MainWindow::runLambdaMinusButton_clicked);
    // --------------------------------------------------------
    // Labels must expand/shrink freely with the layout; pixmap scaling is handled manually
    ui->labelImage1->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->labelImage2->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->labelMatchResult->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    ui->labelImage1->installEventFilter(this);
    ui->labelImage2->installEventFilter(this);

    statusBar()->showMessage("Ready — double-click the image panels to upload images");
}

MainWindow::~MainWindow() { delete ui; }

// ─────────────────────────────────────────────────────────────────────────────
bool MainWindow::eventFilter(QObject* obj, QEvent* ev)
{
    if (ev->type() == QEvent::MouseButtonDblClick) {
        if (obj == ui->labelImage1) { loadImage(1); return true; }
        if (obj == ui->labelImage2) { loadImage(2); return true; }
    }
    return QMainWindow::eventFilter(obj, ev);
}

void MainWindow::keyPressEvent(QKeyEvent* ev)
{
    if (ev->key() == Qt::Key_Delete) { clearImages(); return; }
    QMainWindow::keyPressEvent(ev);
}

// Re-scale every displayed image whenever the window is resized
void MainWindow::resizeEvent(QResizeEvent* ev)
{
    QMainWindow::resizeEvent(ev);
    reflowImages();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Quality rating — dynamic ratio when 'total' is provided
// ─────────────────────────────────────────────────────────────────────────────
static QString getQuality(int count, int total = 0)
{
    if (total > 0) {
        double r = (double)count / total;
        if (r >= 0.75) return "High";
        if (r >= 0.40) return "Medium";
        return "Low";
    }
    if (count >= 150) return "High";
    if (count >= 50)  return "Medium";
    return "Low";
}

// ─────────────────────────────────────────────────────────────────────────────
//  showInLabel  — scale img to fit lbl's actual current pixel size
//  reflowImages — re-apply all stored images (called on resize)
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::showInLabel(const QImage& img, QLabel* lbl)
{
    if (img.isNull()) return;
    // lbl->width()/height() are the real allocated pixels after layout
    int w = lbl->width()  - 4;
    int h = lbl->height() - 4;
    if (w < 10) w = 400;   // safety fallback before first paint
    if (h < 10) h = 300;
    lbl->setPixmap(
        QPixmap::fromImage(img).scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::reflowImages()
{
    if (!displayImage1.isNull())      showInLabel(displayImage1,      ui->labelImage1);
    if (!displayImage2.isNull())      showInLabel(displayImage2,      ui->labelImage2);
    if (!displayMatchResult.isNull()) showInLabel(displayMatchResult,  ui->labelMatchResult);
}

// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::loadImage(int idx)
{
    QString path = QFileDialog::getOpenFileName(
        this, tr("Open Image %1").arg(idx), QString(),
        tr("Images (*.png *.jpg *.jpeg *.bmp *.tiff *.tif *.pgm *.ppm);;All files (*)"));
    if (path.isEmpty()) return;

    QImage img(path);
    if (img.isNull()) {
        QMessageBox::warning(this, tr("Load Error"),
            tr("Could not read the image.\nSupported: PNG, JPG, BMP, TIFF, PGM, PPM."));
        return;
    }

    if (idx == 1) {
        currentImage1 = img;
        displayImage1 = img;
        showInLabel(displayImage1, ui->labelImage1);
    } else {
        currentImage2 = img;
        displayImage2 = img;
        showInLabel(displayImage2, ui->labelImage2);
    }

    ui->removeButton->setEnabled(true);
    ui->runMatchButton->setEnabled(!currentImage1.isNull() && !currentImage2.isNull());

    displayMatchResult = QImage();
    ui->labelMatchResult->clear();
    ui->labelMatchResult->setText("Run a detector or match features to see the result here");
    statusBar()->showMessage(QString("Loaded Image %1: %2").arg(idx).arg(path));
}

void MainWindow::clearImages()
{
    currentImage1 = currentImage2 = QImage();
    displayImage1 = displayImage2 = displayMatchResult = QImage();

    ui->labelImage1->clear();
    ui->labelImage1->setText("Double-click to upload Image 1");
    ui->labelImage2->clear();
    ui->labelImage2->setText("Double-click to upload Image 2");
    ui->labelMatchResult->clear();
    ui->labelMatchResult->setText("Run a detector or match features to see the result here");
    ui->filePathLabel->setText("No images loaded");
    ui->labelCornerCount->setText("Features detected: —");
    ui->labelTime->setText("Computation time: —");
    ui->removeButton->setEnabled(false);
    ui->runMatchButton->setEnabled(false);
    statusBar()->showMessage("Images cleared");
}

// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::on_removeButton_clicked() { clearImages(); }

void MainWindow::on_runHarrisButton_clicked()
{
    runCornerDetector(false); // false = Standard Harris
}

void MainWindow::runLambdaMinusButton_clicked()
{
    runCornerDetector(true);  // true = Lambda-Minus (λ-)
}

void MainWindow::runCornerDetector(bool useLambdaMinus)
{
    if (currentImage1.isNull() && currentImage2.isNull()) {
        QMessageBox::information(this, "Missing Image", "Please upload at least one image.");
        // Uncheck the button if no image is loaded
        if (detectorGroup->checkedButton()) {
            detectorGroup->setExclusive(false);
            detectorGroup->checkedButton()->setChecked(false);
            detectorGroup->setExclusive(true);
        }
        return;
    }

    if (useLambdaMinus) {
        statusBar()->showMessage("Running Lambda-Minus (λ-) corner detection…");
    } else {
        statusBar()->showMessage("Running Harris corner detection…");
    }
    QApplication::processEvents();

    double k  = ui->spinK->value();
    int block = ui->spinBlockSize->value();
    double th = ui->spinThreshold->value();

    int total = 0;
    QString cText = "Corners: ", tText = "Time: ";

    if (!currentImage1.isNull()) {
        auto r = backend.runHarris(currentImage1, k, block, th, useLambdaMinus);
        displayImage1 = r.outputImage;
        showInLabel(displayImage1, ui->labelImage1);
        total += r.cornerCount;
        cText += QString("Img1: %1 (%2)   ").arg(r.cornerCount).arg(getQuality(r.cornerCount));
        tText += QString("Img1: %1 ms   ").arg(r.timeMs, 0, 'f', 1);
    }

    if (!currentImage2.isNull()) {
        auto r = backend.runHarris(currentImage2, k, block, th, useLambdaMinus);
        displayImage2 = r.outputImage;
        showInLabel(displayImage2, ui->labelImage2);
        total += r.cornerCount;
        cText += QString("Img2: %1 (%2)").arg(r.cornerCount).arg(getQuality(r.cornerCount));
        tText += QString("Img2: %1 ms").arg(r.timeMs, 0, 'f', 1);
    }

    ui->labelCornerCount->setText(cText);
    ui->labelTime->setText(tText);

    if (useLambdaMinus) {
        statusBar()->showMessage(QString("Lambda-Minus done — %1 total corners.").arg(total));
    } else {
        statusBar()->showMessage(QString("Harris done — %1 total corners.").arg(total));
    }
}


void MainWindow::on_runSiftButton_clicked()
{
    if (currentImage1.isNull() && currentImage2.isNull()) {
        QMessageBox::information(this, "Missing Image", "Please upload at least one image.");
        return;
    }
    statusBar()->showMessage("Running SIFT feature extraction…");
    QApplication::processEvents();

    int total = 0;
    QString fText = "Features: ", tText = "Time: ";

    if (!currentImage1.isNull()) {
        SiftResult r  = backend.runSift(currentImage1);
        displayImage1 = r.outputImage;
        showInLabel(displayImage1, ui->labelImage1);
        total += r.keypointCount;
        fText += QString("Img1: %1 (%2)   ").arg(r.keypointCount).arg(getQuality(r.keypointCount));
        tText += QString("Img1: %1 ms   ").arg(r.timeMs, 0, 'f', 1);
    }
    if (!currentImage2.isNull()) {
        SiftResult r  = backend.runSift(currentImage2);
        displayImage2 = r.outputImage;
        showInLabel(displayImage2, ui->labelImage2);
        total += r.keypointCount;
        fText += QString("Img2: %1 (%2)").arg(r.keypointCount).arg(getQuality(r.keypointCount));
        tText += QString("Img2: %1 ms").arg(r.timeMs, 0, 'f', 1);
    }

    ui->labelCornerCount->setText(fText);
    ui->labelTime->setText(tText);
    statusBar()->showMessage(QString("SIFT done — %1 total features.").arg(total));
}

void MainWindow::on_runMatchButton_clicked()
{
    if (currentImage1.isNull() || currentImage2.isNull()) return;

    ui->runMatchButton->setEnabled(false);
    ui->runMatchButton->setText("Matching...");
    statusBar()->showMessage("Matching features between Image 1 and Image 2...");
    QApplication::processEvents();

    MatchMethod method = (ui->comboMatchMethod->currentText() == "SSD") ? MatchMethod::SSD : MatchMethod::NCC;

    // We use a looser ratio threshold for SSD to get more visual lines,
    // or we can just pass the default.
    MatchResult res = backend.matchFeatures(currentImage1, currentImage2, method, 0.8);

    displayMatchResult = res.matchImage;
    showInLabel(displayMatchResult, ui->labelMatchResult);

    QString qm = getQuality(res.matchCount, res.totalSourceFeatures);
    ui->labelCornerCount->setText(
        QString("Features Matched: %1 / %2 (%3)").arg(res.matchCount).arg(res.totalSourceFeatures).arg(qm));
    ui->labelTime->setText(QString("Matching time: %1 ms").arg(res.timeMs, 0, 'f', 2));

    ui->runMatchButton->setText("Match Features");
    ui->runMatchButton->setEnabled(true);

    statusBar()->showMessage(QString("Matching done — %1 / %2 matches using %3")
        .arg(res.matchCount).arg(res.totalSourceFeatures)
        .arg(ui->comboMatchMethod->currentText()));
}