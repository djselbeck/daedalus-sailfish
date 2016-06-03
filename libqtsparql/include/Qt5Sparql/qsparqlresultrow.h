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

#ifndef QSPARQLRESULTROW_H
#define QSPARQLRESULTROW_H

#include "qsparql.h"

#include <QtCore/qstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Sparql)

class QSparqlBinding;
class QStringList;
class QVariant;
class QSparqlResultRowPrivate;

class Q_SPARQL_EXPORT QSparqlResultRow
{
public:
    QSparqlResultRow();
    QSparqlResultRow(const QSparqlResultRow& other);
    QSparqlResultRow& operator=(const QSparqlResultRow& other);
    ~QSparqlResultRow();

    bool operator==(const QSparqlResultRow &other) const;
    inline bool operator!=(const QSparqlResultRow &other) const { return !operator==(other); }

    int indexOf(const QString &name) const;
    QString variableName(int i) const;

    QSparqlBinding binding(int i) const;
    QSparqlBinding binding(const QString &name) const;
    QVariant value(int i) const;
    QVariant value(const QString &name) const;

    void append(const QSparqlBinding& binding);

    bool isEmpty() const;
    bool contains(const QString& name) const;
    void clear();
    void clearValues();
    int count() const;

private:
    void detach();
    QSparqlResultRowPrivate* d;
};

#ifndef QT_NO_DEBUG_STREAM
Q_SPARQL_EXPORT QDebug operator<<(QDebug, const QSparqlResultRow &);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSPARQLRESULTROW_H
