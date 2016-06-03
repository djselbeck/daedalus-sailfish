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

#ifndef QSPARQLRESULT_H
#define QSPARQLRESULT_H

#include <qsparqlresultrow.h>
#include <qsparqlquery.h>

#include <QtCore/qvariant.h>
#include <QtCore/qobject.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Sparql)

class QString;
class QSparqlError;
class QSparqlResultPrivate;

class Q_SPARQL_EXPORT QSparqlResult : public QObject
{
    Q_OBJECT
    friend class QSparqlResultPrivate;
    friend class QSparqlConnection;
    friend class QSparqlConnectionPrivate;

public:
    enum Feature { QuerySize, ForwardOnly, Sync } ;
    virtual ~QSparqlResult();

    // Iterating through the result set
    int pos() const;
    virtual bool setPos(int pos);
    virtual bool next();
    virtual bool previous();
    virtual bool first();
    virtual bool last();
    virtual int size() const;
    bool isValid() const; // valid = positioned on a valid row
    // TODO: decide what should be the pos() of the result when the data has
    // arrived; options: 1) pos() == BeforeFirstRow (like now), 2) pos() == 0
    // (the first row), but what if there's no data? pos() == AfterLastRow ?

    // Retrieving data
    virtual QSparqlResultRow current() const = 0;
    // Values from the current row
    virtual QSparqlBinding binding(int i) const = 0;
    virtual QVariant value(int i) const = 0;
    virtual QString stringValue(int i) const;

    // For ASK results
    bool boolValue() const;

    // Asynchronous operations
    virtual void waitForFinished();
    virtual bool isFinished() const;

    bool hasError() const;
    QSparqlError lastError() const;

    QString query() const;
    QSparqlQuery::StatementType statementType() const;

    bool isTable() const;
    bool isGraph() const;
    bool isBool() const;

    virtual bool hasFeature(QSparqlResult::Feature feature) const;

Q_SIGNALS:
    void dataReady(int totalCount);
    void finished();

protected:
    QSparqlResult();

    void setQuery(const QString & query);
    void setStatementType(QSparqlQuery::StatementType type);
    void setLastError(const QSparqlError& e);
    void setBoolValue(bool v);

    void updatePos(int pos); // used by subclasses for managing the position
private:
    QSparqlResultPrivate* d;

private:
    Q_DISABLE_COPY(QSparqlResult)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSPARQLRESULT_H
