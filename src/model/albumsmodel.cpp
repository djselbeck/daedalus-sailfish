#include "albumsmodel.h"

#include <QUrl>

// REmove me
#include <QSparqlResult>

AlbumsModel::AlbumsModel(QObject *parent, QSparqlConnection *connection, QThread *fetchthread, ImageDatabase *db, bool downloadEnable) :
    QAbstractListModel(parent)
{

    if ( connection != NULL ) {
        mConnection = connection;
    }
    mSparqlModel = new QSparqlQueryModel(0);
    if ( fetchthread != 0  ) {
        mThread = fetchthread;
        mSparqlModel->moveToThread(mThread);
    }
    mDB = db;
    mDownloadEnabled = downloadEnable;
    connect(mSparqlModel,SIGNAL(finished()),this,SLOT(sparqlModelfinished()));
    mDownloader = new ImageDownloader();
}

AlbumsModel::~AlbumsModel()
{
    delete(mDownloader);
}


void AlbumsModel::requestAlbums()
{
    emit sendBusy(true);
    // Initialize the query
    mPartialModel = false;
    mSparqlModel->clear();
    beginResetModel();
    qDebug() << "getting albums";
    // See qsparqlquerymodel.cpp and 4 from title, trackcount, totalduration, artistname
    mAlbumsQueryString = "SELECT nmm:albumTitle(?album) as ?albumname COUNT(distinct ?piece) as ?trackcount SUM( distinct nfo:duration(?piece)) as ?totalduration nmm:artistName(nmm:performer(?piece)) as ?artistname ?album WHERE { ?piece a nmm:MusicPiece . OPTIONAL { ?piece nmm:musicAlbum ?album} } GROUP BY ?album ORDER BY ?album";
    mSparqlModel->setQuery(QSparqlQuery(mAlbumsQueryString),*mConnection);
    endResetModel();
    disconnect(this,SIGNAL(requestAlbumInformation(Albumtype)),mDownloader,SLOT(requestAlbumArt(Albumtype)));
    disconnect(mDownloader,SIGNAL(albumInformationReady(AlbumInformation*)),this,SLOT(albumInformationReady(AlbumInformation*)));
    disconnect(this,SIGNAL(requestDBEnter(AlbumInformation*)),mDB,SLOT(enterAlbumInformation(AlbumInformation*)));
    disconnect(mDB,SIGNAL(albumEntered(QString)),this,SLOT(albumEntered(QString)));
}


void AlbumsModel::requestArtistAlbumList()
{
    emit sendBusy(true);
    // Initialize the query
    mPartialModel = false;
    mSparqlModel->clear();
    beginResetModel();
    qDebug() << "getting albums";
    // See qsparqlquerymodel.cpp and 4 from title, trackcount, totalduration, artistname
    mAlbumsQueryString = "SELECT nmm:albumTitle(?album) as ?albumname COUNT(distinct ?piece) as ?trackcount SUM(DISTINCT nfo:duration(?piece)) as ?totalduration nmm:artistName(?artistIns) as ?artistname ?album WHERE { ?piece nmm:musicAlbum ?album . ?album nmm:albumArtist ?artistIns } GROUP BY ?artistname ?album ORDER BY ?artistname";
    mSparqlModel->setQuery(QSparqlQuery(mAlbumsQueryString),*mConnection);
    endResetModel();
    disconnect(this,SIGNAL(requestAlbumInformation(Albumtype)),mDownloader,SLOT(requestAlbumArt(Albumtype)));
    disconnect(mDownloader,SIGNAL(albumInformationReady(AlbumInformation*)),this,SLOT(albumInformationReady(AlbumInformation*)));
    disconnect(this,SIGNAL(requestDBEnter(AlbumInformation*)),mDB,SLOT(enterAlbumInformation(AlbumInformation*)));
    disconnect(mDB,SIGNAL(albumEntered(QString)),this,SLOT(albumEntered(QString)));
}

void AlbumsModel::requestAlbums(QString artist)
{
    emit sendBusy(true);
    qDebug() << "getting albums of: " << artist;
    // Initialize the query
    mPartialModel = true;
    mSparqlModel->clear();
    beginResetModel();
    qDebug() << "getting albums";
    artist = artist.replace('<',"%3C");
    artist = artist.replace('>',"%3E");
    if ( artist != "" ) {
        mAlbumsQueryString = "SELECT nmm:albumTitle(?album) as ?albumname COUNT(?piece) as ?trackcount SUM(nfo:duration(?piece)) as ?totalduration nmm:artistName(?performer) as ?artistname ?album ?performer WHERE { ?piece nmm:performer <" + artist + ">  . OPTIONAL { ?piece nmm:musicAlbum ?album } . ?piece nmm:performer ?performer  } GROUP BY ?album ORDER BY ?album";
    } else {
        mAlbumsQueryString = "SELECT nmm:albumTitle(?album) as ?albumname COUNT(?piece) as ?trackcount SUM(nfo:duration(?piece)) as ?totalduration nmm:artistName(?performer) as ?artistname ?album ?performer WHERE { ?piece a nmm:MusicPiece  . OPTIONAL { ?piece nmm:musicAlbum ?album } . OPTIONAL { ?piece nmm:performer ?performer } FILTER(!bound(?performer))  } GROUP BY ?album ORDER BY ?album";
    }
    mSparqlModel->setQuery(QSparqlQuery(mAlbumsQueryString),*mConnection);
    connect(this,SIGNAL(requestAlbumInformation(Albumtype)),mDownloader,SLOT(requestAlbumArt(Albumtype)),Qt::QueuedConnection);
    connect(mDownloader,SIGNAL(albumInformationReady(AlbumInformation*)),this,SLOT(albumInformationReady(AlbumInformation*)));
    connect(this,SIGNAL(requestDBEnter(AlbumInformation*)),mDB,SLOT(enterAlbumInformation(AlbumInformation*)));
    connect(mDB,SIGNAL(albumEntered(QString)),this,SLOT(albumEntered(QString)));
}

void AlbumsModel::requestArtistAlbumsReverseFromTrack(QString urn)
{
    emit sendBusy(true);
    urn = urn.replace('<',"%3C");
    urn = urn.replace('>',"%3E");
    mPartialModel = true;
    qDebug() << "getting albums of: " << urn;
    // Initialize the query
    mSparqlModel->clear();
    beginResetModel();
    qDebug() << "getting albums";
    mAlbumsQueryString = "SELECT nmm:albumTitle(?album) as ?albumname COUNT(?piece) as ?trackcount SUM(nfo:duration(?piece)) as ?totalduration nmm:artistName(?artist) as ?artistname ?album WHERE { <" + urn + "> nmm:performer ?artist . ?piece nmm:performer ?artist . OPTIONAL { ?piece nmm:musicAlbum ?album } . ?piece nmm:performer ?performer } GROUP BY ?album ORDER BY ?album";
    // See qsparqlquerymodel.cpp and 4 from title, trackcount, totalduration, artistname
    mSparqlModel->setQuery(QSparqlQuery(mAlbumsQueryString),*mConnection);
    connect(this,SIGNAL(requestAlbumInformation(Albumtype)),mDownloader,SLOT(requestAlbumArt(Albumtype)),Qt::QueuedConnection);
    connect(mDownloader,SIGNAL(albumInformationReady(AlbumInformation*)),this,SLOT(albumInformationReady(AlbumInformation*)));
    connect(this,SIGNAL(requestDBEnter(AlbumInformation*)),mDB,SLOT(enterAlbumInformation(AlbumInformation*)));
    connect(mDB,SIGNAL(albumEntered(QString)),this,SLOT(albumEntered(QString)));

}

void AlbumsModel::sparqlModelfinished()
{
    beginResetModel();
    qDebug() << "underlaying model finished result fetching";
    emit albumsReady();
    endResetModel();
    emit sendBusy(false);
}


QHash<int, QByteArray> AlbumsModel::roleNames() const {
    QHash<int,QByteArray> roles;

    roles[AlbumRole] = "title";
    roles[TrackCountRole] = "trackcount";
    roles[DurationRole] = "duration";
    roles[ArtistRole] = "artist";
    roles[AlbumURNRole] = "albumurn";
    roles[ArtistURNRole] = "artisturn";
    roles[SectionRole] = "sectionprop";
    roles[AlbumCleandRole] = "titleClean";
    roles[AlbumImageRole] = "coverURL";
    roles[LengthFormattedRole] = "lengthformatted";
    return roles;
}

int AlbumsModel::rowCount(const QModelIndex &parent) const {
    return mSparqlModel->rowCount(parent);
}

QVariant AlbumsModel::data(const QModelIndex &index, int role) const {
    QString albumTitle;
    QUrl urn;
    switch ( role ) {
    case AlbumRole:
        return mSparqlModel->data(index,role);
        break;
    case AlbumURNRole:
        urn = mSparqlModel->data(index,role).toUrl();
        return urn.toEncoded();
        break;
    case ArtistURNRole:
        urn = mSparqlModel->data(index,role).toUrl();
        return urn.toEncoded();
        break;
    case TrackCountRole:
        urn = mSparqlModel->data(index,role).toUrl();
        return urn.toEncoded();
        break;
    case SectionRole:
        albumTitle = mSparqlModel->data(index,AlbumRole).toString();
        if ( albumTitle.length() > 0 ) {
        return QString(albumTitle.at(0)).toUpper();
        } else {
            return "";
        }
        break;
    case ArtistRole:
        return mSparqlModel->data(index,role);
        break;
    case AlbumCleandRole:

        break;
    case AlbumImageRole:
        break;
    default:
        break;
    }
    if ( role == AlbumImageRole) {
        QString album = mSparqlModel->data(index,AlbumRole).toString();
        QString artist = mSparqlModel->data(index,ArtistRole).toString();
                qDebug() << "Image for album: " << album << "requested";
                if ( artist != "" ) {
                    int imageID = mDB->imageIDFromAlbumArtist(album,artist);
                    // No image found return dummy url
                    if ( imageID == -1 ) {
                        // Start image retrieval
                        qDebug() << "returning dummy image for album: " << album;
                        if ( mDownloadEnabled && mPartialModel) {
                            Albumtype albumObj = {album,artist};
                            emit requestAlbumInformation(albumObj);
                        }
                        // Return dummy for the time being
                        return DUMMY_ALBUMIMAGE;
                    } else if (imageID == -2 ) {
                        // Try getting album art for album with out artist (think samplers)
                        imageID = mDB->imageIDFromAlbum(album);
                        if ( imageID >= 0 ) {
                            QString url = "image://imagedbprovider/albumid/" + QString::number(imageID);
                            return url;
                        }
                        qDebug() << "returning dummy image for blacklisted album: " << album;
                        return DUMMY_ALBUMIMAGE;
                    } else {
                        qDebug() << "returning database image for album: " << album;
                        QString url = "image://imagedbprovider/albumid/" + QString::number(imageID);
                        return url;
                    }
                }
                else {
                    int imageID = mDB->imageIDFromAlbum(album);

                    // No image found return dummy url
                    if ( imageID == -1 ) {
                        // Start image retrieval
                        qDebug() << "returning dummy image for album: " << album;
                        // Return dummy for the time being
                        return DUMMY_ALBUMIMAGE;
                    } else if (imageID == -2 ) {
                        qDebug() << "returning dummy image for blacklisted album: " << album;
                        return DUMMY_ALBUMIMAGE;
                    }
                    else {
                        qDebug() << "returning database image for album: " << album;
                        QString url = "image://imagedbprovider/albumid/" + QString::number(imageID);
                        return url;
                    }
                }
    }
    if ( role == LengthFormattedRole ) {
        int length = mSparqlModel->data(index,DurationRole).toInt();
        return formatLength(length);
    }
    return "";
}

/**
 * This returns an pseudo item in an variant map for SectionScroller
 * @brief AlbumsModel::get
 * @param row
 * @return
 */
QVariantMap AlbumsModel::get(int row){
    QHash<int,QByteArray> roles = roleNames();
    QHashIterator<int, QByteArray> i(roles);
    QVariantMap res;
    while (i.hasNext()) {
        i.next();
        QModelIndex idx = index(row, 0);
        QVariant data = idx.data(i.key());
        res[i.value()] = data;
    }
    return res;
}


void AlbumsModel::setDownloadEnabled(bool enabled)
{
    mDownloadEnabled = enabled;
}

void AlbumsModel::setDownloadSize(QString size)
{
    mDownloadSize = size;
    mDownloader->setDownloadSize(mDownloadSize);
}


void AlbumsModel::albumInformationReady(AlbumInformation *info)
{
    if( info == 0 ) {
        return;
    }

    emit requestDBEnter(info);

}

void AlbumsModel::albumEntered(QString albumName)
{
    qDebug() << "received new information for album: " << albumName;


    // Search for it in entries
    for ( int i = 0; i < rowCount(); i++ ) {
        QString album = mSparqlModel->data(index(i),AlbumRole).toString();
        qDebug() << "Check album: " << album;
        // Found corresponding albumObj, update coverimage url
        if (album == albumName) {
            qDebug() << "Found it at row: " << i;
            emit dataChanged(index(i),index(i));
        }
    }
}

void AlbumsModel::clearData()
{
    mDownloader->clearDownloadQueue();
    disconnect(this,SIGNAL(requestAlbumInformation(Albumtype)),mDownloader,SLOT(requestAlbumArt(Albumtype)));
    disconnect(mDownloader,SIGNAL(albumInformationReady(AlbumInformation*)),this,SLOT(albumInformationReady(AlbumInformation*)));
    disconnect(this,SIGNAL(requestDBEnter(AlbumInformation*)),mDB,SLOT(enterAlbumInformation(AlbumInformation*)));
    disconnect(mDB,SIGNAL(albumEntered(QString)),this,SLOT(albumEntered(QString)));
    beginResetModel();
    mSparqlModel->clear();
    endResetModel();
}

QString AlbumsModel::formatLength(const int seconds) const
{
    QString temp;
    int hours=0,min=0,sec=0;
    hours = seconds/3600;
    if(hours>0)
    {
        min=(seconds-(3600*hours))/60;
    }
    else{
        min=seconds/60;
    }
    sec = seconds-hours*3600-min*60;
    if(hours==0)
    {
        temp=(min<10?"0":"")+QString::number(min)+":"+(sec<10?"0":"")+QString::number(sec);
    }
    else
    {
        temp=(hours<10?"0":"")+QString::number(hours)+":"+(min<10?"0":"")+QString::number(min)+":"+(sec<10?"0":"")+QString::number(sec);
    }
    return temp;
}
