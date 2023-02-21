#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QDebug>
#include <QAudioOutput>
#include <chrono>
#include <QMediaMetaData>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QRandomGenerator>
#include <QThread>
#include "qlistwidget.h"
#include "qplaylist.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_volumeBar_sliderMoved(int position);

    void on_playBtn_clicked();

    void on_stopBtn_clicked();

    void on_progressBar_sliderMoved(int position);

    void on_positionChanged(int position);

    void on_durationChanged(int position);

    QString ms_to_time(long long duration);

    void on_volumeBar_valueChanged(int value);

    void on_pauseBtn_clicked();

    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

    QImage GetMetaDataImg(QMediaPlayer *player);

    QString GetMetaDataName(QMediaPlayer *player);

    void on_chooseFile_clicked();

    void on_playlistWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_nextBtn_clicked();

    void on_prevBtn_clicked();

    void on_shuffleBtn_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QMediaPlayer player;
    QAudioOutput audioOutput;
    qreal linearVolume;
    int currMediaIndex;
    int curr_position;
    QPlaylist playlist;
};
#endif // MAINWINDOW_H
