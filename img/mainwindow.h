#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QDebug>
#include <QToolBar>
#include <QTimer>
#include <QThread>
#include <QKeyEvent>

using namespace cv;


#include <vlc/vlc.h>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public:
    void sharpenImage(const Mat&inputImage,Mat&outputImage,float sharpnessFactor);
    QImage MatToQImage(const Mat&mat);
    void GammaChange(float value);
    void keyPressEvent(QKeyEvent *event);
public slots:
    bool Oldpic();
    void Openpic();
    void Savepic();
    void Grayscale(int value);
    void RightRotation();
    void LeftRotation();
    void TopRotation();
    void MirrorRotation();
    void AddLight(int value);
    void EdgeDetection(int value);
    void AddSaturation(int value);
    void MeanFilterSizeChanged(int value);
    void Binarize(int value);
    void AddContrast(int value);
    void SharpenImage(int value);
    void AddGamma(int value);

    void RedValue(int value);
    void GreenValue(int value);
    void BlueValue(int value);
private:
    Ui::MainWindow *ui;
    QToolBar*toolbar;
    Mat srcImg;
    QString currentImagePath;

    //Mat m_img;
};
#endif // MAINWINDOW_H
