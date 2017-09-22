/*
 * Copyright 2007-2017 The Wazo Authors  (see the AUTHORS file)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with a Section 7 Additional
 * Permission as follows:
 *   This notice constitutes a grant of such permission as is necessary
 *   to combine or link this software, or a modified version of it, with
 *   the OpenSSL project's "OpenSSL" library, or a derivative work of it,
 *   and to copy, modify, and distribute the resulting work. This is an
 *   extension of the special permission given by Trolltech to link the
 *   Qt code with the OpenSSL library (see
 *   <http://doc.trolltech.com/4.4/gpl.html>). The OpenSSL library is
 *   licensed under a dual license: the OpenSSL License and the original
 *   SSLeay license.
 *
 * This programm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __PEOPLE_ENTRY_SORT_FILTER_PROXY_MODEL_H__
#define __PEOPLE_ENTRY_SORT_FILTER_PROXY_MODEL_H__

#include <QModelIndex>
#include <QString>

#include <xletlib/abstract_sort_filter_proxy_model.h>
#include "people_enum.h"

class PeopleEntrySortFilterProxyModel : public AbstractSortFilterProxyModel
{
    Q_OBJECT

    public:
        PeopleEntrySortFilterProxyModel(QObject *parent);
        void setFilterMode(PeopleMode mode);

    protected:
        virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
        virtual bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const;

    private:
        PeopleMode m_mode;
};

#endif