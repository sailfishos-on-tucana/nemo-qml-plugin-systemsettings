/*
 * Copyright (C) 2013 Jolla Ltd. <pekka.vuorela@jollamobile.com>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nemo Mobile nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#include <QObject>
#include <QSettings>
#include <QProcess>
#include <QDebug>
#include "devicelockiface.h"

static bool runPlugin(QStringList args)
{
    QSettings s("/usr/share/lipstick/devicelock/devicelock.conf", QSettings::IniFormat);
    QString pluginName = s.value("DeviceLock/pluginName").toString();

    if (pluginName.isEmpty()) {
        qWarning("DeviceLock: no plugin configuration set in /usr/share/lipstick/devicelock/devicelock.conf");
        return false;
    }

    QProcess p;
    p.start(pluginName, args);
    if (!p.waitForFinished()) {
        qWarning("DeviceLock: plugin did not finish in time");
        return false;
    }

    qDebug() << p.readAllStandardOutput();
    qWarning() << p.readAllStandardError();
    return p.exitCode() == 0;
}

DeviceLockInterface::DeviceLockInterface(QObject *parent)
    : QObject(parent),
      m_cacheRefreshNeeded(true)
{
}

DeviceLockInterface::~DeviceLockInterface()
{
}

bool DeviceLockInterface::checkCode(const QString &code)
{
    return runPlugin(QStringList() << "--check-code" << code);
}

bool DeviceLockInterface::setCode(const QString &oldCode, const QString &newCode)
{
    bool return_value = runPlugin(QStringList() << "--set-code" << oldCode << newCode);
    if (return_value) {
        m_cacheRefreshNeeded = true;
        emit isSetChanged();
    }
    return return_value;
}

bool DeviceLockInterface::clearCode(const QString &currentCode)
{
    bool return_value = runPlugin(QStringList() << "--clear-code" << currentCode);
    if (return_value) {
        m_cacheRefreshNeeded = true;
        emit isSetChanged();
    }
    return return_value;
}

bool DeviceLockInterface::isSet() {
    if (m_cacheRefreshNeeded) {
        m_codeSet = runPlugin(QStringList() << "--is-set" << "lockcode");
        m_cacheRefreshNeeded = false;
    }
    return m_codeSet;
}

void DeviceLockInterface::refresh()
{
    bool wasCodeSet = m_codeSet;
    m_codeSet = runPlugin(QStringList() << "--is-set" << "lockcode");
    m_cacheRefreshNeeded = false;

    if (wasCodeSet != m_codeSet) {
        emit isSetChanged();
    }
}

bool DeviceLockInterface::clearDevice(const QString &code, ResetMode mode)
{
    QStringList parameters;
    parameters << "--clear-device" << code;
    if (mode == DeviceLockInterface::Reboot) {
        parameters << "--reboot";
    }
    return runPlugin(parameters);
}
