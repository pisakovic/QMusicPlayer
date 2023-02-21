#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , player(QMediaPlayer())
    , audioOutput(QAudioOutput())
    , currMediaIndex(0)
    , playlist(QPlaylist())
{
    ui->setupUi(this);

    player.setAudioOutput(&audioOutput);

    connect(&player, &QMediaPlayer::positionChanged, this, &MainWindow::on_positionChanged);
    connect(&player, &QMediaPlayer::durationChanged, this, &MainWindow::on_durationChanged);
    connect(&player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(onMediaStatusChanged(QMediaPlayer::MediaStatus)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_volumeBar_sliderMoved(int position)
{
    qreal linearVolume = QAudio::convertVolume(position/ qreal(100.0), QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);
    audioOutput.setVolume(linearVolume);
}

void MainWindow::on_playBtn_clicked()
{
    if(player.playbackState() == QMediaPlayer::PausedState)
    {
        player.setPosition(curr_position);
        //qDebug()<<QString::number(curr_position);
        player.play();
    }
    else if(player.mediaStatus() == QMediaPlayer::NoMedia)
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Choose a track to play!");
        messageBox.setFixedSize(500,200);
    }
    else
    {
        player.play();
    }
}

void MainWindow::on_stopBtn_clicked()
{
    player.stop();
    ui->progLabel->setText("0:00");
}

void MainWindow::on_chooseFile_clicked()
{
    QStringList fileName = QFileDialog::getOpenFileNames(this, tr("Open File"),"/home/petar/Music",tr("Mp3 Files (*.mp3)"));

    for(auto &f: fileName)
    {
        if(f.length() > 0)
        {
        playlist.insertMedia(ui->playlistWidget->count(),QUrl::fromLocalFile(f));
        QFileInfo fi(f);
        ui->playlistWidget->insertItem(ui->playlistWidget->count(),fi.fileName());
        }
    }
}

void MainWindow::on_progressBar_sliderMoved(int position)
{
    player.setPosition(position);
}

void MainWindow::on_positionChanged(int position)
{
    ui->progressBar->setValue(position);
    QString time_curr = ms_to_time(player.position());
    ui->progLabel->setText(time_curr);
}

void MainWindow::on_durationChanged(int position)
{
    ui->progressBar->setMaximum(position);
    QString time = ms_to_time(player.duration());
    ui->progMaxLabel->setText(time);
}

void MainWindow::on_volumeBar_valueChanged(int value)
{
    ui->volLabel->setText(QString::number(value));
}


void MainWindow::on_pauseBtn_clicked()
{
    player.pause();
    curr_position = player.position();
    qDebug()<<QString::number(curr_position);

}

void MainWindow::on_playlistWidget_itemDoubleClicked(QListWidgetItem *item)
{
    player.stop();
    currMediaIndex = ui->playlistWidget->row(item);
    player.setSource(playlist.media(currMediaIndex));
    player.play();
}

void MainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    QPixmap pixmp = *new QPixmap();
    QString title = *new QString();

    if (status == QMediaPlayer::BufferedMedia){

        pixmp = QPixmap::fromImage(GetMetaDataImg(&player));
        pixmp = pixmp.scaled(ui->imgLabel->size(),Qt::KeepAspectRatio);
        ui->imgLabel->setPixmap(pixmp);

        title = GetMetaDataName(&player);
        ui->mediaName->setText(title);
    }
}

QImage MainWindow::GetMetaDataImg(QMediaPlayer *player)
{

    QMediaMetaData pMetaData = player->metaData();
    QVariant var_coverArt = pMetaData[QMediaMetaData::CoverArtImage];
    QImage coverArt = var_coverArt.value<QImage>();
    return coverArt;

}

QString MainWindow::GetMetaDataName(QMediaPlayer *player)
{

    QMediaMetaData pMetaData = player->metaData();
    QVariant var_title = pMetaData[QMediaMetaData::Title];
    QVariant var_artist = pMetaData[QMediaMetaData::AlbumArtist];
    QString track_title = var_artist.value<QString>() + " - " + var_title.value<QString>();
    return track_title;

}

QString MainWindow::ms_to_time(long long duration)
{
    QString min = *new QString();
    QString sec = *new QString();

    long m = duration / 60000;
    duration = duration - 60000*m;
    long s = duration / 1000;

    min = QString::number(m);

    if(s < 10)
    {
        sec = "0" + QString::number(s);
    }
    else
    {
        sec = QString::number(s);
    }

    QString time = min + ":" + sec;

    return time;
}

void MainWindow::on_nextBtn_clicked()
{
    player.stop();

    if((playlist.mediaCount() - 1) <= currMediaIndex)
    {
        player.setSource(playlist.media(0));
        currMediaIndex = 0;
        player.play();
    }
    else
    {
        currMediaIndex += 1;
        player.setSource(playlist.media(currMediaIndex));
        player.play();
    }

}


void MainWindow::on_prevBtn_clicked()
{
    player.stop();

    if(currMediaIndex == 0)
    {
        currMediaIndex = playlist.mediaCount()-1;
        player.setSource(playlist.media(currMediaIndex));
        player.play();
    }
    else
    {
        currMediaIndex -= 1;
        player.setSource(playlist.media(currMediaIndex));
        player.play();
    }
}


void MainWindow::on_shuffleBtn_clicked()
{
    if(playlist.mediaCount() > 1){
        ui->playlistWidget->clear();
        playlist.shuffle();
        //optimizuj bolje siso
        for(auto &f: playlist)
        {
            QFileInfo fi(f.fileName());
            ui->playlistWidget->insertItem(ui->playlistWidget->count(),fi.fileName());
        }
    }
}



void MainWindow::on_pushButton_clicked()
{
    auto selectedItems = ui->playlistWidget->selectedItems();
    for(auto &item: selectedItems)
    {
        playlist.removeMedia(ui->playlistWidget->row(item));
        //uzima se i brise item iz widgeta
        delete ui->playlistWidget->takeItem(ui->playlistWidget->row(item));
    }
}

