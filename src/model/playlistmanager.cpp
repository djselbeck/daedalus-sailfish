#include "playlistmanager.h"

#include "plsparser.h"
#include "m3uparser.h"
#include <QDebug>

PlaylistManager::PlaylistManager(QObject *parent, QSparqlConnection *connection) :
    QObject(parent)
{
    if ( connection != 0  ) {
        mSparQLConnection = connection;
    }
    mTracksModel = 0;
    mThread = new QThread();
    mThread->start();
}


void PlaylistManager::requestPlaylist(QString url)
{
    qDebug() << "Playlist requested: " << url;
    emit sendBusy(true);
    // Check which ending
    if ( url.toLower().endsWith(".pls")  ) {
        QList<QUrl> *urls;

        qDebug() << "requested PL is an PLS list";
        PLSParser plsParser;
        urls = plsParser.parsePlaylist(url);
        mTracksModel = new SavedPlaylistTracksModel(0,urls,mSparQLConnection,mThread);
        mActiveFile = url;
        emit playlistTracksReady(mTracksModel);
    } else if ( url.toLower().endsWith(".m3u") ) {
        QList<QUrl> *urls;

        qDebug() << "requested PL is an M3U list";
        M3UParser m3uParser;
        urls = m3uParser.parsePlaylist(url);
        mTracksModel = new SavedPlaylistTracksModel(0,urls,mSparQLConnection,mThread);
        mActiveFile = url;
        emit playlistTracksReady(mTracksModel);
    }
}

void PlaylistManager::deleteActivePlaylist()
{
    deletePlaylist(mActiveFile);
}

void PlaylistManager::deletePlaylist(QString name)
{
    name = name.replace("file://","");
    qDebug() << "Removal of: " << name << " requested";
    if ( name.toLower().endsWith(".m3u") || name.toLower().endsWith(".pls") ) {
        QFile plFile(name);
        if ( plFile.exists() ) {
            if ( plFile.remove() ) {
                qDebug() << "Successfully removed";
            } else {
                qDebug() << "Error removing file: " << plFile.errorString();
            }

        }
    }
}
