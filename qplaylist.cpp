#include "qplaylist.h"

#include <QObject>
#include <QUrl>
#include <QList>
#include <QRandomGenerator>

QPlaylist::QPlaylist(QObject *parent)
          :QObject(parent)
{

}

bool QPlaylist::hasNext(int index) const
{
    if((mediaList.count() - 2) <= index)
        return true;

    return false;
}

QList<QUrl>::const_iterator QPlaylist::begin() const
{
    return mediaList.begin();
}

QList<QUrl>::const_iterator QPlaylist::end() const
{
    return mediaList.end();
}


int QPlaylist::mediaCount() const
{
    return mediaList.count();
}

QUrl QPlaylist::media(int index) const
{
    return mediaList.at(index);
}

void QPlaylist::addMedia(const QUrl &media)
{
    mediaList.append(media);
}

void QPlaylist::insertMedia(int index,const QUrl &media)
{
    mediaList.insert(index,media);
}

void QPlaylist::removeMedia(int index)
{
    mediaList.removeAt(index);
}

void QPlaylist::shuffle()
{
    std::srand(std::time(NULL));

    for(int i = mediaList.size() - 1;i>0;i--)
    {
        int j = std::rand() % (i+1);

        mediaList.swapItemsAt(i,j);
    }
}


void QPlaylist::clear()
{
    mediaList.clear();
}
