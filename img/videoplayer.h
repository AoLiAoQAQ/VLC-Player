#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QMainWindow>
#include <vlc/vlc.h>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QThread>
namespace Ui {
class VideoPlayer;
}

class VideoPlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit VideoPlayer(QWidget *parent = nullptr);
    ~VideoPlayer();

    void MediaPlayerSetDrawbleWindow(libvlc_media_player_t*player);
    void MediaPlayerPlay();

    QString formatTime(int seconds);
public:
    void keyPressEvent(QKeyEvent*event)override;
    //void mousePressEvent(QMouseEvent *event)override;
public slots:
    void PlayVideo();
    void StopVideo();

    void ProgressChange(int value);
    void VolumeChange(int value);

    void OpenActionFile();
    void updateStartTime();
    void SpeedChange(int index);
private slots:
    void on_btn_open_clicked();

private:
    Ui::VideoPlayer *ui;

    libvlc_instance_t*m_instence=nullptr;
    libvlc_media_player_t*m_vlcPlayer=nullptr;

    int m_currentVolume;
    int m_totalDuration;

    QTimer m_timer;

    bool returnflag = false;
    bool m_isplaying = false;
};

#endif // VIDEOPLAYER_H
