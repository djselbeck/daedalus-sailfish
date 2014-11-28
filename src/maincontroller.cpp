#include "maincontroller.h"

#include <QSettings>

#include "global.h"

MainController::MainController(QObject *parent) :
    QObject(parent)
{
}

MainController::MainController(QQuickView *viewer, QObject *parent) : QObject(parent)
{
    mSparQLConnection = new QSparqlConnection("QTRACKER_DIRECT");
    mAlbumsModel = new AlbumsModel(this,mSparQLConnection);
    mQuickView = viewer;

    mQuickView->rootContext()->setContextProperty("albumsModel",0);
    mQuickView->rootContext()->setContextProperty("listImageSize", 1);

    connectQMLSignals();
    connectModelSignals();

    readSettings();
}

void MainController::requestAlbums()
{
    qDebug() << "Albums requested";
    mAlbumsModel->requestAlbums();
}

void MainController::requestArtists()
{

}

void MainController::requestPlaylists()
{

}

void MainController::requestFolder(QString path)
{

}

void MainController::albumsReady()
{
    qDebug() << "Albums ready";
    mQuickView->rootContext()->setContextProperty("albumsModel",mAlbumsModel);
}

void MainController::artistsReady()
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
}

void MainController::connectModelSignals()
{
    connect(mAlbumsModel,SIGNAL(albumsReady()),this,SLOT(albumsReady()));
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

