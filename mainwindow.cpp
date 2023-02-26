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
    connect(&player, SIGNAL(playbackStateChanged(QMediaPlayer::PlaybackState)), this, SLOT(onPlaybackStateChanged(QMediaPlayer::PlaybackState)));

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

void MainWindow::onPlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    int prevMediaIndex = currMediaIndex;
    QString row = ui->playlistWidget->item(currMediaIndex)->text();
    row.append(QString::fromUtf8("           \u25B6"));

    if(state == QMediaPlayer::PlayingState)
    {
        ui->playBtn->setText("Pause");
        ui->playlistWidget->item(currMediaIndex)->setText(row);
    }
    else
    {
        ui->playlistWidget->item(prevMediaIndex)->setText(row.replace("           \u25B6",""));
        ui->playBtn->setText("Play");
    }


}

void MainWindow::on_playBtn_clicked()
{
    if(player.playbackState() == QMediaPlayer::PausedState)
    {
        player.setPosition(curr_position);
        //qDebug()<<QString::number(curr_position);
        player.play();
    }
    else if(player.mediaStatus() == QMediaPlayer::NoMedia && playlist.mediaCount()==0)
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Add a track to play!");
        messageBox.setFixedSize(500,200);
    }
    else if(player.mediaStatus() == QMediaPlayer::NoMedia && playlist.mediaCount()!=0)
    {
        player.setSource(playlist.media(0));
        player.play();
    }
    else if(player.playbackState() == QMediaPlayer::PlayingState)
    {
        player.pause();
        curr_position = player.position();
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
    //Problem with new Win10 update

    for(auto &f: fileName)
    {
        if(f.length() > 0)
        {
        playlist.insertMedia(ui->playlistWidget->count(),QUrl::fromLocalFile(f));
        TagLib::MPEG::File file(f.toStdString().c_str());
        TagLib::Tag *tag = file.tag();
        QString artist = QString::fromStdString(tag->artist().toCString());
        QString track = QString::fromStdString(tag->title().toCString());
        QString num = QString::number(ui->playlistWidget->count()+1);
        ui->playlistWidget->insertItem(ui->playlistWidget->count(),num + ". " + artist + " - " + track);
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
    QString time_curr = msToTime(player.position());
    ui->progLabel->setText(time_curr);
}

void MainWindow::on_durationChanged(int position)
{
    ui->progressBar->setMaximum(position);
    QString time = msToTime(player.duration());
    ui->progMaxLabel->setText(time);
}

void MainWindow::on_volumeBar_valueChanged(int value)
{
    ui->volLabel->setText(QString::number(value));
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
    QPixmap pixmp;
    QString title;

    if (status == QMediaPlayer::BufferedMedia){

        pixmp = QPixmap::fromImage(GetMetaDataImg(player));
        pixmp = pixmp.scaled(ui->imgLabel->size(),Qt::KeepAspectRatio);
        ui->imgLabel->setPixmap(pixmp);
        title = GetMetaDataName(player);
        ui->mediaName->setText(title);
    }
    else if(status == QMediaPlayer::EndOfMedia)
    {
        on_nextBtn_clicked();
    }


}

QImage MainWindow::GetMetaDataImg(QMediaPlayer &player)
{

    QMediaMetaData pMetaData = player.metaData();
    QVariant varCoverArt = pMetaData[QMediaMetaData::CoverArtImage];
    QImage coverArt = varCoverArt.value<QImage>();
    return coverArt;

}

QString MainWindow::GetMetaDataName(QMediaPlayer &player)
{

    QMediaMetaData pMetaData = player.metaData();
    QVariant varTitle = pMetaData[QMediaMetaData::Title];
    QVariant varArtist = pMetaData[QMediaMetaData::AlbumArtist];
    QString trackTitle = varArtist.value<QString>() + " - " + varTitle.value<QString>();
    return trackTitle;

}

QString MainWindow::msToTime(long long duration)
{
    QString min;
    QString sec;

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

    if((playlist.mediaCount() - 1) <= currMediaIndex && ui->radioButton->isChecked())
    {
        player.setSource(playlist.media(0));
        currMediaIndex = 0;
        player.play();
    }
    else if((playlist.mediaCount() - 1) <= currMediaIndex && !(ui->radioButton->isChecked()))
    {
        player.stop();
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
        player.stop();
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
    static const QRegularExpression re("\\d+\\. ");
    auto item = ui->playlistWidget->item(currMediaIndex);
    item->setText(item->text().replace("           \u25B6","").replace(re,""));
    QString itemTxt = item->text();

    if(playlist.mediaCount() > 1){

        playlist.shuffle();
        ui->playlistWidget->clear();
        for(auto f: playlist)
        {

            f.setScheme("");
            TagLib::MPEG::File file(f.toString().toStdString().c_str());
            TagLib::Tag *tag = file.tag();
            QString artist = QString::fromStdString(tag->artist().toCString());
            QString track = QString::fromStdString(tag->title().toCString());
            QString num = QString::number(ui->playlistWidget->count()+1);
            QString fullname = artist + " - " + track;

            if(fullname == itemTxt)
            {
               currMediaIndex = ui->playlistWidget->count();
            }

            ui->playlistWidget->insertItem(ui->playlistWidget->count(),num + ". " + fullname);
        }
    }
    emit player.playbackStateChanged(QMediaPlayer::PlayingState);
}

//Remove button
void MainWindow::on_pushButton_clicked()
{
    auto selectedItems = ui->playlistWidget->selectedItems();
    for(auto &item: selectedItems)
    {
        playlist.removeMedia(ui->playlistWidget->row(item));
        delete ui->playlistWidget->takeItem(ui->playlistWidget->row(item));
    }
}




