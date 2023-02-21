#ifndef QPLAYLIST_H
#define QPLAYLIST_H

#include <QObject>
#include <QUrl>
#include <QList>
#include <QRandomGenerator>

class QPlaylist: public QObject
{
    Q_OBJECT
public:
    explicit QPlaylist(QObject *parent = nullptr);

    int mediaCount() const;
    QUrl media(int index) const;
    void addMedia(const QUrl &media);
    void insertMedia(int index, const QUrl &media);
    void removeMedia(int index);
    void clear();
    void shuffle();
    bool hasNext(int index) const;
    QList<QUrl>::const_iterator begin() const;
    QList<QUrl>::const_iterator end() const;


private:
    QList<QUrl> mediaList;
};

#endif // QPLAYLIST_H
