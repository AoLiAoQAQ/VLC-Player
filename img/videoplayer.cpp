 #include "videoplayer.h"
#include "ui_videoplayer.h"
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>

#include "choosepage.h"

VideoPlayer::VideoPlayer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VideoPlayer)
{
    ui->setupUi(this);

    setWindowTitle("视频处理");

    //设置图标
    // QPixmap icon(":/img/file.png");
    // QSize scaleSize(25,25);
    // QPixmap scaledIcon = icon.scaled(scaleSize, Qt::KeepAspectRatio);
    //ui->label_file->setPixmap(scaledIcon);
    ui->btn_open->setIcon(QIcon::fromTheme(":/img/file.png"));
    ui->btn_play->setIcon(QIcon::fromTheme(":/img/play.png"));
    ui->btn_stop->setIcon(QIcon::fromTheme(":/img/stop.png"));


    ui->actionhelp2->setVisible(true);
    connect(ui->actionhelp2,&QAction::triggered,this,[=](){
        QMessageBox::information(this, "介绍", "按下ESC返回界面，CTRL+O打开文件,F1打开此对话框。"
                                               "该软件是一个基于VLC的视频播放和OPENCV简单图像处理的软件");
    });



    //初始化
    m_instence = libvlc_new(0,nullptr);
    if(m_instence == nullptr){
        qDebug()<<"Instence null";
    }else{
        qDebug()<<"ok...";
    }

    //按钮
    connect(ui->btn_play,&QPushButton::clicked,this,&VideoPlayer::PlayVideo);
    connect(ui->btn_stop,&QPushButton::clicked,this,&VideoPlayer::StopVideo);

    //Slider滑动条
    connect(ui->Slider_progress,&QSlider::valueChanged,this,&VideoPlayer::ProgressChange);

    connect(ui->Slider_volume,&QSlider::valueChanged,this,&VideoPlayer::VolumeChange);
    ui->Slider_volume->setValue(100);

    //快捷键
    connect(ui->actionOpen,&QAction::triggered,this,&VideoPlayer::OpenActionFile);

    connect(ui->comboBox_speed,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&VideoPlayer::SpeedChange);
    ui->comboBox_speed->setCurrentIndex(2);


}

VideoPlayer::~VideoPlayer()
{
    if(m_instence!=NULL){
        libvlc_release(m_instence);
        m_instence = nullptr;
    }

    delete ui;
}

void VideoPlayer::OpenActionFile()
{
    if(m_vlcPlayer != nullptr){
        StopVideo();
    }

    QString filepath = QFileDialog::getOpenFileName(this,tr("Open Video"),"E:\\pic",tr("Video Files(*.avi *.mp4 *.mkv *.mp3 *.wma)"));
    if(filepath.isEmpty()){
        qDebug()<<"path null";
        return;
    }

    filepath = QDir::toNativeSeparators(filepath);
    ui->statusbar->showMessage("该文件路径为：" + filepath);
    //qDebug()<<filepath;

    //获取视频文件
    libvlc_media_t*media = libvlc_media_new_path(m_instence,filepath.toUtf8().data());
    libvlc_media_parse(media);

    //获取时长
    m_totalDuration = static_cast<int>(libvlc_media_get_duration(media) / 1000);

    //ui->Slider_progress->setMaximum(m_totalDuration);

    if (m_vlcPlayer != nullptr) {
        libvlc_media_player_stop(m_vlcPlayer);
        libvlc_media_player_release(m_vlcPlayer);
        m_vlcPlayer=nullptr;
    }

    m_vlcPlayer = libvlc_media_player_new_from_media(media);
    libvlc_media_release(media);

    //窗口句柄
    MediaPlayerSetDrawbleWindow(m_vlcPlayer);

    //播放
    MediaPlayerPlay();

    ui->label_endTime->setText(formatTime(m_totalDuration));
}

void VideoPlayer::updateStartTime()
{
    if (m_vlcPlayer != nullptr) {
        float position = libvlc_media_player_get_position(m_vlcPlayer);
        position = qBound(0.0f, position, 1.0f);
        int currentTime = static_cast<int>(position * m_totalDuration);


        ui->label_startTime->setText(formatTime(currentTime));
        ui->Slider_progress->setValue(static_cast<int>(position * ui->Slider_progress->maximum()));
        // if (currentTime % 5 == 0) {
        //     ui->Slider_progress->setValue(static_cast<int>(position * ui->Slider_progress->maximum()));
        // }
    }
}

void VideoPlayer::SpeedChange(int index)
{
    if (m_vlcPlayer != nullptr) {
        float speed = 1.0;
        switch (index) {
        case 0: speed = 0.5; break;
        case 1: speed = 0.75; break;
        case 2: speed = 1.0; break;
        case 3: speed = 1.25; break;
        case 4: speed = 1.5; break;
        case 5: speed = 1.75; break;
        case 6: speed = 2.0; break;

        default:
            break;
        }

        libvlc_media_player_set_rate(m_vlcPlayer, speed);
    }
}



void VideoPlayer::MediaPlayerSetDrawbleWindow(libvlc_media_player_t *player)
{
    libvlc_media_player_set_hwnd(player,reinterpret_cast<void*>(ui->video_widget->winId()));
}

void VideoPlayer::MediaPlayerPlay()
{
    if(m_vlcPlayer == nullptr){
        qDebug()<<"m_vlcPlayer is null";
        return;
    }

    connect(&m_timer, &QTimer::timeout, this, &VideoPlayer::updateStartTime);
    m_timer.start(1000);

    libvlc_media_player_play(m_vlcPlayer);
    m_isplaying = true;
}

QString VideoPlayer::formatTime(int seconds)
{
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int remainingSeconds = seconds % 60;

    return QString("%1:%2:%3")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(remainingSeconds, 2, 10, QChar('0'));
}

void VideoPlayer::keyPressEvent(QKeyEvent *event)
{
    returnflag = true;
    if(event->key() == Qt::Key_Escape){
        StopVideo();
        ChoosePage*page = new ChoosePage(this);
        this->close();
        this->deleteLater();
        page->exec();
    }
    returnflag = false;
}

void VideoPlayer::PlayVideo()
{
    if(m_vlcPlayer == nullptr){
        QMessageBox::warning(this,"无视频","请选择一个视频");
        return;
    }
    if(m_isplaying){
        libvlc_media_player_pause(m_vlcPlayer);
        //ui->btn_play->setText("暂停");
        ui->btn_play->setIcon(QIcon::fromTheme(":/img/pause.png"));
    }else{
        libvlc_media_player_play(m_vlcPlayer);
        //ui->btn_play->setText("播放");
        ui->btn_play->setIcon(QIcon::fromTheme(":/img/play.png"));
    }
    m_isplaying = !m_isplaying;
}

void VideoPlayer::StopVideo()
{
    if (m_vlcPlayer != nullptr) {
        libvlc_media_player_stop(m_vlcPlayer);

        libvlc_media_player_release(m_vlcPlayer);
        m_vlcPlayer = nullptr;
    }else{
        if(returnflag){
            qDebug()<<"返回选择界面...";
            return;
        }else{
            QMessageBox::warning(this,"没有视频","请选择一个视频");
        }
    }
    ui->btn_stop->setIcon(QIcon::fromTheme(":/img/stop.png"));
    ui->label_startTime->setText("00:00:00");
    ui->label_endTime->setText("00:00:00");
    ui->Slider_progress->setValue(0);
    m_isplaying = false;
}

void VideoPlayer::ProgressChange(int value)
{
    if (m_vlcPlayer != nullptr) {
        float position = static_cast<float>(value) / ui->Slider_progress->maximum();
        libvlc_media_player_set_position(m_vlcPlayer, position);
    }
}

void VideoPlayer::VolumeChange(int value)
{
    if(m_vlcPlayer != nullptr){
        int volume= static_cast<int>(static_cast<float>(value) / ui->Slider_volume->maximum() * 100);
        libvlc_audio_set_volume(m_vlcPlayer, volume);

        m_currentVolume = volume;
        ui->statusbar->showMessage("音量为: " + QString::number(volume));
    }
}

void VideoPlayer::on_btn_open_clicked()
{
    OpenActionFile();
}
