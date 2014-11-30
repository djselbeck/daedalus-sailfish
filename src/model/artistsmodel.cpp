#include "artistsmodel.h"

#include <QUrl>

ArtistsModel::ArtistsModel(QObject *parent, QSparqlConnection *connection) :
    QAbstractListModel(parent)
{
    if ( connection != NULL ) {
        mConnection = connection;
        mArtistsQueryString = "SELECT ?artist  COUNT(?album) as ?albumcount ?artistobj WHERE { ?album nmm:albumArtist ?artistobj . ?artistobj nmm:artistName ?artist } GROUP BY ?artist ORDER BY ?artist";
    }
    mSparqlModel = new QSparqlQueryModel(this);
    connect(mSparqlModel,SIGNAL(finished()),this,SLOT(sparqlModelfinished()));
}


void ArtistsModel::requestArtists()
{
    // Initialize the query
    //mSparqlModel->clear();
    qDebug() << "getting artists";
    // See qsparqlquerymodel.cpp and 4 from title, trackcount, totalduration, artistname
    mSparqlModel->setQuery(QSparqlQuery(mArtistsQueryString),*mConnection);
}

void ArtistsModel::sparqlModelfinished()
{
 //   emit dataChanged(QModelIndex().child(0,0),QModelIndex().child(mSparqlModel->rowCount(),mSparqlModel->columnCount()));
    beginResetModel();
    qDebug() << "underlaying model finished result fetching";
    emit artistsReady();
    endResetModel();
}


QHash<int, QByteArray> ArtistsModel::roleNames() const {
    QHash<int,QByteArray> roles;
    roles[NameRole] = "artist";
    roles[AlbumCountRole] = "albumcount";
    roles[ArtistURNRole] = "artisturn";
    roles[SectionRole] = "sectionprop";
    roles[ImageURLRole] = "imageURL";
    return roles;
}

int ArtistsModel::rowCount(const QModelIndex &parent) const {
    return mSparqlModel->rowCount(parent);
}

QVariant ArtistsModel::data(const QModelIndex &index, int role) const {
    QString artistName;
    QUrl urn;
    switch ( role ) {
    case NameRole:
        return mSparqlModel->data(index,role);
        break;
    case ArtistURNRole:
        urn = mSparqlModel->data(index,role).toUrl();
        return urn.toEncoded();
    case SectionRole:
        artistName = mSparqlModel->data(index,NameRole).toString();
        return QString(artistName.at(0)).toUpper();
        break;
    case AlbumCountRole:

        break;
    case ImageURLRole:

        break;
    default:
        return "";
    }

    return "";
}

/**
 * This returns an pseudo item in an variant map for SectionScroller
 * @brief ArtistsModel::get
 * @param row
 * @return
 */
QVariantMap ArtistsModel::get(int row){
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
