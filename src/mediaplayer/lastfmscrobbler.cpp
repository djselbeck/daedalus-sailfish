#include "lastfmscrobbler.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QUrlQuery>
#include <QXmlStreamReader>
#include <QDateTime>

LastFMScrobbler::LastFMScrobbler(PlaybackStatusObject *status, QObject *parent) :
    QObject(parent)
{
    mStatus = status;
    // connect signals
    if ( mStatus != 0 ) {
        connect(mStatus,SIGNAL(valuesChanged()),this,SLOT(trackChanged()));
    }
    mNetAccess = new QNetworkAccessManager(this);
    connect(mNetAccess,SIGNAL(finished(QNetworkReply*)),this,SLOT(parseReply(QNetworkReply*)));
}

void LastFMScrobbler::authenticate(QString username, QString password)
{
    QMap<QString, QString> requestVariables;
    requestVariables["method"] = "auth.getMobileSession";
    requestVariables["api_key"] = LASTFMAPIKEY;
    requestVariables["username"] = username;
    requestVariables["password"] = password;
    QString request = makeRequest(requestVariables);
    qDebug() << "Auth request is: " << request;
    QNetworkRequest authRequest = QNetworkRequest(QUrl("https://ws.audioscrobbler.com/2.0/"));
    authRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    QUrlQuery query(request);
    qDebug() << "query: " << query.toString(QUrl::FullyEncoded).toUtf8();
    mNetAccess->post(authRequest,query.toString(QUrl::FullyEncoded).toUtf8());
}

void LastFMScrobbler::authenticate(QVariant userData)
{
    // extract user name and pasword
    QStringList dataStrings = userData.toStringList();
    authenticate(dataStrings[0],dataStrings[1]);
}

void LastFMScrobbler::trackChanged()
{
    QString album = mStatus->getAlbum();
    QString artist = mStatus->getArtist();
    QString title = mStatus->getTitle();
    if ( mLastAlbum != album || mLastArtist != artist || mLastTitle != title ) {
        mTimeTrackStarted = QString::number(QDateTime::currentDateTime().toTime_t());
        qDebug() << "New track: " << mTimeTrackStarted << mStatus->getTitle() << mStatus->getArtist() << mStatus->getAlbum();
        mTrackScrobbled = false;
        sendNowPlaying();
        mLastAlbum = album;
        mLastArtist = artist;
        mLastTitle = title;
    }

    // See last.fm documentation: http://www.lastfm.de/api/scrobbling
    if ( !mTrackScrobbled && (mStatus->getElapsed() >= mStatus->getLength()/2 || mStatus->getElapsed() >= 4*60) ) {
        scrobbleTrack();
        mTrackScrobbled = true;
    }

}

void LastFMScrobbler::sendNowPlaying()
{
        QMap<QString, QString> requestVariables;
        requestVariables["method"] = "track.updateNowPlaying";
        requestVariables["api_key"] = LASTFMAPIKEY;
        requestVariables["artist"] = mStatus->getArtist();
        requestVariables["album"] = mStatus->getAlbum();
        requestVariables["track"] = mStatus->getTitle();
        requestVariables["trackNumber"] = QString::number(mStatus->getTrackNr());
        requestVariables["duration"] = QString::number(mStatus->getLength());
        requestVariables["sk"] = mSessionKey;
        QString request = makeRequest(requestVariables);
        qDebug() << "Auth request is: " << request;
        QNetworkRequest authRequest = QNetworkRequest(QUrl("https://ws.audioscrobbler.com/2.0/"));
        authRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
        QUrlQuery query(request);
        qDebug() << "query: " << query.toString(QUrl::FullyEncoded).toUtf8();
        mNetAccess->post(authRequest,query.toString(QUrl::FullyEncoded).toUtf8());
}

void LastFMScrobbler::scrobbleTrack()
{
    qDebug() << "Scrobble track: " << mStatus->getTitle() << mStatus->getArtist() << mStatus->getAlbum();
    QMap<QString, QString> requestVariables;
    requestVariables["method"] = "track.scrobble";
    requestVariables["api_key"] = LASTFMAPIKEY;
    requestVariables["artist"] = mStatus->getArtist();
    requestVariables["album"] = mStatus->getAlbum();
    requestVariables["timestamp"] = mTimeTrackStarted;
    requestVariables["track"] = mStatus->getTitle();
    requestVariables["trackNumber"] = QString::number(mStatus->getTrackNr());
    requestVariables["duration"] = QString::number(mStatus->getLength());
    requestVariables["sk"] = mSessionKey;
    QString request = makeRequest(requestVariables);
    qDebug() << "Auth request is: " << request;
    QNetworkRequest authRequest = QNetworkRequest(QUrl("https://ws.audioscrobbler.com/2.0/"));
    authRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    QUrlQuery query(request);
    qDebug() << "query: " << query.toString(QUrl::FullyEncoded).toUtf8();
    mNetAccess->post(authRequest,query.toString(QUrl::FullyEncoded).toUtf8());
}

QString LastFMScrobbler::makeRequest(QMap<QString, QString> variables)
{
    QString request;
    QString signatureInput;
    QMap<QString, QString>::iterator i;
    for ( i = variables.begin(); i != variables.end(); ++i) {
        qDebug() << "key: " << i.key() << " value: " << i.value();
        if( i == variables.begin() ) {
            request += "" + i.key() + "=" + i.value();
        } else {
            request += "&" + i.key() + "=" + i.value();
        }
        signatureInput += i.key() + i.value();
    }
    QByteArray sigInputRaw;
    sigInputRaw.append(signatureInput + LASTFMSECRET);
    QString signature = QCryptographicHash::hash(sigInputRaw,QCryptographicHash::Md5).toHex();
    request += "&api_sig=" + signature;

    return request;
}

void LastFMScrobbler::parseReply(QNetworkReply* reply)
{
    QByteArray replyData = reply->readAll();
    qDebug() << replyData;
    QXmlStreamReader xmlReader(replyData);

    while ( !xmlReader.atEnd()) {
        QXmlStreamReader::TokenType token = xmlReader.readNext();
        //qDebug() << "xml name: " << xmlReader.name();
        if ( token == QXmlStreamReader::StartDocument ) {
            continue;
        }
        else if ( token == QXmlStreamReader::StartElement ) {
            // Found data
            if ( xmlReader.name() == "lfm" ) {
                // last.fm start check result type
                // FIXME
                QXmlStreamAttributes attributes = xmlReader.attributes();
                if ( attributes.hasAttribute("status") ) {
                    qDebug() << "LFM status: " << attributes.value("status").toString();
                }
            } else if ( xmlReader.name() == "session") {
                parseSession(xmlReader);
            }
        }
    }
    qDebug() << "finished response parsing";
}

void LastFMScrobbler::parseSession(QXmlStreamReader &xmlReader)
{
    while ( !(xmlReader.tokenType() == QXmlStreamReader::EndElement && xmlReader.name() == "session") ) {
        if ( (xmlReader.tokenType() == QXmlStreamReader::StartElement) && xmlReader.name() == "key" ) {
            QString key = xmlReader.readElementText();
            mSessionKey = key;
            emit newLastFMSessionKey(key);
        }
        xmlReader.readNext();
    }
}


void LastFMScrobbler::setSessionKey(QString key)
{
    qDebug() << "got session key: " << key;
    mSessionKey = key;
}
