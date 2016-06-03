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

#ifndef QSPARQLERROR_H
#define QSPARQLERROR_H

#include <qsparql.h>

#include <QtCore/qstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Sparql)

// TODO: how to differentiate between fatal and non-fatal errors. (Fatal =
// doesn't make sense to try to exec() anything ever, non-fatal: exec:in the
// next query, or the same query again, makes sense.)

class QString;

class Q_SPARQL_EXPORT QSparqlError
{
public:
    enum ErrorType {
        NoError,
        ConnectionError,
        StatementError, // = syntax error
        TransactionError,
        BackendError, // other error sent by the backend
        UnknownError
    };
    QSparqlError(const QString& message = QString(),
                 ErrorType type = NoError,
                 int number = -1);
    QSparqlError(const QSparqlError& other);
    QSparqlError& operator=(const QSparqlError& other);
    ~QSparqlError();

    QString message() const;
    void setMessage(const QString& message);
    ErrorType type() const;
    void setType(ErrorType type);
    int number() const;
    void setNumber(int number);
    bool isValid() const;

private:
    QString errorMessage;
    ErrorType errorType;
    int errorNumber;
};

#ifndef QT_NO_DEBUG_STREAM
Q_SPARQL_EXPORT QDebug operator<<(QDebug, const QSparqlError &);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSPARQLERROR_H
