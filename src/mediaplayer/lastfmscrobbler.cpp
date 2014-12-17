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
    QUrlQuery query = makeRequest(requestVariables);
    QNetworkRequest authRequest = QNetworkRequest(QUrl("https://ws.audioscrobbler.com/2.0/"));
    authRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    qDebug() << "authenticate query: " << query.toString(QUrl::FullyEncoded);
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
        if ( mLastArtist != "" && mLastTitle != "" && (mStatus->getPlaying() == 1)) {
            qDebug() << "New track: " << mTimeTrackStarted << mStatus->getTitle() << mStatus->getArtist() << mStatus->getAlbum();
            sendNowPlaying();
        }
    }
    mElapsed = mStatus->getElapsed();
    if ( mStatus->getPlaying() == 0 ) {
        // reset values
        mLastAlbum = "";
        mLastArtist = "";
        mLastTitle = "";
        mElapsed = 0;
        qDebug() << "Playing state changed, reset values";
    }


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
        QUrlQuery query = makeRequest(requestVariables);
        QNetworkRequest authRequest = QNetworkRequest(QUrl("https://ws.audioscrobbler.com/2.0/"));
        authRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
        qDebug() << "now playing query: " << query.toString(QUrl::FullyEncoded);
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
    QUrlQuery query = makeRequest(requestVariables);
    QNetworkRequest authRequest = QNetworkRequest(QUrl("https://ws.audioscrobbler.com/2.0/"));
    authRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    qDebug() << "scrobble track query: " << query.toString(QUrl::FullyEncoded).toUtf8();
    mNetAccess->post(authRequest,query.toString(QUrl::FullyEncoded).toUtf8());
}

QUrlQuery LastFMScrobbler::makeRequest(QMap<QString, QString> variables)
{
    QString request;
    QString signatureInput = "";
    QUrlQuery query;
    QMap<QString, QString>::iterator i;
    for ( i = variables.begin(); i != variables.end(); ++i) {
        qDebug() << "key: " << i.key() << " value: " << i.value();
        query.addQueryItem(i.key(),i.value());
        signatureInput += i.key() + i.value();
    }
    qDebug() << "query to string: " << query.toString(QUrl::FullyEncoded);
    QByteArray sigInputRaw;
    sigInputRaw.append(signatureInput + LASTFMSECRET);
    qDebug() << "signing: " << sigInputRaw;
    QString signature = QCryptographicHash::hash(sigInputRaw,QCryptographicHash::Md5).toHex();
    query.addQueryItem("api_sig",signature);
    qDebug() << "query(signed): " << query.toString(QUrl::FullyEncoded);
    return query;
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
