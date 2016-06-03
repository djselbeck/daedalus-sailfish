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

#ifndef QSPARQLQUERYOPTIONS_H
#define QSPARQLQUERYOPTIONS_H

#include <qsparql.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Sparql)

class QSparqlQueryOptionsPrivate;

class Q_SPARQL_EXPORT QSparqlQueryOptions
{
public:
    QSparqlQueryOptions();
    ~QSparqlQueryOptions();

    QSparqlQueryOptions(const QSparqlQueryOptions& other);
    QSparqlQueryOptions& operator=(const QSparqlQueryOptions& other);
    bool operator==(const QSparqlQueryOptions &other) const;

    enum ExecutionMethod {
        AsyncExec = 1,
        SyncExec  = 2
    };

    void setExecutionMethod(ExecutionMethod em);
    void setForwardOnly(bool value);
    bool isForwardOnly() const;
    void setFireAndForget(bool value);
    bool isFireAndForget() const;
    ExecutionMethod executionMethod() const;

    enum Priority {
        HighPriority =  -10,
        NormalPriority =  0,
        LowPriority    = 10
    };

    void setPriority(Priority p);
    Priority priority() const;

private:
    QSharedDataPointer<QSparqlQueryOptionsPrivate> d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSparqlQueryOptions_H
