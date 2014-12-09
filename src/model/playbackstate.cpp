#include "playbackstate.h"

#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QThread>

PlaybackState::PlaybackState(QObject *parent) :
    QObject(parent)
{
    QStringList drivers = QSqlDatabase::drivers();
    if ( !drivers.contains("QSQLITE") ) {
    } else {
        // Check if database exists already otherwise create on
        QDir dirAccess;
        if ( !dirAccess.mkpath(QStandardPaths::writableLocation(QStandardPaths::DataLocation))) {
            return;
        }
        mDBFilePath = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/playbackstate.sqlite3";
        mDB = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE","Playlist"));
        mDB->setDatabaseName(mDBFilePath);
        if ( !mDB->open() ) {
            qDebug() << "Playbackstate db not opened";
        } else {
            qDebug() << "DB opened";
        }
        createTables();
    }
}


bool PlaybackState::createTables()
{
    if ( mDB->open() ) {
        // Check if database contains necessary tables
        if ( !mDB->tables().contains("lasttracks") ) {
            QSqlQuery createQuery(*mDB);
            if (createQuery.exec("CREATE TABLE lasttracks"
                                 "(id integer default 0 ,"
                                 "title text default '',"
                                 "album text default '',"
                                 "artist text default '',"
                                 "url text default '',"
                                 "urn text default '',"
                                 "length integer default 0,"
                                 "tracknr integer default 0,"
                                 "discnr integer default 0"
                                 " )") ) {
                qDebug() << "Created tables";
            } else {
                qDebug() << "problem creating tables";
            }
        }
    }
}

void PlaybackState::saveCurrentPlaylist(QList<TrackObject *> *list)
{
    if ( list == 0) {
        return;
    }
    // Delete old table

    if ( !mDB->isOpen() ) {
        return;
    }

    QSqlQuery query(*mDB);
    if ( !query.exec("delete from lasttracks") ) {
        qDebug() << "Couldn't remove old tracks";
        return;
    }

    if ( mDB->transaction() ) {
        qDebug() << "Sql transaction begin";
    }
    query.prepare("INSERT INTO lasttracks (id, title, album, artist, url, urn, length, tracknr, discnr) VALUES (:id, :title, :album, :artist, :url, :urn, :length, :tracknr, :discnr)");
    unsigned int index = 0;
    foreach ( TrackObject *track, *list ) {
        qDebug() << "Track inserting: " << track->getTitle();
        query.bindValue(":id",index++);
        query.bindValue(":title",track->getTitle());
        query.bindValue(":album",track->getAlbum());
        query.bindValue(":artist",track->getArtist());
        query.bindValue(":url",track->getURL());
        query.bindValue(":urn",track->getURN());
        query.bindValue(":length",track->getLength());
        query.bindValue(":tracknr",track->getTrackNr());
        query.bindValue(":discnr",track->getDiscNr());
//        qDebug() << "Last query: " << query.lastQuery();
        if ( query.exec()) {
            qDebug() << "Track inserted: " << track->getTitle();
        } else {
            qDebug() << "Error: " << query.lastError().text();
        }
    }
    if ( mDB->commit() ) {
        qDebug() << "Sql commit succeded";
    }
    if ( !query.exec("vacuum") ) {
        qDebug() << "Couldn't vacuum old tracks";
        return;
    }
    emit workDone();
    thread()->exit(0);
}

void PlaybackState::resumeLastPlaylist()
{
    if ( !mDB->isOpen() ) {
        return;
    }
    qDebug() << "Resuming last playlist";
    QSqlQuery query(*mDB);
    query.prepare("SELECT * FROM lasttracks");
    QList<TrackObject*> *trackList = new QList<TrackObject*>();
    QString title, album, artist;
    QUrl url,urn;
    unsigned int length,tracknr,discnr;

    query.exec();
    while ( query.next() ) {
        title = query.value("title").toString();
        album = query.value("album").toString();
        artist = query.value("artist").toString();
        url = query.value("url").toUrl();
        urn = query.value("urn").toUrl();
        length = query.value("length").toUInt();
        tracknr = query.value("tracknr").toUInt();
        discnr = query.value("discnr").toUInt();
        TrackObject *track = new TrackObject(title,artist,album,url.toEncoded(),urn,length,tracknr,discnr,0);
        trackList->append(track);
        qDebug() << "Resuming last playlist track: " << url.toEncoded();
    }
    emit lastPlaylistReady(trackList);
}
