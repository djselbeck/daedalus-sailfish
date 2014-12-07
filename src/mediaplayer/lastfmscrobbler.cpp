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

    mLastAlbum = "";
    mLastArtist = "";
    mLastTitle = "";
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
        // See last.fm documentation: http://www.lastfm.de/api/scrobbling
        // Scrobble last track if any
        if ( (mElapsed >= mDuration/2 || mElapsed >= 4*60) &&
             (mLastArtist != "" && mLastTitle != "") ) {
            scrobbleTrack();
//            mTrackScrobbled = true;
        }
        mTimeTrackStarted = QString::number(QDateTime::currentDateTime().toTime_t());
//        mTrackScrobbled = false;
        mLastAlbum = album;
        mLastArtist = artist;
        mLastTitle = title;
        mTrackNR = QString::number(mStatus->getTrackNr());
        mDuration = mStatus->getLength();
        if ( mLastArtist != "" && mLastTitle != "") {
            qDebug() << "New track: " << mTimeTrackStarted << mStatus->getTitle() << mStatus->getArtist() << mStatus->getAlbum();
            sendNowPlaying();
        }
    }
    mElapsed = mStatus->getElapsed();



}

void LastFMScrobbler::sendNowPlaying()
{
        QMap<QString, QString> requestVariables;
        requestVariables["method"] = "track.updateNowPlaying";
        requestVariables["api_key"] = LASTFMAPIKEY;
        requestVariables["artist"] = mLastArtist;
        requestVariables["album"] = mLastAlbum;
        requestVariables["track"] = mLastTitle;
        requestVariables["trackNumber"] = mTrackNR;
        requestVariables["duration"] = QString::number(mDuration);
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
    qDebug() << "Scrobble track: " << mLastTitle << mLastArtist << mLastAlbum;
    QMap<QString, QString> requestVariables;
    requestVariables["method"] = "track.scrobble";
    requestVariables["api_key"] = LASTFMAPIKEY;
    requestVariables["artist"] = mLastArtist;
    requestVariables["album"] = mLastAlbum;
    requestVariables["timestamp"] = mTimeTrackStarted;
    requestVariables["track"] = mLastTitle;
    requestVariables["trackNumber"] = mTrackNR;
    requestVariables["duration"] = QString::number(mDuration);
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
        QString key = i.key();
        key = key.replace('&'," ");
        key = key.replace('?'," ");
        key = key.replace(':'," ");
//        key = key.replace(' ',"%20");
        QString value = i.value();
        value = value.replace('&'," ");
        value = value.replace('?'," ");
        value = value.replace(':'," ");
//        value = value.replace(' '," ");
        if( i == variables.begin() ) {
            request += "" + key + "=" + value;
        } else {
            request += "&" + key + "=" + value;
        }
        signatureInput += key + value;
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
