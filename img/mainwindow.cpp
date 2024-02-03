#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QImage>
#include <QMessageBox>
#include <QPixmap>
#include <QShortcut>
#include "choosepage.h"
#include "videoplayer.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1352,711);
    setWindowTitle("图像处理");

    //图标设置
    ui->btn_turnright->setIcon(QIcon::fromTheme(":/img/turn-right.png"));
    ui->btn_turnleft->setIcon(QIcon::fromTheme(":/img/turn-left.png"));
    ui->btn_mirror->setIcon(QIcon::fromTheme(":/img/mirror.png"));
    ui->btn_turntop->setIcon(QIcon::fromTheme(":/img/cuizhi.png"));

    //停靠窗口不可移动
    ui->dockWidget_color->setFeatures(QDockWidget::NoDockWidgetFeatures);
    ui->dockWidget_default->setFeatures(QDockWidget::NoDockWidgetFeatures);
    ui->dockWidget_edge->setFeatures(QDockWidget::NoDockWidgetFeatures);

    ui->stackedWidget->setCurrentIndex(0);

    connect(ui->btn_videochange,&QPushButton::clicked,this,[=](){
        VideoPlayer*videopage = new VideoPlayer(this);
        QEventLoop loop;
        connect(videopage, &QObject::destroyed, &loop, &QEventLoop::quit);

        videopage->show();
        this->close();
        this->deleteLater();

        loop.exec();
    });
    connect(ui->btn_imgchange,&QPushButton::clicked,this,[=](){
        ui->stackedWidget->setCurrentIndex(0);
        ui->statusbar->showMessage("欢迎进入图片处理界面");
    });

    //工具栏
    toolbar = new QToolBar(this);
    addToolBar(toolbar);
    QAction*actionopen = new QAction("打开",this);
    actionopen->setIcon(QIcon::fromTheme(":/img/open.png"));
    QAction*actionsave = new QAction("保存",this);
    actionsave->setIcon(QIcon::fromTheme(":/img/save.png"));
    QAction*actionVideo = new QAction("打开视频处理",this);
    actionVideo->setIcon(QIcon::fromTheme(":/img/video.png"));
    QAction*actionImg = new QAction("打开图片处理",this);
    actionImg->setIcon(QIcon::fromTheme(":/img/picture.png"));

    toolbar->addAction(actionopen);
    toolbar->addAction(actionsave);
    toolbar->addAction(actionVideo);
    toolbar->addAction(actionImg);
    connect(actionopen, &QAction::triggered, this, &MainWindow::Openpic);
    connect(actionsave, &QAction::triggered, this, &MainWindow::Savepic);
    connect(actionVideo, &QAction::triggered, this,[=](){
        ui->stackedWidget->setCurrentIndex(1);
        ui->statusbar->showMessage("欢迎进入视频处理界面");
    });
    connect(actionImg, &QAction::triggered, this,[=](){
        ui->stackedWidget->setCurrentIndex(0);
        ui->statusbar->showMessage("欢迎进入图片处理界面");
    });

    //原图显示
    connect(ui->btn_oldpic,&QPushButton::clicked,this,&MainWindow::Oldpic);

    //恢复默认
    connect(ui->btn_default,&QPushButton::clicked,this,[=](){
        ui->slider_red->setValue(0);
        ui->slider_blue->setValue(0);
        ui->slider_green->setValue(0);

        ui->slider_sharpness->setValue(-20);
        ui->slider_Contrast->setValue(0);
        ui->slider_binaryzation->setValue(30);
        ui->slider_filter->setValue(0);
        ui->slider_Saturation->setValue(-100);
        ui->slider_detection->setValue(0);
        ui->slider_light->setValue(-100);
        ui->slider_gamma->setValue(0);
        ui->slider_gray->setValue(1);

        bool flag = Oldpic();
        if(flag){
            srcImg.release();
            ui->label->clear();

            ui->slider_gray->setDisabled(true);
            ui->slider_Contrast->setDisabled(true);
            ui->slider_Saturation->setDisabled(true);
            ui->slider_detection->setDisabled(true);
            ui->slider_filter->setDisabled(true);
            ui->slider_gamma->setDisabled(true);
            ui->slider_detection->setDisabled(true);
            ui->slider_light->setDisabled(true);
            ui->slider_sharpness->setDisabled(true);
            ui->slider_red->setDisabled(true);
            ui->slider_green->setDisabled(true);
            ui->slider_blue->setDisabled(true);
            ui->slider_binaryzation->setDisabled(true);
            ui->statusbar->showMessage("恢复默认成功");
        }else{
            ui->statusbar->showMessage("恢复默认失败");
        }
    });

    //打开图片
    QShortcut *openimg = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_O), this);
    connect(openimg, &QShortcut::activated, this, &MainWindow::Openpic);
    connect(ui->btn_openpic,&QPushButton::clicked,this,&MainWindow::Openpic);
    connect(ui->actionopen,&QAction::triggered,this,&MainWindow::Openpic);

    //保存图片
    QShortcut *saveShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this);
    connect(saveShortcut, &QShortcut::activated, this, &MainWindow::Savepic);
    connect(ui->btn_savepic,&QPushButton::clicked,this,&MainWindow::Savepic);
    connect(ui->actionsave,&QAction::triggered,this,&MainWindow::Savepic);

    //灰度控制
    connect(ui->slider_gray,&QSlider::valueChanged,this,&MainWindow::Grayscale);
    ui->slider_gray->setMinimum(1);
    ui->slider_gray->setMaximum(100);
    ui->slider_gray->setDisabled(true);

    //右旋转图片
    QShortcut *turnright = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Right), this);
    connect(turnright, &QShortcut::activated, this, &MainWindow::RightRotation);
    connect(ui->btn_turnright,&QPushButton::clicked,this,&MainWindow::RightRotation);

    //左旋转图片
    QShortcut *turnleft = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Left), this);
    connect(turnleft, &QShortcut::activated, this, &MainWindow::LeftRotation);
    connect(ui->btn_turnleft,&QPushButton::clicked,this,&MainWindow::LeftRotation);

    //上翻转图片
    QShortcut *turntop = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up), this);
    connect(turntop, &QShortcut::activated, this, &MainWindow::TopRotation);
    connect(ui->btn_turntop,&QPushButton::clicked,this,&MainWindow::TopRotation);

    //镜像翻转
    QShortcut *mirror = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down), this);
    connect(mirror, &QShortcut::activated, this, &MainWindow::MirrorRotation);
    connect(ui->btn_mirror,&QPushButton::clicked,this,&MainWindow::MirrorRotation);

    //亮度
    connect(ui->slider_light,&QSlider::valueChanged,this,&MainWindow::AddLight);
    ui->slider_light->setMinimum(-100);
    ui->slider_light->setMaximum(100);
    ui->slider_light->setValue(-100);
    ui->slider_light->setDisabled(true);

    //边缘检测
    connect(ui->slider_detection,&QSlider::valueChanged,this,&MainWindow::EdgeDetection);
    ui->slider_detection->setDisabled(true);

    //饱和度
    connect(ui->slider_Saturation,&QSlider::valueChanged,this,&MainWindow::AddSaturation);
    ui->slider_Saturation->setMinimum(-100);
    ui->slider_Saturation->setMaximum(100);
    ui->slider_Saturation->setValue(-100);
    ui->slider_Saturation->setDisabled(true);

    //均值滤波
    connect(ui->slider_filter,&QSlider::valueChanged,this,&MainWindow::MeanFilterSizeChanged);
    ui->slider_filter->setDisabled(true);

    //二值化
    connect(ui->slider_binaryzation,&QSlider::valueChanged,this,&MainWindow::Binarize);
    ui->slider_binaryzation->setMinimum(30);
    ui->slider_binaryzation->setMaximum(200);
    ui->slider_binaryzation->setValue(30);
    ui->slider_binaryzation->setDisabled(true);

    //对比度
    connect(ui->slider_Contrast,&QSlider::valueChanged,this,&MainWindow::AddContrast);
    ui->slider_Contrast->setMaximum(255);
    ui->slider_Contrast->setValue(0);
    ui->slider_Contrast->setDisabled(true);

    //锐化
    connect(ui->slider_sharpness,&QSlider::valueChanged,this,&MainWindow::SharpenImage);
    ui->slider_sharpness->setMaximum(180);
    ui->slider_sharpness->setMinimum(-20);
    ui->slider_sharpness->setValue(-20);
    ui->slider_sharpness->setDisabled(true);

    //伽马
    connect(ui->slider_gamma,&QSlider::valueChanged,this,&MainWindow::AddGamma);
    ui->slider_gamma->setDisabled(true);

    //R通道
    connect(ui->slider_red,&QSlider::valueChanged,this,&MainWindow::RedValue);
    ui->slider_red->setMaximum(255);
    ui->slider_red->setMinimum(-255);
    ui->slider_red->setValue(0);
    ui->slider_red->setDisabled(true);

    //G通道
    connect(ui->slider_green,&QSlider::valueChanged,this,&MainWindow::GreenValue);
    ui->slider_green->setMaximum(255);
    ui->slider_green->setMinimum(-255);
    ui->slider_green->setValue(0);
    ui->slider_green->setDisabled(true);

    //B通道
    connect(ui->slider_blue,&QSlider::valueChanged,this,&MainWindow::BlueValue);
    ui->slider_blue->setMaximum(255);
    ui->slider_blue->setMinimum(-255);
    ui->slider_blue->setValue(0);
    ui->slider_blue->setDisabled(true);


}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape){
        ChoosePage*page = new ChoosePage(this);
        this->close();
        this->deleteLater();
        page->exec();
    }

}
bool MainWindow::Oldpic()
{
    if (!srcImg.empty()) {
        if (!currentImagePath.isEmpty()) {
            srcImg = imread(currentImagePath.toStdString());
            QImage originalImage(srcImg.data, srcImg.cols, srcImg.rows, srcImg.step, QImage::Format_BGR888);
            QSize sz = ui->label->size();
            ui->label->setPixmap(QPixmap::fromImage(originalImage).scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            ui->statusbar->showMessage("显示原图成功");
            return true;
        } else {
            QMessageBox::warning(this, "No Image", "未找到原始图片路径");
            ui->statusbar->showMessage("显示原图失败");
            return false;
        }
    } else {
        QMessageBox::warning(this, "No Image", "请选择一张图片");
        ui->statusbar->showMessage("显示原图失败");
        return false;
    }
    return false;
}

void MainWindow::Openpic()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open Img"),"E:\\pic","Images(*.png *.jpg *.bmp)");
    if(!filename.isEmpty()){
        currentImagePath = filename;
        srcImg = imread(filename.toStdString());
        if(!srcImg.empty()){
            QImage imge(srcImg.data,srcImg.cols,srcImg.rows,srcImg.step,QImage::Format_BGR888);

            //ui->label->setPixmap(QPixmap::fromImage(imge));
            //ui->label->setScaledContents(true);//按照比例进行缩放
            QSize sz = ui->label->size();
            ui->label->setPixmap(QPixmap::fromImage(imge).scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            //ui->label_change->setPixmap(QPixmap::fromImage(imge).scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            ui->statusbar->showMessage("图片路径: " + filename);

            ui->slider_gray->setDisabled(false);
            ui->slider_Contrast->setDisabled(false);
            ui->slider_Saturation->setDisabled(false);
            ui->slider_detection->setDisabled(false);
            ui->slider_filter->setDisabled(false);
            ui->slider_gamma->setDisabled(false);
            ui->slider_detection->setDisabled(false);
            ui->slider_light->setDisabled(false);
            ui->slider_sharpness->setDisabled(false);
            ui->slider_red->setDisabled(false);
            ui->slider_green->setDisabled(false);
            ui->slider_blue->setDisabled(false);
            ui->slider_binaryzation->setDisabled(false);
        }else{
            QMessageBox::critical(this,"Error","打开图片失败...");
        }
    }else{
        //QMessageBox::information(this,"Error","打开图片失败...");

    }
}

void MainWindow::Savepic()
{
    if(srcImg.empty()){
        QMessageBox::warning(this, "Warning", "当前没有图片可保存");
        return;
    }
    //applyAllMod();
    QString savePath = QFileDialog::getSaveFileName(this,tr("保存图片"),"","Images(*.png *.jpg *.bmp)");
    if(!savePath.isEmpty()){
        QPixmap labelPixmap = ui->label->pixmap(Qt::ReturnByValue);
        QImage saveImage = labelPixmap.toImage().copy();
        if(saveImage.save(savePath)){
            //imwrite(savePath.toStdString(),srcImg);
            ui->statusbar->showMessage("图片保存成功：" + savePath);
        }else{
            QMessageBox::warning(this,"save img","保存图片失败");
        }
    }else{
        //QMessageBox::information(this,"Error","未选择图片...");
    }
}

void MainWindow::Grayscale(int value)
{
    if(!srcImg.empty()){

        Mat adImgValue;
        Mat grayImag;
        cvtColor(srcImg,grayImag,COLOR_BGR2GRAY);
        convertScaleAbs(grayImag,adImgValue,1.0,value);

        QImage graysImg(adImgValue.data, adImgValue.cols, adImgValue.rows, adImgValue.step, QImage::Format_Grayscale8);
        //ui->label->setPixmap(QPixmap::fromImage(graysImg));

        QSize sz = ui->label->size();
        ui->label->setPixmap(QPixmap::fromImage(graysImg).scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        ui->statusbar->showMessage("灰度值: " + QString::number(value));
    }else{

    }
}

void MainWindow::RightRotation()
{
    if(!srcImg.empty()){
        transpose(srcImg,srcImg);
        flip(srcImg,srcImg,0);

        QImage Rotation(srcImg.data, srcImg.cols, srcImg.rows, srcImg.step, QImage::Format_BGR888);
        ui->label->setPixmap(QPixmap::fromImage(Rotation));

        QSize sz = ui->label->size();
        ui->label->setPixmap(QPixmap::fromImage(Rotation).scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        ui->statusbar->showMessage("顺时针旋转了90°");

    }else{
        QMessageBox::warning(this,"no img","请选择一张图片");
    }
}

void MainWindow::LeftRotation()
{
    if(!srcImg.empty()){
        transpose(srcImg,srcImg);
        flip(srcImg,srcImg,1);

        QImage Rotation(srcImg.data, srcImg.cols, srcImg.rows, srcImg.step, QImage::Format_BGR888);
        ui->label->setPixmap(QPixmap::fromImage(Rotation));

        QSize sz = ui->label->size();
        ui->label->setPixmap(QPixmap::fromImage(Rotation).scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        ui->statusbar->showMessage("逆时针旋转了90°");

    }else{
        QMessageBox::warning(this,"no img","请选择一张图片");
    }
}

void MainWindow::TopRotation()
{
    if(!srcImg.empty()){
        flip(srcImg,srcImg,0);

        QImage Rotation(srcImg.data, srcImg.cols, srcImg.rows, srcImg.step, QImage::Format_BGR888);
        ui->label->setPixmap(QPixmap::fromImage(Rotation));

        QSize sz = ui->label->size();
        ui->label->setPixmap(QPixmap::fromImage(Rotation).scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        ui->statusbar->showMessage("垂直翻转");

    }else{
        QMessageBox::warning(this,"no img","请选择一张图片");
    }
}

void MainWindow::MirrorRotation()
{
    if(!srcImg.empty()){
        flip(srcImg,srcImg,1);

        QImage Rotation(srcImg.data, srcImg.cols, srcImg.rows, srcImg.step, QImage::Format_BGR888);
        ui->label->setPixmap(QPixmap::fromImage(Rotation));

        QSize sz = ui->label->size();
        ui->label->setPixmap(QPixmap::fromImage(Rotation).scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        ui->statusbar->showMessage("镜像翻转");

    }else{
        QMessageBox::warning(this,"no img","请选择一张图片");
    }
}

void MainWindow::AddLight(int value)
{
    if(!srcImg.empty()){
        Mat addlight = srcImg + Scalar(value,value,value);

        QImage brightimg(addlight.data,addlight.cols,addlight.rows,addlight.step,QImage::Format_BGR888);
        QSize sz = ui->label->size();
        ui->label->setPixmap(QPixmap::fromImage(brightimg).scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        ui->statusbar->showMessage("亮度调整为: " + QString::number(value));
    }else{

    }
}

void MainWindow::EdgeDetection(int value)
{
    if(!srcImg.empty()){
        Mat grayImg;
        cvtColor(srcImg,grayImg,COLOR_BGRA2GRAY);

        Mat edge;
        Canny(grayImg,edge,value,value * 2,3);

        QImage edgeImage(edge.data, edge.cols, edge.rows, edge.step, QImage::Format_Grayscale8);
        QSize sz = ui->label->size();
        ui->label->setPixmap(QPixmap::fromImage(edgeImage).scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        ui->statusbar->showMessage("边缘检测调整为: " + QString::number(value));

    }
}

void MainWindow::AddSaturation(int value)
{
    Q_UNUSED(value);
}
void MainWindow::MeanFilterSizeChanged(int value)
{
    if(!srcImg.empty()){
        if(value <= 0){
            return;
        }
        Mat meanFiltered;
        cv::Size kernelSize(value,value);
        cv::blur(srcImg,meanFiltered,kernelSize);

        QImage meanFilteredImage(meanFiltered.data, meanFiltered.cols, meanFiltered.rows, meanFiltered.step, QImage::Format_BGR888);
        QSize sz = ui->label->size();
        ui->label->setPixmap(QPixmap::fromImage(meanFilteredImage).scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        ui->statusbar->showMessage("均值滤波大小调整为: " + QString::number(value));
    }
}

void MainWindow::Binarize(int value)
{
    if(!srcImg.empty()){
        Mat grayImg;
        cvtColor(srcImg, grayImg, cv::COLOR_BGR2GRAY);

        Mat binaryImg;
        threshold(grayImg, binaryImg, value, 255, cv::THRESH_BINARY);

        QImage binaryImage(binaryImg.data, binaryImg.cols, binaryImg.rows, binaryImg.step, QImage::Format_Grayscale8);
        QSize sz = ui->label->size();
        ui->label->setPixmap(QPixmap::fromImage(binaryImage).scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        ui->statusbar->showMessage("二值化阈值调整为: " + QString::number(value));
    }
}

void MainWindow::AddContrast(int value)
{
    if(!srcImg.empty()){
        double alpha = (100 + value) / 100.0;

        Mat contrastImg;
        srcImg.convertTo(contrastImg, -1, alpha, 0);

        QImage contrastImage(contrastImg.data, contrastImg.cols, contrastImg.rows, contrastImg.step, QImage::Format_BGR888);
        QSize sz = ui->label->size();
        ui->label->setPixmap(QPixmap::fromImage(contrastImage).scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        ui->statusbar->showMessage("对比度调整为: " + QString::number(value));

    }
}

void MainWindow::sharpenImage(const Mat &inputImage, Mat &outputImage,float sharpnessFactor)
{
    Mat kernel = (Mat_<float>(3,3) << -1, -1, -1, -1, 9 + sharpnessFactor, -1, -1, -1, -1);

    filter2D(inputImage, outputImage, inputImage.depth(), kernel);

    // 将结果限制在0到255之间
    normalize(outputImage, outputImage, 0, 255, cv::NORM_MINMAX);
}

QImage MainWindow::MatToQImage(const Mat &mat)
{
    if (mat.empty()) {
        return QImage();
    }

    if (mat.type() == CV_8UC1) {
        return QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_Grayscale8);
    } else if (mat.type() == CV_8UC3) {
        cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
        return QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_RGB888);
    }

    return QImage();
}

void MainWindow::SharpenImage(int value)
{
    if (!srcImg.empty()) {
        cv::Mat sharpenedImg;
        float sharpnessFactor = static_cast<float>(value) / 50.0;
        sharpenImage(srcImg, sharpenedImg, sharpnessFactor);

        QImage sharpenedImage = MatToQImage(sharpenedImg);

        QSize sz = ui->label->size();
        ui->label->setPixmap(QPixmap::fromImage(sharpenedImage).scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        ui->statusbar->showMessage("图像已锐化，强度: " + QString::number(value));
    } else {
        //QMessageBox::warning(this,"no img","请选择一张图片");
    }
}

void MainWindow::AddGamma(int value)
{
    float gammavlue = static_cast<float>(value) / 20.0;
    GammaChange(gammavlue);
}

void MainWindow::RedValue(int value)
{
    if(!srcImg.empty()){
        std::vector<Mat>channels;
        split(srcImg,channels);

        channels[2] += value;

        merge(channels,srcImg);

        QImage adjustedImg(srcImg.data, srcImg.cols, srcImg.rows, srcImg.step, QImage::Format_BGR888);
        QSize sz = ui->label->size();
        ui->label->setPixmap(QPixmap::fromImage(adjustedImg).scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        ui->statusbar->showMessage("红色通道调整为: " + QString::number(value));
    }
}

void MainWindow::GreenValue(int value)
{
    if(!srcImg.empty()){
        // 分离通道
        std::vector<cv::Mat> channels;
        split(srcImg, channels);

        // 调整绿色通道
        channels[1] += value;

        // 合并通道
        merge(channels, srcImg);

        QImage adjustedImg(srcImg.data, srcImg.cols, srcImg.rows, srcImg.step, QImage::Format_BGR888);
        QSize sz = ui->label->size();
        ui->label->setPixmap(QPixmap::fromImage(adjustedImg).scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        ui->statusbar->showMessage("绿色通道调整为: " + QString::number(value));
    }
}

void MainWindow::BlueValue(int value)
{
    if(!srcImg.empty()){
        // 分离通道
        std::vector<cv::Mat> channels;
        split(srcImg, channels);

        // 调整绿色通道
        channels[0] += value;

        // 合并通道
        merge(channels, srcImg);

        QImage adjustedImg(srcImg.data, srcImg.cols, srcImg.rows, srcImg.step, QImage::Format_BGR888);
        QSize sz = ui->label->size();
        ui->label->setPixmap(QPixmap::fromImage(adjustedImg).scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        ui->statusbar->showMessage("蓝色通道调整为: " + QString::number(value));
    }
}

void MainWindow::GammaChange(float value)
{
    if (!srcImg.empty()) {
        // 将输入图像转换为浮点型
        Mat srcFloat;
        srcImg.convertTo(srcFloat, CV_32F, 1.0 / 255.0);

        // 计算伽马校正
        Mat gammaCorrectedImg;
        cv::pow(srcFloat, value, gammaCorrectedImg);

        // 将结果限制在0到1之间
        normalize(gammaCorrectedImg, gammaCorrectedImg, 0, 1, cv::NORM_MINMAX);

        // 将浮点型图像转换为8位无符号整数图像
        gammaCorrectedImg *= 255;
        gammaCorrectedImg.convertTo(gammaCorrectedImg, CV_8U);

        QImage gammaCorrectedImage = MatToQImage(gammaCorrectedImg);

        QSize sz = ui->label->size();
        ui->label->setPixmap(QPixmap::fromImage(gammaCorrectedImage).scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));

        ui->statusbar->showMessage("伽马校正，伽马值: " + QString::number(value));
    } else {
        QMessageBox::warning(this, "no img", "请选择一张图片");
    }
}
