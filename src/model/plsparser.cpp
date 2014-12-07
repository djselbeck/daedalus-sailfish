#include "plsparser.h"

#include <QFile>
#include <QTextStream>

PLSParser::PLSParser(QObject *parent) :
    QObject(parent)
{
    mPLFile = 0;
    mList = new QList<TrackObject>();
    mTrackURLs = new QList<QUrl>();
}

QList<QUrl>* PLSParser::parsePlaylist(QUrl filename)
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
    bool foundPl = false;
    QTextStream inputStream(mPLFile);
    QString inputLine;
    while ( !inputStream.atEnd() && !foundPl ) {
        inputLine = inputStream.readLine();
        if ( inputLine == "[playlist]" ) {
            foundPl = true;
        }
    }
    if ( !foundPl ) {
        // File is completly read but no [playlist] was found, abort
        return 0;
    }
    // Search for file paths
    while ( !inputStream.atEnd()) {
        inputLine = inputStream.readLine();
        if ( inputLine.startsWith("File") ) {
            // Found an path
            QStringList subStrings = inputLine.split('=');
            QString filePath = subStrings.at(1);
            filePath = filePath.replace("file://","");
            qDebug() << "Found track path: " << filePath;
            if ( QFile(filePath).exists() ) {
                qDebug() << "File exists";
                mTrackURLs->append(QUrl::fromLocalFile(filePath));
            } else {
                // Try path prefix from playlist path
                QString playlistPrefix = filename.toLocalFile();
                QStringList pathComponents = playlistPrefix.split('/');
                playlistPrefix = "";
                for ( int i = 0; i < pathComponents.size() - 1 ; i++ ) {
                    playlistPrefix += pathComponents.at(i) + '/';
                }
                filePath = playlistPrefix + filePath;
                qDebug() << "trying with prefix: " << filePath;
                if ( QFile(filePath).exists()) {
                    qDebug() << "File exists";
                    mTrackURLs->append(QUrl::fromLocalFile(filePath));
                }
            }
        }
    }
    return mTrackURLs;
}
