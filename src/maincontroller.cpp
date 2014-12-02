#include "maincontroller.h"

#include <QSettings>

#include "global.h"

MainController::MainController(QObject *parent) :
    QObject(parent)
{
}

MainController::MainController(QQuickView *viewer, QObject *parent) : QObject(parent)
{
    mModelThread = new QThread(this);
    mSparQLConnection = new QSparqlConnection("QTRACKER_DIRECT");
    mAlbumsModel = new AlbumsModel(0,mSparQLConnection,mModelThread);
    mArtistsModel = new ArtistsModel(0,mSparQLConnection,mModelThread);
    mAlbumTracksModel = new AlbumTracksModel(0,mSparQLConnection,mModelThread);

    mPlaylist = new Playlist(0);
    mPlaybackStatus = new PlaybackStatusObject();
    // Register PlaybackStatusObject in metasystem
    qmlRegisterType<PlaybackStatusObject>();
    //qRegisterMetaType<PlaybackStatusObject>("PlaybackStatusObject");

    mQuickView = viewer;

    mQuickView->rootContext()->setContextProperty("albumsModel",mAlbumsModel);
    mQuickView->rootContext()->setContextProperty("artistsModel",mArtistsModel);
    mQuickView->rootContext()->setContextProperty("albumTracksModel",mAlbumTracksModel);
    mQuickView->rootContext()->setContextProperty("playlistModel",mPlaylist);
    mQuickView->rootContext()->setContextProperty("playbackstatus",mPlaybackStatus);

    mQuickView->rootContext()->setContextProperty("artistInfoText","");
    mQuickView->rootContext()->setContextProperty("albumInfoText","");

    connectQMLSignals();
    connectModelSignals();
    mModelThread->start();

    readSettings();
    mPlaylist->registerStatusObject(mPlaybackStatus);
}

void MainController::requestAlbums()
{
    qDebug() << "Albums requested";
    mAlbumsModel->requestAlbums();
}

void MainController::requestArtists()
{
    qDebug() << "Artists requested";
    mArtistsModel->requestArtists();
}

void MainController::requestPlaylists()
{

}

void MainController::requestFolder(QString path)
{

}


void MainController::playlistsReady()
{

}

void MainController::folderReady()
{

}


void MainController::connectQMLSignals()
{
    QObject *item = (QObject *)mQuickView->rootObject();
    connect(item,SIGNAL(requestAlbums()),this,SLOT(requestAlbums()));
    connect(item,SIGNAL(requestArtists()),this,SLOT(requestArtists()));
    connect(item,SIGNAL(requestArtistAlbums(QString)),mAlbumsModel,SLOT(requestAlbums(QString)));
    connect(item,SIGNAL(requestAlbum(QString)),mAlbumTracksModel,SLOT(requestAlbumTracks(QString)));

    // Settings
    connect(item,SIGNAL(newDownloadSize(int)),this,SLOT(receiveDownloadSize(int)));
    connect(item,SIGNAL(newSettingKey(QVariant)),this,SLOT(receiveSettingKey(QVariant)));

    // playlist management
    connect(item,SIGNAL(addAlbumTrack(int)),this,SLOT(addAlbumTrack(int)));
    connect(item,SIGNAL(playAlbumTrack(int)),this,SLOT(playAlbumTrack(int)));
    connect(item,SIGNAL(addAlbumTrackAfterCurrent(int)),this,SLOT(addAlbumTrackAfterCurrent(int)));
    connect(item,SIGNAL(addActiveAlbum()),this,SLOT(addActiveAlbum()));
    connect(item,SIGNAL(playPlaylistIndex(int)),mPlaylist,SLOT(playPosition(int)));
    connect(item,SIGNAL(playActiveAlbum()),this,SLOT(playActiveAlbum()));

    // basic controls
    connect(item,SIGNAL(next()),mPlaylist,SLOT(next()));
    connect(item,SIGNAL(prev()),mPlaylist,SLOT(previous()));
    connect(item,SIGNAL(stop()),mPlaylist,SLOT(stop()));
    connect(item,SIGNAL(togglePlayPause()),mPlaylist,SLOT(togglePlayPause()));
    connect(item,SIGNAL(play()),mPlaylist,SLOT(play()));
    connect(item,SIGNAL(pause()),mPlaylist,SLOT(pause()));
    connect(item,SIGNAL(seek(int)),mPlaylist,SLOT(seek(int)));
    connect(item,SIGNAL(setRepeat(bool)),mPlaylist,SLOT(setRepeat(bool)));
    connect(item,SIGNAL(setShuffle(bool)),mPlaylist,SLOT(setRandom(bool)));
    connect(item,SIGNAL(deletePlaylist()),mPlaylist,SLOT(clear()));
    connect(item,SIGNAL(deletePlaylistTrack(int)),mPlaylist,SLOT(removePosition(int)));
    connect(item,SIGNAL(playPlaylistSongNext(int)),mPlaylist,SLOT(playNext(int)));
    connect(item,SIGNAL(requestTrackAlbumTracks(QString)),mAlbumTracksModel,SLOT(requestAlbumTracksReverseFromTrack(QString)));
    connect(item,SIGNAL(requestTrackArtistAlbums(QString)),mAlbumsModel,SLOT(requestArtistAlbumsReverseFromTrack(QString)));
}

void MainController::connectModelSignals()
{
//    connect(mAlbumsModel,SIGNAL(albumsReady()),this,SLOT(albumsReady()));
//    connect(mArtistsModel,SIGNAL(artistsReady()),this,SLOT(artistsReady()));
}

void MainController::readSettings()
{
    QSettings settings;

    settings.beginGroup("general_properties");
    int dlSize = settings.value("download_size",LASTFM_EXTRALARGE).toInt();
    mArtistViewSetting = settings.value("artist_view",0).toInt();
    mAlbumViewSetting = settings.value("album_view",1).toInt();
    mListImageSize = settings.value("list_image_size",0).toInt();
    mSectionsInSearch = settings.value("sections_in_search",1).toInt();
    mSectionsInPlaylist = settings.value("sections_in_playlist",1).toInt();
    mDownloadEnabled = settings.value("lastfm_download",1).toInt();
    mCoverInNowPlaying = settings.value("show_covernowplaying",1).toInt();

    emit newDownloadEnabled(mDownloadEnabled);

    mQuickView->rootContext()->setContextProperty("artistView", mArtistViewSetting);
    mQuickView->rootContext()->setContextProperty("albumView", mAlbumViewSetting);
    mQuickView->rootContext()->setContextProperty("listImageSize", mListImageSize);
    mQuickView->rootContext()->setContextProperty("sectionsInSearch", mSectionsInSearch);
    mQuickView->rootContext()->setContextProperty("sectionsInPlaylist", mSectionsInPlaylist);
    mQuickView->rootContext()->setContextProperty("lastfmEnabled", mDownloadEnabled);
    mQuickView->rootContext()->setContextProperty("showCoverNowPlaying", mCoverInNowPlaying);

    mQuickView->rootContext()->setContextProperty("downloadSize",dlSize);
    mDownloadSize = dlSize;
    emit newDownloadSize(getLastFMArtSize(mDownloadSize));
    settings.endGroup();
}

void MainController::writeSettings()
{
    QSettings settings;
    settings.clear();
    settings.beginGroup("general_properties");
    settings.setValue("download_size",mDownloadSize);
    settings.setValue("artist_view",mArtistViewSetting);
    settings.setValue("album_view",mAlbumViewSetting);
    settings.setValue("list_image_size",mListImageSize);
    settings.setValue("sections_in_search",mSectionsInSearch);
    settings.setValue("sections_in_playlist",mSectionsInPlaylist);
    settings.setValue("lastfm_download",mDownloadEnabled);
    settings.setValue("show_covernowplaying",mCoverInNowPlaying);
    settings.endGroup();
}

QString MainController::getLastFMArtSize(int index)
{
    switch (index)  {
    case 0: {
        return "small";
        break;
    }
    case 1: {
        return "medium";
        break;
    }
    case 2: {
        return "large";
        break;
    }
    case 3: {
        return "extralarge";
        break;
    }
    case 4: {
        return "mega";
        break;
    }
    }
    return LASTFMDEFAULTSIZE;
}

void MainController::receiveDownloadSize(int size)
{
    mDownloadSize = size;
    mQuickView->rootContext()->setContextProperty("downloadSize",size);
    emit newDownloadSize(getLastFMArtSize(size));
    writeSettings();
}

void MainController::receiveSettingKey(QVariant setting)
{
    QStringList settings = setting.toStringList();
    if ( settings.length() == 2 ) {
        if ( settings.at(0) == "albumView" ) {
            mAlbumViewSetting = settings.at(1).toInt();
            mQuickView->rootContext()->setContextProperty("albumView", mAlbumViewSetting);
        } else if ( settings.at(0) == "artistView" ) {
            mArtistViewSetting = settings.at(1).toInt();
            mQuickView->rootContext()->setContextProperty("artistView", mArtistViewSetting);
        } else if ( settings.at(0) == "listImageSize" ) {
            mListImageSize = settings.at(1).toInt();
            mQuickView->rootContext()->setContextProperty("listImageSize", mListImageSize);
        } else if ( settings.at(0) == "sectionsInSearch" ) {
            mSectionsInSearch = settings.at(1).toInt();
            mQuickView->rootContext()->setContextProperty("sectionsInSearch", mSectionsInSearch);
        } else if ( settings.at(0) == "sectionsInPlaylist" ) {
            mSectionsInPlaylist = settings.at(1).toInt();
            mQuickView->rootContext()->setContextProperty("sectionsInPlaylist", mSectionsInPlaylist);
        } else if ( settings.at(0) == "lastfmEnabled" ) {
            mDownloadEnabled = settings.at(1).toInt();
            mQuickView->rootContext()->setContextProperty("lastfmEnabled", mDownloadEnabled);
            emit newDownloadEnabled(mDownloadEnabled);
        } else if ( settings.at(0) == "showCoverNowPlaying" ) {
            mCoverInNowPlaying = settings.at(1).toInt();
            mQuickView->rootContext()->setContextProperty("showCoverNowPlaying", mCoverInNowPlaying);
        }

    }
    writeSettings();
}

// FIXME clearup
void MainController::addAlbumTrack(int index)
{
    // get track information from model and create trackobject
    QVariantMap modelTrack = mAlbumTracksModel->get(index);
    QVariant title = modelTrack["title"];
    QVariant album = modelTrack["album"];
    QVariant artist = modelTrack["artist"];
    QVariant length = modelTrack["length"];
    QVariant tracknr = modelTrack["tracknr"];
    QVariant discnr = modelTrack["discnr"];
    QVariant url = modelTrack["fileurl"];
    QVariant urn = modelTrack["trackurn"];
    TrackObject *track = new TrackObject(title.toString(),artist.toString(),album.toString(),url.toString(),urn.toUrl(),length.toInt(),tracknr.toInt(),discnr.toInt(),this);
    mPlaylist->addFile(track);
}

void MainController::playAlbumTrack(int index)
{
    // get track information from model and create trackobject
    QVariantMap modelTrack = mAlbumTracksModel->get(index);
    QVariant title = modelTrack["title"];
    QVariant album = modelTrack["album"];
    QVariant artist = modelTrack["artist"];
    QVariant length = modelTrack["length"];
    QVariant tracknr = modelTrack["tracknr"];
    QVariant discnr = modelTrack["discnr"];
    QVariant url = modelTrack["fileurl"];
    QVariant urn = modelTrack["trackurn"];
    TrackObject *track = new TrackObject(title.toString(),artist.toString(),album.toString(),url.toString(),urn.toUrl(),length.toInt(),tracknr.toInt(),discnr.toInt(),this);
    mPlaylist->playSong(track);
}

void MainController::addAlbumTrackAfterCurrent(int index)
{
    // get track information from model and create trackobject
    QVariantMap modelTrack = mAlbumTracksModel->get(index);
    QVariant title = modelTrack["title"];
    QVariant album = modelTrack["album"];
    QVariant artist = modelTrack["artist"];
    QVariant length = modelTrack["length"];
    QVariant tracknr = modelTrack["tracknr"];
    QVariant discnr = modelTrack["discnr"];
    QVariant url = modelTrack["fileurl"];
    QVariant urn = modelTrack["trackurn"];
    TrackObject *track = new TrackObject(title.toString(),artist.toString(),album.toString(),url.toString(),urn.toUrl(),length.toInt(),tracknr.toInt(),discnr.toInt(),this);
    mPlaylist->insertAt(track,mPlaylist->currentIndex() + 1);
}

void MainController::addActiveAlbum()
{
    for ( int i = 0; i < mAlbumTracksModel->rowCount() ; i++) {
        addAlbumTrack(i);
    }
}

void MainController::playActiveAlbum()
{
    mPlaylist->clear();
    addActiveAlbum();
    mPlaylist->playPosition(0);
}
