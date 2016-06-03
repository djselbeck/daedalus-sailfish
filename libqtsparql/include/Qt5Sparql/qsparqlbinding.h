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

#ifndef QSPARQLBINDING_H
#define QSPARQLBINDING_H

#include "qsparql.h"

#include <QtCore/qvariant.h>
#include <QtCore/qstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Sparql)

class QSparqlBindingPrivate;

class Q_SPARQL_EXPORT QSparqlBinding
{
public:
    QSparqlBinding(const QString& name = QString());
    QSparqlBinding(const QString& name, const QVariant& value);

    QSparqlBinding(const QSparqlBinding& other);
    QSparqlBinding& operator=(const QSparqlBinding& other);
    bool operator==(const QSparqlBinding& other) const;
    inline bool operator!=(const QSparqlBinding &other) const { return !operator==(other); }
    ~QSparqlBinding();

    void setValue(const QString& value, const QUrl& dataTypeUri);
    void setValue(const QVariant& value);
    void setBlankNodeLabel(const QString& label);
    inline QVariant value() const
    { return val; }
    void setName(const QString& name);
    QString name() const;
    bool isUri() const;
    bool isLiteral() const;
    bool isBlank() const;
    void clear();
    QUrl dataTypeUri() const;
    QString toString() const;

    void setDataTypeUri(const QUrl& datatype);
    void setLanguageTag(const QString& lang);

    QString languageTag() const;
    bool isValid() const;

private:
    void detach();
    QVariant val;
    QSparqlBindingPrivate* d;
};

#ifndef QT_NO_DEBUG_STREAM
Q_SPARQL_EXPORT QDebug operator<<(QDebug, const QSparqlBinding &);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSPARQLBINDING_H
