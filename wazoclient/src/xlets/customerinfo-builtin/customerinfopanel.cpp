/*
 * Copyright 2007-2017 The Wazo Authors  (see the AUTHORS file)
 *
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

#include <assembler.h>

#include "customerinfopanel.h"

CustomerInfoPanel::CustomerInfoPanel(QWidget *parent)
    : XLet(parent, tr("Sheets"))
{
    connect(b_engine, SIGNAL(displayFiche(const QString &, bool, const QString &)),
            this, SLOT(displayFiche(const QString &, bool, const QString &)));

    // qDebug() << Q_FUNC_INFO;
    m_glayout = new QGridLayout(this);
    m_tabs = new QTabWidget(this);
    m_glayout->addWidget( m_tabs, 0, 0 );
    m_glayout->setRowStretch(0, 1);
    m_glayout->setColumnStretch(0, 1);

    m_tablimit = b_engine->getConfig("guioptions.sheet-tablimit").toUInt();
    m_autourl_allowed = b_engine->getConfig("guioptions.autourl_allowed").toBool();
}

/*!
 * Display the new profile in the tabbed area
 * and show a message with the systray icon
 */
void CustomerInfoPanel::showNewProfile(Popup * popup)
{
    QString opt = "";
    qDebug() << Q_FUNC_INFO << popup->callWhere()
             << popup->callIpbxId() << popup->callChannel();
    if(popup->sheetpopup()) {
        // removed the "already_popup" stuff, since it is already handled in displayFiche()
        QString currentTimeStr = QDateTime::currentDateTime().toString("hh:mm:ss");
        quint32 index = m_tabs->addTab(popup, currentTimeStr);
        qDebug() << Q_FUNC_INFO << "added tab" << index;
        m_tabs->setCurrentIndex(index);
        if (index >= m_tablimit)
            // close the first widget
            m_tabs->removeTab(0);

        // no need to focus if there is no sheet popup
        if(popup->focus())
            opt += "fp";
    }

    // tells the main widget that a new popup has arrived here
    if(popup->systraypopup())
        opt += "s";
    emit newPopup(popup->messagetitle(), popup->message(), opt);
    // set this widget to be the current tab in Wazo Client
    emit showWidgetOnTop(this);
}

void CustomerInfoPanel::popupDestroyed(QObject * obj)
{
    qDebug() << "Removing" << obj;

    foreach(Popup * mpopup, m_popups) {
        if (mpopup == obj) {
            m_popups.removeAll(mpopup);
            mpopup->deleteLater();
        }
    }
}

void CustomerInfoPanel::displayFiche(const QString & fichecontent, bool qtui, const QString & id)
{
    Popup * popup = NULL;
    for(int i = m_popups.size() - 1; i >= 0; i --) {
        if(id == m_popups[i]->id()) {
            qDebug() << Q_FUNC_INFO << "fiche id already there" << i << id;
            popup = m_popups[i];
            break;
        }
    }

    QBuffer * inputstream = new QBuffer(this);
    inputstream->open(QIODevice::ReadWrite);
    inputstream->write(fichecontent.toUtf8());
    inputstream->close();

    // Get Data and Popup the profile if ok
    if (popup == NULL) {
        popup = new Popup(m_autourl_allowed);
        m_popups.append(popup);
        popup->setId(id);
        connect(popup, SIGNAL(destroyed(QObject *)),
                this, SLOT(popupDestroyed(QObject *)));
        connect(popup, SIGNAL(wantsToBeShown(Popup *)),
                this, SLOT(showNewProfile(Popup *)));
    }

    popup->feed(inputstream, qtui);
    delete inputstream;
}

void CustomerInfoPanel::doGUIConnects(QWidget * mainwindow)
{
    connect(this, SIGNAL(newPopup(const QString &, const QHash<QString, QString> &, const QString &)),
            mainwindow, SLOT(customerInfoPopup(const QString &, const QHash<QString, QString> &, const QString &)));
    connect(this, SIGNAL(showWidgetOnTop(QWidget *)),
            assembler->xletDispatcher(), SLOT(showWidgetOnTop(QWidget *)));
}
