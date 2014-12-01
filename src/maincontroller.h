#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QQuickView>
#include <QQmlContext>
#include <QtQml>
#include <QQmlEngine>
#include <QThread>
#include <Qt5Sparql/QSparqlConnection>


// local includes
#include "model/albumsmodel.h"
#include "model/artistsmodel.h"
#include "model/albumtracksmodel.h"
#include "model/playbackstatusobject.h"
#include "mediaplayer/playlist.h"

class MainController : public QObject
{
    Q_OBJECT

    enum LastFMDownloadSizes {
        LASTFM_SMALL,
        LASTFM_MEDIUM,
        LASTFM_LARGE,
        LASTFM_EXTRALARGE,
        LASTFM_MEGA
    };


public:
    explicit MainController(QObject *parent = 0);
    MainController(QQuickView *viewer,QObject *parent = 0);

signals:
    // local cover database stuff
    void newDownloadSize(QString);
    void newDownloadEnabled(bool);

public slots:
    // request slos
    void requestAlbums();
    void requestArtists();
    void requestPlaylists();
    void requestFolder(QString path);

    void playlistsReady();
    void folderReady();

private:
    QString getLastFMArtSize(int index);

    // SparQL
    QSparqlConnection *mSparQLConnection;

    AlbumsModel *mAlbumsModel;
    ArtistsModel *mArtistsModel;
    AlbumTracksModel *mAlbumTracksModel;
    QThread *mModelThread;


    // Qml stuff
    QQuickView *mQuickView;

    void readSettings();
    void writeSettings();
    void connectModelSignals();
    void connectQMLSignals();

    // GUI Settings
    int mDownloadEnabled;
    int mDownloadSize;
    int mAlbumViewSetting;
    int mArtistViewSetting;
    int mListImageSize;
    int mSectionsInSearch;
    int mSectionsInPlaylist;
    int mCoverInNowPlaying;

    // Playback objects
    Playlist *mPlaylist;
    PlaybackStatusObject *mPlaybackStatus;


private slots:
    void receiveDownloadSize(int);
    void receiveSettingKey(QVariant setting);

    void addAlbumTrack(int index);
    void playAlbumTrack(int index);
    void addActiveAlbum();
    void playActiveAlbum();




};

#endif // MAINCONTROLLER_H
