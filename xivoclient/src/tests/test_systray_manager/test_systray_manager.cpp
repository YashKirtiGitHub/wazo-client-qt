/* XiVO Client
 * Copyright (C) 2007-2012, Avencall
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

#include <QtTest/QtTest>
#include <QObject>
#include <QIcon>
#include <QSystemTrayIcon>

#include <gmock/gmock.h>

#include <systray_manager.h>
#include <systray_icon_manager.h>

using namespace testing;

class MockSystrayIconManager: public SystrayIconManager
{
    public:
        MOCK_CONST_METHOD1(get_systray_icon, QIcon(SystrayIcon));
};

class MockQSystemTrayIcon: public QSystemTrayIcon
{
    public:
        MOCK_METHOD1(setIcon, void(QIcon));
};

class TestSystrayManager: public QObject
{
    Q_OBJECT

    private slots:
    void testChangeIcon()
    {
        MockSystrayIconManager mock_systray_icon_manager;
        MockQSystemTrayIcon mock_qt_system_tray_icon;
        SystrayManager systray_manager(mock_systray_icon_manager,
                                       mock_qt_system_tray_icon);
        SystrayIcon new_systray_icon_id = icon_unlogged;
        QIcon new_systray_icon;
        ON_CALL(mock_systray_icon_manager, get_systray_icon(Eq(new_systray_icon_id)))
            .WillByDefault(Return(new_systray_icon));
        EXPECT_CALL(mock_systray_icon_manager, get_systray_icon(Eq(new_systray_icon_id)))
            .Times(1);
        EXPECT_CALL(mock_qt_system_tray_icon, setIcon(_))
            .Times(1);

        systray_manager.change_icon(new_systray_icon_id);
    }
};

int main (int argc, char *argv[])
{
    ::testing::GTEST_FLAG(throw_on_failure) = true;
    ::testing::InitGoogleMock(&argc, argv);
    TestSystrayManager test;
    QTest::qExec(&test, argc, argv);
}

#include <test_systray_manager.moc>
