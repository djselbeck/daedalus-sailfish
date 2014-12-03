#ifndef IMAGEDOWNLOADER_H
#define IMAGEDOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QThread>
#include <QMutex>
#include <QEventLoop>
#include <QQueue>

#include <metadata/lastfmalbumprovider.h>
#include <metadata/lastfmartistprovider.h>
#include <metadata/albuminformation.h>
#include <metadata/artistinformation.h>
#include <metadata/album.h>

class ImageDownloader : public QObject
{
    Q_OBJECT
public:

    ImageDownloader();
    ~ImageDownloader();

    int getArtistQueueSize();
    int getAlbumQueueSize();

    void setDownloadSize(QString size);


private:
    QThread *mWorkingThread;
    QMutex mDownloadCounterMutex;

    int mRunningAlbumDownloads;
    int mRunningArtistDownloads;

    QQueue<Albumtype> *mAlbumQueue;
    QQueue<QString> *mArtistQueue;

    LastFMAlbumProvider *mAlbumProvider;
    LastFMArtistProvider *mArtistProvider;



signals:
    void albumInformationReady(AlbumInformation *info);
    void artistInformationReady(ArtistInformation *info);

public slots:
    void requestAlbumArt(Albumtype album);
    void albumInformationReceiver(AlbumInformation *info);
    void albumDownloadFailure();

    void requestArtistArt(QString artistObj);
    void artistInformationReceiver(ArtistInformation *info);
    void artistDownloadFailure();

    void clearDownloadQueue();


};

#endif // IMAGEDOWNLOADER_H
