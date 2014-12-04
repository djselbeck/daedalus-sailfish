#ifndef LASTFMSCROBBLER_H
#define LASTFMSCROBBLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QXmlStreamReader>

#include "../global.h"

#include "../model/playbackstatusobject.h"

class LastFMScrobbler : public QObject
{
    Q_OBJECT
public:
    explicit LastFMScrobbler(PlaybackStatusObject *status = 0, QObject *parent = 0);

    void authenticate(QString username, QString password);
    void setSessionKey(QString key);


signals:
    void newLastFMSessionKey(QString);

public slots:
    void authenticate(QVariant userData);

private slots:
    void trackChanged();
    void parseReply(QNetworkReply*);

private:
    void sendNowPlaying();
    void scrobbleTrack();
    QString makeRequest(QMap<QString,QString> variables);

    QNetworkAccessManager *mNetAccess;
    PlaybackStatusObject *mStatus;

    QString mSessionKey;
    QString mLastAlbum;
    QString mLastArtist;
    QString mLastTitle;
    QString mTimeTrackStarted;
    bool mTrackScrobbled;

    void parseSession(QXmlStreamReader &);




};

#endif // LASTFMSCROBBLER_H
