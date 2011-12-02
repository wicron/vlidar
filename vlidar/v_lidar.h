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

#ifndef __V_LIDAR_H
#define __V_LIDAR_H

#include <QString>
#include <QVector>
#include <QPointF>

#include <urg/UrgCtrl.h>

class VLidar
{
public:
    bool connect(const QString &filename, long baudrate);
    void disconnect();
    bool connected();

    bool update();

    const QVector<double>& angles() const;
    const QVector<double>& distances() const;
    const QVector<QPointF>& cartesian() const;

private:
    QVector<double> m_angles;
    QVector<double> m_distances;
    QVector<QPointF> m_cartesian;
    std::vector<long> m_data;


    qrk::UrgCtrl m_urgDevice;
    bool m_connected;
};

#endif /* __V_LIDAR_H */
