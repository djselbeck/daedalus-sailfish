#ifndef ARTISTSMODEL_H
#define ARTISTSMODEL_H

#include <QAbstractListModel>
#include <Qt5Sparql/QSparqlQueryModel>
#include <QThread>

#include <metadata/imagedatabase.h>
#include <global.h>

class ArtistsModel : public QAbstractListModel
{
    Q_OBJECT    
    Q_PROPERTY(int count READ rowCount)

public:
    explicit ArtistsModel(QObject *parent = 0, QSparqlConnection *connection = 0, QThread *fetchthread = 0, ImageDatabase *db = 0);

    enum EntryRoles {
        NameRole = Qt::UserRole + 1,
        AlbumCountRole,
        ArtistURNRole,
        SectionRole,
        ImageURLRole
    };

    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    Q_INVOKABLE QHash<int, QByteArray> roleNames() const;
    Q_INVOKABLE QVariantMap get(int row);


signals:
    void artistsReady();

public slots:
    void requestArtists();

private slots:
    void sparqlModelfinished();


private:
    QString mArtistsQueryString;
    QSparqlConnection *mConnection;
    QSparqlQueryModel *mSparqlModel;
    QThread *mThread;
    ImageDatabase *mImgDB;


};

#endif // ARTISTSMODEL_H
