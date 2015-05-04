/* XiVO Client
 * Copyright (C) 2007-2015 Avencall
 *
 * This file is part of XiVO Client.
 *
 * XiVO Client is free software: you can redistribute it and/or modify
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
 * XiVO Client is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XiVO Client.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QList>

#include <baseengine.h>

#include "history_model.h"

HistoryModel::HistoryModel(QWidget * parent)
    : QAbstractTableModel(parent)
{
    registerListener("history");
    m_mode = OUTCALLS;
    m_history << QVariant() << QVariant() << QVariant();
    connect(b_engine, SIGNAL(settingsChanged()),
            this, SLOT(requestHistory()));
}

void HistoryModel::parseCommand(const QVariantMap &map) {
    updateHistory(map);
}

int HistoryModel::rowCount(const QModelIndex&) const
{
    return m_history[m_mode].toList().count();
}

int HistoryModel::columnCount(const QModelIndex&) const
{
    if ((m_mode == OUTCALLS) || (m_mode == INCALLS))
        return 3;
    else if (m_mode == MISSEDCALLS)
        return 2;

    return 0;
}

QVariant HistoryModel::data(const QModelIndex &a, int role) const
{
    int row, column; row = a.row(); column = a.column();
    QVariantList histlist = m_history[m_mode].toList();

    if (role == Qt::DisplayRole) {
        if (((histlist.count()) &&
             ((histlist).value(row).toMap().count()))) {
            if (column == 0) {
                return histlist.value(row).toMap().value("fullname");
            } else if (column == 1) {
                QString qsd = histlist.value(row).toMap().value("calldate").toString();
                QDateTime qdt = QDateTime::fromString(qsd, Qt::ISODate);
                return qdt.toString(QString("dd/MM/yyyy  HH:mm:ss"));
            } else if (column == 2) {
                int duration = histlist.value(row).toMap().value("duration").toInt();
                int sec =   ( duration % 60);
                int min =   ( duration - sec ) / 60 % 60;
                int hou = ( ( duration - sec - min * 60 ) / 60 ) / 60;
                if (hou)
                    return tr("%1 hr %2 min %3 s").arg(hou).arg(min).arg(sec);
                else if (min)
                    return tr("%1 min %2 s").arg(min).arg(sec);
                else
                    return tr("%1 s").arg(sec);
            }
        }
    } else if (role == Qt::UserRole) {
        if (column == 1) {
            QString call_datetime = histlist.value(row).toMap().value("calldate").toString();
            return QDateTime::fromString(call_datetime, Qt::ISODate);
        }
        if (column == 2) {
            return histlist.value(row).toMap().value("duration").toInt();
        }
    }

    return QVariant();
}

void HistoryModel::updateHistory(const QVariantMap &p)
{
    int mode = p.value("mode").toInt();
    QVariantList h = p.value("history").toList();
    beginResetModel();
    if (mode == m_mode)
        m_history[m_mode] = h;
    endResetModel();
}

void HistoryModel::requestHistory(HistoryMode mode, QString xuserid)
{
    if (mode == DEFAULT) {
        mode = m_mode;
    }
    if (xuserid.isEmpty()) {
        xuserid = b_engine->getFullId();
    }
    if (mode == OUTCALLS || mode == INCALLS || mode == MISSEDCALLS) {
        QVariantMap command;
        command["class"] = "history";
        command["xuserid"] = xuserid;
        command["size"] = QString::number(b_engine->getConfig("historysize").toUInt());
        command["mode"] = QString::number(mode);
        b_engine->sendJsonCommand(command);
    }
}

void HistoryModel::missedCallMode()
{
    m_mode = MISSEDCALLS;
    requestHistory(m_mode);
    beginResetModel();
    emit headerDataChanged(Qt::Horizontal, 0, 3);
    endResetModel();
}

void HistoryModel::receivedCallMode()
{
    m_mode = INCALLS;
    requestHistory(m_mode);
    beginResetModel();
    emit headerDataChanged(Qt::Horizontal, 0, 3);
    endResetModel();
}

void HistoryModel::sentCallMode()
{
    m_mode = OUTCALLS;
    requestHistory(m_mode);
    beginResetModel();
    emit headerDataChanged(Qt::Horizontal, 0, 3);
    endResetModel();
}

QVariant HistoryModel::headerData(int section,
                                    Qt::Orientation orientation,
                                    int role = Qt::DisplayRole) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section == 0)
            return QVariant(tr("Number"));
        else if (section == 1)
            return QVariant(tr("Date"));

        if ((section == 2) && ((m_mode == OUTCALLS) || (m_mode == INCALLS)))
            return QVariant(tr("Duration"));
    }

    return QVariant();
}