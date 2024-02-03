#include "choosepage.h"
#include "ui_choosepage.h"
#include "mainwindow.h"
#include "videoplayer.h"
#include <QPixmap>
ChoosePage::ChoosePage(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChoosePage)
{
    ui->setupUi(this);

    connect(ui->btn_ImgChange,&QPushButton::clicked,this,[=](){
        MainWindow*picWindow = new MainWindow(this);
        QEventLoop loop;
        connect(picWindow, &QObject::destroyed, &loop, &QEventLoop::quit);

        picWindow->show();
        this->close();
        this->deleteLater();

        loop.exec();
    });
    connect(ui->btn_VideoChange,&QPushButton::clicked,this,[=](){
        VideoPlayer*vdp = new VideoPlayer(this);
         QEventLoop loop;
        connect(vdp, &QObject::destroyed, &loop, &QEventLoop::quit);

        vdp->show();
        this->close();
        this->deleteLater();

        loop.exec();
    });

    setFixedSize(800,500);
    setWindowTitle("选择界面");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QPixmap originalPixmap(":/img/GGboned.jpg");

    QSize targetSize(800, 500);

    QPixmap scaledPixmap = originalPixmap.scaled(targetSize, Qt::KeepAspectRatio);

    ui->label_ggboned->setPixmap(scaledPixmap);
}

ChoosePage::~ChoosePage()
{
    delete ui;
}
