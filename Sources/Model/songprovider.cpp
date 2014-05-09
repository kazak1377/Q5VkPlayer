#include "songprovider.h"
#include <assert.h>
#include <QDir>
#include "Sources/Controller/Network/networker.h"

SongProvider *SongProvider::__sharedProvider = NULL;

SongProvider::SongProvider(QObject *parent) : QObject(parent)
{
    updateFromLocalCopy();
    connect(NetWorker::sharedNetworker(), SIGNAL(didRecieveSongs(QList<Song *>)), this, SLOT(updateSongList(QList<Song *>)));
}

SongProvider *SongProvider::sharedProvider()
{
    if (NULL == SongProvider::__sharedProvider)
    {
        SongProvider::__sharedProvider = new SongProvider();
    }
    return SongProvider::__sharedProvider;
}

void SongProvider::updateSongList(QList<Song *> songs)
{
    foreach (Song *song, songs)
    {
        int indexOfLocal = -1;
        if ((indexOfLocal = localSongs.indexOf(song->fullName()+".mp3")) != -1)
        {
            QUrl localUrl = QUrl ("file://" + musicFolderPath() + localSongs.at(indexOfLocal));
            song->url = localUrl;
            song->local = true;
            qDebug() << localUrl;
        }
    }
    this->songs = songs;
    emit this->songListDidUpdated();
}

Song *SongProvider::songWithIndex(quint16 index)
{
    assert(index < this->songs.count());
    return this->songs.at(index);
}

quint16 SongProvider::songsCount()
{
    return this->songs.count();
}

QList<Song *> SongProvider::songsWithTitleContains(QString searchPart)
{
    QList<Song *> matches;

    foreach (Song *song, this->songs)
    {
        if (song->title.contains(searchPart,Qt::CaseInsensitive) || song->artist.contains(searchPart,Qt::CaseInsensitive))
        {
            matches.append(song);
        }
    }

    return matches;
}

QString SongProvider::musicFolderPath()
{
    QString musicDirectory = QDir::homePath() + SLASH + "Music" + SLASH;
#ifdef Q_OS_OSX
//    musicDirectory += "iTunes/iTunes Media/Automatically Add to iTunes/";
#endif
    return musicDirectory;
}

void SongProvider::updateFromLocalCopy()
{
    QDir dir(musicFolderPath());
    localSongs = dir.entryList();
    qDebug() <<localSongs;
}

SongProvider::~SongProvider()
{
//    delete this->songs;
}
