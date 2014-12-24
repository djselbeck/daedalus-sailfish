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
#include "model/savedplaylistsmodel.h"
#include "model/playlistmanager.h"
#include "model/savedplaylisttracksmodel.h"
#include "model/playbackstate.h"

#include "mediaplayer/daedalusmediaplayer.h"
#include "mediaplayer/lastfmscrobbler.h"

#include "metadata/imagedatabase.h"
#include "metadata/qmlimageprovider.h"

#include "dbus/daedalusdbusadaptor.h"
#include "dbus/daedalusdbusmainmprisadaptor.h"

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
    ~MainController();
    MainController(QQuickView *viewer,QObject *parent = 0);

signals:
    // local cover database stuff
    void newDownloadSize(QString);
    void newDownloadEnabled(bool);

    void requestCoverArt(Albumtype album);
    void requestCoverArtistArt(QString artist);

    void requestDBStatistic();

    void requestArtistBulkDownload(QList<QString>*);
    void requestAlbumBulkDownload(QMap<QString, QList<Albumtype>* > *map);

    void requestFirstUseDialog();



public slots:
    // request slos
    void requestAlbums();
    void requestArtists();
    void requestPlaylists();
    void requestFolder(QString path);

    void playlistsReady();
    void folderReady();

    void backgroundBusy(bool);

private:
    // Local metadata-db
    DatabaseStatistic *mDBStatistic;
    int mDownloadSize;
    int mDownloadEnabled;
    QString getLastFMArtSize(int index);
    ImageDatabase *mImgDB;
    QMLImageProvider *mQMLImgProvider;

    // SparQL
    QSparqlConnection *mSparQLConnection;

    AlbumsModel *mAlbumsModel;
    ArtistsModel *mArtistsModel;
    AlbumTracksModel *mAlbumTracksModel;
    SavedPlaylistsModel *mPlaylistsModel;
    PlaylistManager *mPlaylistManager;
    SavedPlaylistTracksModel *mPlaylistTracksModel;

    QThread *mModelThread;
    QThread *mDBThread;

    // DBus remote mpris interface
    DaedalusDBUSAdaptor *mDbusAdaptor;
    DaedalusDbusMainMprisAdaptor *mDbusMainAdaptor;

    // Qml stuff
    QQuickView *mQuickView;

    void readSettings();
    void writeSettings();
    void connectModelSignals();
    void connectQMLSignals();

    // GUI Settings
    int mAlbumViewSetting;
    int mArtistViewSetting;
    int mListImageSize;
    int mSectionsInSearch;
    int mSectionsInPlaylist;
    int mCoverInNowPlaying;
    int mFirstUse;

    // Playback objects
    DaedalusMediaPlayer *mPlaylist;
    PlaybackStatusObject *mPlaybackStatus;

    // Scrobbling
    QString mLastFMSessionKey;
    LastFMScrobbler *mScrobbler;

    unsigned int mResumeTime;
    unsigned int mResumeIndex;

    // FIXME clear this up?
    int mPlaylistAction;


private slots:
    void receiveSavedPlaylistTracks(SavedPlaylistTracksModel *model);

    void receiveDownloadSize(int);
    void receiveSettingKey(QVariant setting);

    void addAlbumTrack(int index);
    void playAlbumTrack(int index);
    void addAlbumTrackAfterCurrent(int index);
    void addActiveAlbum();
    void playActiveAlbum();
    void playActiveAlbumRandom();
    void playActiveAlbumShuffle();

    void addSavedPlaylistTrack(int index);
    void addSavedPlaylistTrackAfterCurrent(int index);
    void playSavedPlaylistTrack(int index);

    void addAlbumTracksStart(QString albumurn);
    void playAlbumTracksStart(QString albumurn);

    void addArtistTracksStart(QString artisturn);
    void playArtistTracksStart(QString artisturn);

    void newPlaybackStatus();
    void newDBStatisticReceiver(DatabaseStatistic *statistic);

    void doBulkArtistDownload();
    void doBulkAlbumDownload();

    void receiveBulkArtistList();
    void receiveBulkAlbumList();

    void setArtistBioInfo(QString info);
    void setAlbumWikiInfo(QString info);

    void receiveLastFMSessionKey(QString key);
    void clearLastFMAuthentication();

    void playActivePlaylist();
    void addActivePlaylist();

    void playPlaylist(QString url);
    void addPlaylist(QString url);

    void clearSavedPlaylistTracks();

};

#endif // MAINCONTROLLER_H
