/*
 *  Copyright (c) 2011 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "v_lidar.h"

bool VLidar::connect(const QString &fileName, long baudrate)
{
    if(!m_urgDevice.connect(fileName.toAscii().data(), baudrate))
        return false;

    m_urgDevice.setCaptureMode(qrk::ManualCapture);

    int size = m_urgDevice.maxScanLines();
    m_angles.resize(size);
    m_distances.resize(size);
    m_cartesian.resize(size);

    for(int i = 0; i < size; i++) {
        m_angles[i] = m_urgDevice.index2rad(i);
    }

    return true;
}

void VLidar::disconnect()
{
    m_urgDevice.disconnect();
}

bool VLidar::connected()
{
    return m_urgDevice.connection();
}

bool VLidar::update()
{
    if(!m_urgDevice.capture(m_data)) return false;

    Q_ASSERT(m_data.size() == m_angles.size());

    for(int i = 0; i < m_data.size(); i++) {
        m_distances[i] = qreal(m_data[i]) / 1000.0;
        m_cartesian[i] = QPointF(m_distances[i] * cos(m_angles[i]),
                                 m_distances[i] * sin(m_angles[i]));
    }

    return true;
}

const QVector<double>& VLidar::angles() const
{
    return m_angles;
}

const QVector<double>& VLidar::distances() const
{
    return m_distances;
}

const QVector<QPointF>& VLidar::cartesian() const
{
    return m_cartesian;
}

