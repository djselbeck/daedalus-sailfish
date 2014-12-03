#ifndef LASTFMALBUMPROVIDER_H
#define LASTFMALBUMPROVIDER_H

#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QXmlStreamReader>

#include <metadata/albuminformation.h>
#include <metadata/album.h>


class LastFMAlbumProvider : public QObject
{
    Q_OBJECT
public:
    LastFMAlbumProvider(QString albumName,QString artistName,QObject *parent=0);
    ~LastFMAlbumProvider();

    void startDownload();

    void setArtistAlbum(Albumtype album);
    void setDownloadSize(QString size);


    QString getImageURL();

    AlbumInformation* getLastInformation();

public slots:
    void requestDownload(Albumtype album);

signals:
    void ready(AlbumInformation *albumInfo);
    void readyRead();
    void failed();

private:
    QString mAlbumName;
    QString mArtistName;
    QString mImageURL;
    QString mAlbumInfo;
    QByteArray *mImageData;
    QNetworkAccessManager *mXMLNetAccess;
    QNetworkAccessManager *mAlbumArtAccess;
    AlbumInformation *mLastInformation;

    QString mDownloadSize;

    void parseAlbum(QXmlStreamReader &xmlReader);
    void parseWikiInformation(QXmlStreamReader &xmlReader);
    void downloadImageData(QUrl imageURL);

private slots:
    void xmlDownloaded(QNetworkReply*);
    void imageDownloaded(QNetworkReply*);
//    void downloadError(QNetworkReply*);

};

#endif // LASTFMURLPROVIDER_H
