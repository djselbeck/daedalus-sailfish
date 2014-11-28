#ifndef ALBUMSMODEL_H
#define ALBUMSMODEL_H

#include <QObject>
#include <Qt5Sparql/QSparqlQueryModel>

class AlbumsModel : public QSparqlQueryModel
{
    Q_OBJECT
public:
    explicit AlbumsModel(QObject *parent = 0, QSparqlConnection *connection = 0);

signals:
    void albumsReady();

public slots:
    void requestAlbums();


private:
    QString mAlbumsQueryString;
    QSparqlConnection *mConnection;

};

#endif // ALBUMSMODEL_H
