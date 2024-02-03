#include "mainwindow.h"
#include "choosepage.h"
#include <QApplication>
#include <QFile>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //qDebug()<< av_version_info();

    QString m_qss;
    QFile fileQss(":/QssStyle/MacOS.qss");
    fileQss.open(QFile::ReadOnly);
    m_qss = fileQss.readAll();
    a.setStyleSheet(m_qss);
    fileQss.close();
    // MainWindow w;
    // w.show();
    ChoosePage c;
    c.show();
    return a.exec();
}
