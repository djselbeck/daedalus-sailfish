#ifndef IMAGEDATABASE_H
#define IMAGEDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QDir>
#include <QStandardPaths>
#include <QPixmap>
#include <QEventLoop>
#include <QMutex>

#include <metadata/albuminformation.h>
#include <metadata/artistinformation.h>
#include <metadata/lastfmalbumprovider.h>
#include <metadata//imagedownloader.h>
#include <metadata/album.h>

#include "databasestatistic.h"

#define COMPRESSION_LEVEL 9

class ImageDatabase : public QObject
{
    Q_OBJECT

public:
    explicit ImageDatabase(QObject *parent = 0);
    ~ImageDatabase();

//    bool syncAlbums(QList<Albumtype*> *albums,MpdArtist *artist);
//    bool syncArtists(QList<MpdAlbum*> &artists);
    bool hasAlbumArt(QString album,QString artist);
    bool hasArtistArt(QString artist);

    int imageIDFromHash(QString hashValue);
    int imageIDFromAlbumArtist(QString album,QString artist);
    int imageIDFromAlbum(QString album);
    int imageIDFromArtist(QString artist);


    QPixmap getAlbumImage(QString album, QString artist, bool download=false);
    QPixmap getAlbumImage(QString album, bool download=false);
    QPixmap getAlbumImage(int artworkID);
    QPixmap getArtistImage(QString artist,bool download=false);
    QPixmap getArtistImage(int artworkID);
    QPixmap getArtistImageForAlbum(QString album);

    QString getArtistBioInformation(QString artist);
    QString getAlbumWikiInformation(QString album, QString artist);


public slots:
    void albumReady(AlbumInformation *albumInformation);
    void fillDatabase(QMap<QString, QList<Albumtype>* > *map);
    void fillDatabase(QList<QString> *artistList);
    void enterAlbumInformation(AlbumInformation *info);
    void enterArtistInformation(ArtistInformation *info);

    void requestCoverImage(Albumtype album);
    void requestCoverArtistImage(QString artist);

    void requestStatisticUpdate();

    void requestArtistBioInformation(QString artist);
    void requestAlbumWikiInformation(QVariant album);

    // Cleanups
    void cleanUPBlacklistedAlbums();
    void cleanupAlbums();
    void cleanupArtists();
    void cleanupOrphans();
    void cleanupDatabase();

    void setDownloadSize(QString size);
    void setDownloadEnabled(bool enabled);


private:
    QSqlDatabase *mDB;
    QString mDBFilePath;

    bool mAlbumSyncRunning;
    int mAlbumNo;
    QString mDownloadSize;
    LastFMAlbumProvider *mCurrentAlbumProvider;
    QList<Albumtype>* mAlbums;
    QString *mAlbumArtist;

    ImageDownloader *mDownloader;

    // Holds the currently playing album for cover image retrieval
    Albumtype mCoverAlbum;
    QString mCoverArtist;

    // Statistical variables
    int mAlbumCount;
    int mImageCount;
    int mArtistCount;
    int mAlbumBlacklistCount;

    // Settings
    bool mDownloadEnabled;

    int getArtistCount();
    int getAlbumCount();
    int getImageCount();
    int getBlacklistCount();

    void createTables();
    DatabaseStatistic *updateStatistic();

signals:
    void requestAlbumDownload(Albumtype album);
    void coverAlbumArtReady(QVariant url);

    void requestArtistDownload(QString artist);
    void coverArtistArtReady(QVariant url);

    void albumEntered(QString name);

    void newStasticReady(DatabaseStatistic *statistic);

    void artistBioInformationReady(QString);
    void albumWikiInformationReady(QString);


public slots:

};

#endif // IMAGEDATABASE_H
