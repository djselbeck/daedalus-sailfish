/***************************************************************************/
/**
** @copyright Copyright (C) 2010-2011 Nokia Corporation and/or its subsidiary(-ies).
**
** @license Commercial Qt/LGPL 2.1 with Nokia exception/GPL 3.0
**
**
** Copyright (C) 2010-2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (ivan.frade@nokia.com)
**
** This file is part of the QtSparql module (not yet part of the Qt Toolkit).
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at ivan.frade@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSPARQLCONNECTIONOPTIONS_H
#define QSPARQLCONNECTIONOPTIONS_H

#include <qsparql.h>

#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>
#include <QtCore/qshareddata.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Sparql)

class QSparqlConnectionOptionsPrivate;
class QNetworkProxy;
class QNetworkAccessManager;

class Q_SPARQL_EXPORT QSparqlConnectionOptions
{
public:
    QSparqlConnectionOptions();
    ~QSparqlConnectionOptions();

    QSparqlConnectionOptions(const QSparqlConnectionOptions& other);
    bool operator==(const QSparqlConnectionOptions &other) const;
    QSparqlConnectionOptions& operator=(const QSparqlConnectionOptions& other);

    void setOption(const QString& name, const QVariant& value);
    QVariant option(const QString& name) const;

    void setDatabaseName(const QString& name);
    void setUserName(const QString& name);
    void setPassword(const QString& password);
    void setHostName(const QString& host);
    void setPath(const QString& path);
    void setPort(int p);
    void setDataReadyInterval(int p);
    void setMaxThreadCount(int p);
    void setThreadExpiryTime(int p);

#ifndef QT_NO_NETWORKPROXY
    void setProxy(const QNetworkProxy& proxy);
#endif

    void setNetworkAccessManager(QNetworkAccessManager* manager);

    QString databaseName() const;
    QString userName() const;
    QString password() const;
    QString hostName() const;
    QString path() const;
    int port() const;
    int dataReadyInterval() const;
    int maxThreadCount() const;
    int threadExpiryTime() const;

#ifndef QT_NO_NETWORKPROXY
    QNetworkProxy proxy () const;
#endif

    QNetworkAccessManager* networkAccessManager() const;

private:
    QSharedDataPointer<QSparqlConnectionOptionsPrivate> d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSPARQLCONNECTIONOPTIONS_H
