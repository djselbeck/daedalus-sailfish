#include "maincontroller.h"

#include <QSettings>
#include <QDBusConnection>

#include "global.h"

#include "dbus/daedalusdbusmainmprisadaptor.h"

MainController::MainController(QObject *parent) :
    QObject(parent)
{
}

MainController::MainController(QQuickView *viewer, QObject *parent) : QObject(parent)
{
    // Metadata-db
    mDBThread = new QThread(this);
    mImgDB = new ImageDatabase(0);
    mImgDB->moveToThread(mDBThread);
    mDBThread->start();

    mQMLImgProvider = new QMLImageProvider(mImgDB);

    mModelThread = new QThread(this);
    mSparQLConnection = new QSparqlConnection("QTRACKER");
    mAlbumsModel = new AlbumsModel(0,mSparQLConnection,mModelThread,mImgDB,mDownloadEnabled);
    mArtistsModel = new ArtistsModel(0,mSparQLConnection,mModelThread,mImgDB);
    mAlbumTracksModel = new AlbumTracksModel(0,mSparQLConnection,mModelThread);
    mPlaylistsModel = new SavedPlaylistsModel(0,mSparQLConnection,mModelThread);
    mPlaylistTracksModel = 0;


    mResumeIndex = 0;
    mResumeTime = 0;

    mPlaylistManager = new PlaylistManager(0,mSparQLConnection);
//    mPlaylistManager->moveToThread(mModelThread);

    mPlaylist = new DaedalusMediaPlayer(0);
    mPlaybackStatus = new PlaybackStatusObject();
    // Register PlaybackStatusObject in metasystem
    qmlRegisterType<PlaybackStatusObject>();
    //qRegisterMetaType<PlaybackStatusObject>("PlaybackStatusObject");
    viewer->engine()->addImageProvider("imagedbprovider",mQMLImgProvider);

    mDbusMainAdaptor = new DaedalusDbusMainMprisAdaptor(this);
    mDbusAdaptor = new DaedalusDBUSAdaptor(mPlaylist,mPlaybackStatus,this);
    QDBusConnection::sessionBus().registerObject(QString("/org/mpris/MediaPlayer2"), this, QDBusConnection::ExportAdaptors);
    QDBusConnection::sessionBus().registerService("org.mpris.MediaPlayer2.daedalus");

    mQuickView = viewer;

    mQuickView->rootContext()->setContextProperty("albumsModel",mAlbumsModel);
    mQuickView->rootContext()->setContextProperty("artistsModel",mArtistsModel);
    mQuickView->rootContext()->setContextProperty("albumTracksModel",mAlbumTracksModel);
    mQuickView->rootContext()->setContextProperty("savedPlaylistsModel",mPlaylistsModel);
    mQuickView->rootContext()->setContextProperty("playlistModel",mPlaylist);
    mQuickView->rootContext()->setContextProperty("playbackstatus",mPlaybackStatus);
    mQuickView->rootContext()->setContextProperty("playlistTracksModel",0);
    mQuickView->rootContext()->setContextProperty("mBusy",true);


    mQuickView->rootContext()->setContextProperty("artistInfoText","");
    mQuickView->rootContext()->setContextProperty("albumInfoText","");
    mQuickView->rootContext()->setContextProperty("versionstring",QVariant::fromValue(QString(VERSION)));



    mModelThread->start();

    mPlaylist->registerStatusObject(mPlaybackStatus);

    mScrobbler = new LastFMScrobbler(mPlaybackStatus,0);

    mDBStatistic = 0;
    mPlaylistAction = 0;


    mPlaylist->setResumeIndex(mResumeIndex);
    mPlaylist->setResumeTime(mResumeTime);
    connectQMLSignals();
    connectModelSignals();
    readSettings();
    if ( mFirstUse ) {
        emit ( requestFirstUseDialog());
    }
    emit requestDBStatistic();
    mPlaylist->resumePlaylist();
}

MainController::~MainController()
{
    mResumeIndex = mPlaybackStatus->getPlaylistPosition();
    mResumeTime = mPlaybackStatus->getElapsed();
    writeSettings();
    mPlaylist->savePlaylistToSql();
    clearSavedPlaylistTracks();
    delete(mAlbumsModel);
    delete(mArtistsModel);
    delete(mAlbumTracksModel);
    delete(mPlaylistsModel);
    delete(mImgDB);
    delete(mPlaylist);
    delete(mScrobbler);
    mModelThread->exit(0);
    mDBThread->exit(0);
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
    connect(item,SIGNAL(requestAllTracks()),mAlbumTracksModel,SLOT(requestAllTracks()));
    connect(item,SIGNAL(requestArtistAlbum(QVariant)),mAlbumTracksModel,SLOT(requestArtistAlbumTracks(QVariant)));

    // Settings
    connect(item,SIGNAL(newDownloadSize(int)),this,SLOT(receiveDownloadSize(int)));
    connect(item,SIGNAL(newSettingKey(QVariant)),this,SLOT(receiveSettingKey(QVariant)));
    // Last.fm userdata
    connect(item,SIGNAL(newLastfmUserData(QVariant)),mScrobbler,SLOT(authenticate(QVariant)));
    connect(item,SIGNAL(clearLastFMAuthentication()),this,SLOT(clearLastFMAuthentication()));
    connect(this,SIGNAL(requestFirstUseDialog()),item,SLOT(showFirstUseDialog()));

    // playlist management
    connect(item,SIGNAL(addAlbumTrack(int)),this,SLOT(addAlbumTrack(int)));
    connect(item,SIGNAL(playAlbumTrack(int)),this,SLOT(playAlbumTrack(int)));
    connect(item,SIGNAL(addAlbumTrackAfterCurrent(int)),this,SLOT(addAlbumTrackAfterCurrent(int)));
    connect(item,SIGNAL(addActiveAlbum()),this,SLOT(addActiveAlbum()));
    connect(item,SIGNAL(playPlaylistIndex(int)),mPlaylist,SLOT(playPosition(int)));
    connect(item,SIGNAL(playActiveAlbum()),this,SLOT(playActiveAlbum()));
    connect(item,SIGNAL(playActiveAlbumRandom()),this,SLOT(playActiveAlbumRandom()));
    connect(item,SIGNAL(playActiveAlbumShuffle()),this,SLOT(playActiveAlbumShuffle()));
    connect(item,SIGNAL(addAlbum(QString)),this,SLOT(addAlbumTracksStart(QString)));
    connect(item,SIGNAL(playAlbum(QString)),this,SLOT(playAlbumTracksStart(QString)));
    connect(item,SIGNAL(addArtist(QString)),this,SLOT(addArtistTracksStart(QString)));
    connect(item,SIGNAL(playArtist(QString)),this,SLOT(playArtistTracksStart(QString)));
    connect(item,SIGNAL(addSavedPlaylistTrack(int)),this,SLOT(addSavedPlaylistTrack(int)));
    connect(item,SIGNAL(addSavedPlaylistTrackAfterCurrent(int)),this,SLOT(addSavedPlaylistTrackAfterCurrent(int)));
    connect(item,SIGNAL(playSavedPlaylistTrack(int)),this,SLOT(playSavedPlaylistTrack(int)));
    connect(item,SIGNAL(addActivePlaylist()),this,SLOT(addActivePlaylist()));
    connect(item,SIGNAL(playActivePlaylist()),this,SLOT(playActivePlaylist()));
    connect(item,SIGNAL(savePlaylist(QString)),mPlaylist,SLOT(savePlaylist(QString)));
    connect(item,SIGNAL(addURL(QString)),mPlaylist,SLOT(addUrl(QString)));
    connect(item,SIGNAL(playPlaylist(QString)),this,SLOT(playPlaylist(QString)));
    connect(item,SIGNAL(addPlaylist(QString)),this,SLOT(addPlaylist(QString)));
    connect(item,SIGNAL(shuffleCurrentPlaylist()),mPlaylist,SLOT(shufflePlaylist()));

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
    connect(item,SIGNAL(requestSavedPlaylists()),mPlaylistsModel,SLOT(requestSavedPlaylists()));
    connect(item,SIGNAL(requestSavedPlaylist(QString)),mPlaylistManager,SLOT(requestPlaylist(QString)));
    connect(item,SIGNAL(deleteActivePlaylist()),mPlaylistManager,SLOT(deleteActivePlaylist()));
    connect(item,SIGNAL(deletePlaylistFile(QString)),mPlaylistManager,SLOT(deletePlaylist(QString)));

    // metadata-db stuff
    connect(mImgDB,SIGNAL(coverAlbumArtReady(QVariant)),item,SLOT(coverArtReceiver(QVariant)));

    connect(mImgDB,SIGNAL(coverArtistArtReady(QVariant)),item,SLOT(coverArtistArtReceiver(QVariant)));
    connect(item,SIGNAL(newDownloadSize(int)),this,SLOT(receiveDownloadSize(int)));

    connect(this, SIGNAL(requestDBStatistic()),mImgDB,SLOT(requestStatisticUpdate()));
    connect(mImgDB,SIGNAL(newStasticReady(DatabaseStatistic*)),this,SLOT(newDBStatisticReceiver(DatabaseStatistic*)));
    connect(item,SIGNAL(bulkDownloadArtists()),this,SLOT(doBulkArtistDownload()));
    connect(item,SIGNAL(bulkDownloadAlbums()),this,SLOT(doBulkAlbumDownload()));

    connect(item,SIGNAL(cleanupBlacklisted()),mImgDB,SLOT(cleanUPBlacklistedAlbums()));
    connect(item,SIGNAL(cleanupAlbums()),mImgDB,SLOT(cleanupAlbums()));
    connect(item,SIGNAL(cleanupArtists()),mImgDB,SLOT(cleanupArtists()));
    connect(item,SIGNAL(cleanupDB()),mImgDB,SLOT(cleanupDatabase()));

    connect(item,SIGNAL(requestAlbumInfo(QVariant)),mImgDB,SLOT(requestAlbumWikiInformation(QVariant)));
    connect(item,SIGNAL(requestArtistInfo(QString)),mImgDB,SLOT(requestArtistBioInformation(QString)));

    connect(mImgDB,SIGNAL(albumWikiInformationReady(QString)),this,SLOT(setAlbumWikiInfo(QString)));
    connect(mImgDB,SIGNAL(artistBioInformationReady(QString)),this,SLOT(setArtistBioInfo(QString)));

    // memory release methods
    connect(item,SIGNAL(clearAlbumsList()),mAlbumsModel,SLOT(clearData()));
    connect(item,SIGNAL(clearAlbumTrackList()),mAlbumTracksModel,SLOT(clearData()));
    connect(item,SIGNAL(clearArtistsList()),mArtistsModel,SLOT(clearData()));
    connect(item,SIGNAL(clearSavedPlaylists()),mPlaylistsModel,SLOT(clearData()));
    connect(item,SIGNAL(clearSavedPlaylistTracks()),this,SLOT(clearSavedPlaylistTracks()));

}

void MainController::connectModelSignals()
{
//    connect(mAlbumsModel,SIGNAL(albumsReady()),this,SLOT(albumsReady()));
//    connect(mArtistsModel,SIGNAL(artistsReady()),this,SLOT(artistsReady()));
    connect(mPlaybackStatus,SIGNAL(artistChanged()),this,SLOT(newPlaybackStatus()));
    connect(mPlaybackStatus,SIGNAL(albumChanged()),this,SLOT(newPlaybackStatus()));

    connect(this,SIGNAL(requestCoverArt(Albumtype)),mImgDB,SLOT(requestCoverImage(Albumtype)));
    connect(this,SIGNAL(requestCoverArtistArt(QString)),mImgDB,SLOT(requestCoverArtistImage(QString)));

    // Set downloading enabled variable to imagedatabase
    connect(this,SIGNAL(newDownloadEnabled(bool)),mImgDB,SLOT(setDownloadEnabled(bool)));
    connect(this,SIGNAL(newDownloadEnabled(bool)),mAlbumsModel,SLOT(setDownloadEnabled(bool)));
    // Received new Download size from database GUI settings
    connect(this,SIGNAL(newDownloadSize(QString)),mImgDB,SLOT(setDownloadSize(QString)));
    connect(this,SIGNAL(newDownloadSize(QString)),mAlbumsModel,SLOT(setDownloadSize(QString)));

    connect(this,SIGNAL(requestArtistBulkDownload(QList<QString>*)),mImgDB,SLOT(fillDatabase(QList<QString>*)));
    connect(this,SIGNAL(requestAlbumBulkDownload(QMap<QString,QList<Albumtype>*>*)),mImgDB,SLOT(fillDatabase(QMap<QString,QList<Albumtype>*>*)));
    connect(mScrobbler,SIGNAL(newLastFMSessionKey(QString)),this,SLOT(receiveLastFMSessionKey(QString)));

    connect(mPlaylistManager,SIGNAL(playlistTracksReady(SavedPlaylistTracksModel*)),this,SLOT(receiveSavedPlaylistTracks(SavedPlaylistTracksModel*)));

    connect(mAlbumsModel,SIGNAL(sendBusy(bool)),this,SLOT(backgroundBusy(bool)));
    connect(mArtistsModel,SIGNAL(sendBusy(bool)),this,SLOT(backgroundBusy(bool)));
    connect(mAlbumTracksModel,SIGNAL(sendBusy(bool)),this,SLOT(backgroundBusy(bool)));
    connect(mPlaylist,SIGNAL(sendBusy(bool)),this,SLOT(backgroundBusy(bool)));
    connect(mPlaylistManager,SIGNAL(sendBusy(bool)),this,SLOT(backgroundBusy(bool)));
}

void MainController::readSettings()
{
    QSettings settings;

    settings.beginGroup("general_properties");
    int dlSize = settings.value("download_size",LASTFM_EXTRALARGE).toInt();
    mArtistViewSetting = settings.value("artist_view",0).toInt();
    mAlbumViewSetting = settings.value("album_view",0).toInt();
    mListImageSize = settings.value("list_image_size",0).toInt();
    mSectionsInSearch = settings.value("sections_in_search",1).toInt();
    mSectionsInPlaylist = settings.value("sections_in_playlist",1).toInt();
    mDownloadEnabled = settings.value("lastfm_download",0).toInt();
    mCoverInNowPlaying = settings.value("show_covernowplaying",1).toInt();
    mLastFMSessionKey = settings.value("lastfmsession","").toString();
    mResumeIndex = settings.value("resumeindex",1).toInt();
    mResumeTime  = settings.value("resumetime",1).toInt();
    mFirstUse = settings.value("firstuse",1).toInt();
    emit newDownloadEnabled(mDownloadEnabled);

    mQuickView->rootContext()->setContextProperty("artistView", mArtistViewSetting);
    mQuickView->rootContext()->setContextProperty("albumView", mAlbumViewSetting);
    mQuickView->rootContext()->setContextProperty("listImageSize", mListImageSize);
    mQuickView->rootContext()->setContextProperty("sectionsInSearch", mSectionsInSearch);
    mQuickView->rootContext()->setContextProperty("sectionsInPlaylist", mSectionsInPlaylist);
    mQuickView->rootContext()->setContextProperty("lastfmEnabled", mDownloadEnabled);
    mQuickView->rootContext()->setContextProperty("showCoverNowPlaying", mCoverInNowPlaying);
    mQuickView->rootContext()->setContextProperty("lastfmsessionkey",mLastFMSessionKey);

    mQuickView->rootContext()->setContextProperty("downloadSize",dlSize);
    mDownloadSize = dlSize;
    emit newDownloadSize(getLastFMArtSize(mDownloadSize));
    settings.endGroup();
    mScrobbler->setSessionKey(mLastFMSessionKey);
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
    settings.setValue("lastfmsession",mLastFMSessionKey);
    settings.setValue("resumeindex",mResumeIndex);
    settings.setValue("resumetime",mResumeTime);
    settings.setValue("firstuse",mFirstUse);
    settings.endGroup();
    readSettings();
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
        } else if ( settings.at(0) == "firstuse" ) {
            mFirstUse = settings.at(1).toInt();
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
    disconnect(mAlbumTracksModel,SIGNAL(modelReady()),this,SLOT(addActiveAlbum()));
}

void MainController::playActiveAlbum()
{
    mPlaylist->clear();
    addActiveAlbum();
    mPlaylist->playPosition(0);
    mPlaylist->setRandom(false);
    disconnect(mAlbumTracksModel,SIGNAL(modelReady()),this,SLOT(playActiveAlbum()));

}

void MainController::playActiveAlbumRandom()
{
    mPlaylist->clear();
    addActiveAlbum();
    mPlaylist->setRandom(true);
    mPlaylist->play();
    mPlaylist->next();
    disconnect(mAlbumTracksModel,SIGNAL(modelReady()),this,SLOT(playActiveAlbum()));
}

void MainController::playActiveAlbumShuffle()
{
    mPlaylist->clear();
    addActiveAlbum();
    mPlaylist->shufflePlaylist();
    disconnect(mAlbumTracksModel,SIGNAL(modelReady()),this,SLOT(playActiveAlbum()));

}

void MainController::addAlbumTracksStart(QString albumurn)
{
    mAlbumTracksModel->requestAlbumTracks(albumurn);
    connect(mAlbumTracksModel,SIGNAL(modelReady()),this,SLOT(addActiveAlbum()));
}

void MainController::playAlbumTracksStart(QString albumurn)
{
    mAlbumTracksModel->requestAlbumTracks(albumurn);
    connect(mAlbumTracksModel,SIGNAL(modelReady()),this,SLOT(playActiveAlbum()));
}


void MainController::addArtistTracksStart(QString artisturn)
{
    mAlbumTracksModel->requestArtistTracks(artisturn);
    connect(mAlbumTracksModel,SIGNAL(modelReady()),this,SLOT(addActiveAlbum()));
}

void MainController::playArtistTracksStart(QString artisturn)
{
    mAlbumTracksModel->requestArtistTracks(artisturn);
    connect(mAlbumTracksModel,SIGNAL(modelReady()),this,SLOT(playActiveAlbum()));
}

void MainController::newPlaybackStatus()
{
    Albumtype albumObj = {mPlaybackStatus->getAlbum(),mPlaybackStatus->getArtist()};
    emit requestCoverArt(albumObj);
    emit requestCoverArtistArt(mPlaybackStatus->getArtist());
}

void MainController::newDBStatisticReceiver(DatabaseStatistic *statistic)
{
    mQuickView->rootContext()->setContextProperty("dbStatistic",statistic);
    if ( mDBStatistic ) {
        delete mDBStatistic;
    }
    mDBStatistic = statistic;
}

void MainController::doBulkAlbumDownload()
{
    connect(mAlbumsModel,SIGNAL(albumsReady()),this,SLOT(receiveBulkAlbumList()));
    mAlbumsModel->requestArtistAlbumList();
}

void MainController::doBulkArtistDownload()
{
    // Get Artist list
    connect(mArtistsModel,SIGNAL(artistsReady()),this,SLOT(receiveBulkArtistList()));
    mArtistsModel->requestArtists();
}

void MainController::receiveBulkArtistList()
{
    QList<QString>* artistList = new QList<QString>();
    for ( int i = 0; i < mArtistsModel->rowCount(); i++) {
        artistList->append(mArtistsModel->get(i)["artist"].toString());
    }
    disconnect(mArtistsModel,SIGNAL(artistsReady()),this,SLOT(receiveBulkArtistList()));
    emit requestArtistBulkDownload(artistList);
}

void MainController::receiveBulkAlbumList()
{
    qDebug() << "start bulk download for " << mAlbumsModel->rowCount() << " albums";
    QMap<QString, QList<Albumtype>* > *map = new QMap<QString, QList<Albumtype>* >();
    QString artistname = "";
    QList<Albumtype> *list;
    for ( int i = 0 ; i < mAlbumsModel->rowCount(); i++ ) {
        QString tempArtist = mAlbumsModel->get(i)["artist"].toString();
        if ( artistname != tempArtist ) {
            qDebug() << "append artist: " << tempArtist;
            // New artist begins, create new list append old one
            list = new QList<Albumtype>();
            artistname = tempArtist;
            (*map)[artistname] = list;
        }
        Albumtype tmpAlbum = {mAlbumsModel->get(i)["title"].toString(),artistname};
        list->append(tmpAlbum);
    }
    disconnect(mAlbumsModel,SIGNAL(albumsReady()),this,SLOT(receiveBulkAlbumList()));
    emit requestAlbumBulkDownload(map);
}


void MainController::setArtistBioInfo(QString info)
{
    mQuickView->rootContext()->setContextProperty("artistInfoText",info);
}

void MainController::setAlbumWikiInfo(QString info)
{
    mQuickView->rootContext()->setContextProperty("albumInfoText",info);
}

void MainController::receiveLastFMSessionKey(QString key)
{
    qDebug() << "Received last.fm session: " << key;
    mLastFMSessionKey = key;
    writeSettings();
}

void MainController::receiveSavedPlaylistTracks(SavedPlaylistTracksModel *model)
{
    clearSavedPlaylistTracks();
    connect(model,SIGNAL(sendBusy(bool)),this,SLOT(backgroundBusy(bool)));
    mPlaylistTracksModel = model;
    mQuickView->rootContext()->setContextProperty("playlistTracksModel",mPlaylistTracksModel);
    qDebug() << "received saved playlist tracks";
    if ( mPlaylistAction == 1 ) {
        connect(mPlaylistTracksModel,SIGNAL(playlistReady()),this,SLOT(playActivePlaylist()));
    } else if ( mPlaylistAction == 2 ) {
        connect(mPlaylistTracksModel,SIGNAL(playlistReady()),this,SLOT(addActivePlaylist()));
    }
    mPlaylistAction = 0;
}


void MainController::addSavedPlaylistTrack(int index)
{
    if ( mPlaylistTracksModel != 0 ) {
        mPlaylist->addFile(mPlaylistTracksModel->getTrack(index));
    }
}

void MainController::addSavedPlaylistTrackAfterCurrent(int index)
{
    if ( mPlaylistTracksModel != 0 ) {
           mPlaylist->insertAt(mPlaylistTracksModel->getTrack(index),mPlaylist->currentIndex() + 1);
    }
}


void MainController::playSavedPlaylistTrack(int index)
{
    qDebug() << "Play saved playlist song: " << index;
    if ( mPlaylistTracksModel != 0 ) {
        mPlaylist->playSong(mPlaylistTracksModel->getTrack(index));
    }
}

void MainController::addActivePlaylist()
{
    if ( mPlaylistTracksModel != 0 ) {
        for ( int i = 0; i < mPlaylistTracksModel->rowCount(); i++ ) {
            mPlaylist->addFile(mPlaylistTracksModel->getTrack(i));
        }
    }
    clearSavedPlaylistTracks();
}

void MainController::playActivePlaylist()
{
    mPlaylist->clear();
    if ( mPlaylistTracksModel != 0 ) {
        for ( int i = 0; i < mPlaylistTracksModel->rowCount(); i++ ) {
            mPlaylist->addFile(mPlaylistTracksModel->getTrack(i));
        }
    }
    mPlaylist->playPosition(0);
    mPlaylist->setRandom(false);
    clearSavedPlaylistTracks();
}

void MainController::clearSavedPlaylistTracks()
{
    qDebug() << "Clear saved pl tracks";
    mQuickView->rootContext()->setContextProperty("playlistTracksModel",0);
    if ( mPlaylistTracksModel != 0 ) {
        delete(mPlaylistTracksModel);
        mPlaylistTracksModel = 0;
    }
}

void MainController::clearLastFMAuthentication()
{
    mLastFMSessionKey = "";
    writeSettings();
}

void MainController::backgroundBusy(bool busy)
{
    mQuickView->rootContext()->setContextProperty("mBusy",busy);
}

void MainController::playPlaylist(QString url)
{
    mPlaylistAction = 1;
    mPlaylistManager->requestPlaylist(url);
}

void MainController::addPlaylist(QString url)
{
    mPlaylistAction = 2;
    mPlaylistManager->requestPlaylist(url);
}
