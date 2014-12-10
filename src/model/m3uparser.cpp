#include "m3uparser.h"

#include <QFile>
#include <QTextStream>

M3UParser::M3UParser(QObject *parent) :
    QObject(parent)
{
    mPLFile = 0;
    mList = new QList<TrackObject>();
    mTrackURLs = new QList<QUrl>();
}

QList<QUrl>* M3UParser::parsePlaylist(QUrl filename)
{
    if ( mPLFile != 0 ) {
        delete(mPLFile);
        mPLFile = 0;
    }
    mPLFile = new QFile(filename.toLocalFile());
    qDebug() << "opening playlist: " << mPLFile->fileName();
    if ( !mPLFile->open(QIODevice::ReadOnly)) {
        qDebug() << "Couldn't open playlist";
        return 0;
    }
    // search for beginning [playlist]
    QTextStream inputStream(mPLFile);
    QString inputLine;

    // Search for file paths
    while ( !inputStream.atEnd()) {
        inputLine = inputStream.readLine();
        if ( !inputLine.startsWith("#") ) {
            // Found an path
            QString filePath = inputLine;
            filePath = filePath.replace("file://","");
            qDebug() << "Found track path: " << filePath;
            if ( QFile(filePath).exists() ) {
                qDebug() << "File exists";
                mTrackURLs->append(QUrl::fromLocalFile(filePath));
            } else {
                // Try path prefix from playlist path
                QString playlistPrefix = filename.toLocalFile();
                QStringList pathComponents = playlistPrefix.split('/');
                QString prefixPath;
                playlistPrefix = "";
                for ( int i = 0; i < pathComponents.size() - 1 ; i++ ) {
                    playlistPrefix += pathComponents.at(i) + '/';
                }
                prefixPath = playlistPrefix + filePath;
                qDebug() << "trying with prefix: " << prefixPath;
                if ( QFile(prefixPath).exists()) {
                    qDebug() << "File exists";
                    mTrackURLs->append(QUrl::fromLocalFile(prefixPath));
                }
            }

            // check if it is an stream
            if ( filePath.toLower().startsWith("http") || filePath.toLower().startsWith("rtp") ) {
                mTrackURLs->append(QUrl(filePath));
            }
        }
    }
    return mTrackURLs;
}
