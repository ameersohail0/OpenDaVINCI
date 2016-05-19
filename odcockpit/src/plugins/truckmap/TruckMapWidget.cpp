/**
 * cockpit - Visualization environment
 * Copyright (C) 2012 - 2016 Christian Berger
 * Copyright (C) 2008 - 2011 (as monitor component) Christian Berger, Bernhard Rumpe
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <QtCore>
#include <QtGui>

#include <cmath>
#include <iostream>
#include <sstream>

#include "opendavinci/odcore/opendavinci.h"
#include "opendavinci/odcore/base/KeyValueConfiguration.h"
#include "opendavinci/odcore/base/Lock.h"
#include "opendavinci/odcore/data/Container.h"
#include "opendavinci/odcore/strings/StringComparator.h"
#include "opendlv/data/environment/Point3.h"
#include "plugins/truckmap/TruckMapWidget.h"

namespace cockpit { namespace plugins { class PlugIn; } }

namespace cockpit {

    namespace plugins {

        namespace truckmap {

            using namespace std;
            using namespace odcore::base;
            using namespace odcore::data;
            using namespace opendlv::data::environment;

            TruckMapWidget::TruckMapWidget(const PlugIn &/*plugIn*/, const odcore::base::KeyValueConfiguration &kvc, QWidget *prnt) :
                QWidget(prnt),
                m_timer(NULL),
                m_scaleFactorMutex(),
                m_scaleFactor(0.0125),
                m_rotation(90),
                m_sensorBoardDataMutex(),
                m_sensorBoardData() {

                // Unused configuration so far.
                (void)kvc;

                // Timer for sending data regularly.
                m_timer = new QTimer(this);
                connect(m_timer, SIGNAL(timeout()), this, SLOT(repaint()));
                m_timer->start(200);
            }

            TruckMapWidget::~TruckMapWidget() {}

            void TruckMapWidget::stopTimer() {
                m_timer->stop();
            }

            void TruckMapWidget::nextContainer(Container &c) {
                if (c.getDataType() == automotive::miniature::SensorBoardData::ID()) {
                    Lock l(m_sensorBoardDataMutex);
                    m_sensorBoardData = c.getData<automotive::miniature::SensorBoardData>();
                }
            }

            void TruckMapWidget::setScale(const int &val) {
                if (val > 0) {
                    Lock l(m_scaleFactorMutex);
                    m_scaleFactor = (val / 400.0);
                }
            }

            void TruckMapWidget::paintEvent(QPaintEvent *evt) {
                Lock l(m_scaleFactorMutex);

                QPainter painter(this);
                painter.setRenderHint(QPainter::Antialiasing);

                const double scaleMax = 5;
                const double scaleMin = 1e-4;
                const double offsetViewMaxFactor = 8;

                if (m_scaleFactor < scaleMin) {
                    m_scaleFactor = scaleMin;
                } else if (m_scaleFactor > scaleMax) {
                    m_scaleFactor = scaleMax;
                }

                // White background.
                painter.fillRect(evt->rect(), QBrush(Qt::white));

                // Variables for displaying labels on the map.
                QString description;
                QPointF pt_description;
                QRectF rect_description;
                QFont fontSettings = this->font();

                QPen pen;

                // Map coordinate system transformation according to DIN 70000: x = 12am, y = 9am --> m_rotation = +90 (http://www.isupia.de/download/Fahrdynamische%20Größen.pdf)
                QTransform transformationDIN70000;
                transformationDIN70000.translate(evt->rect().width() / 2, evt->rect().height() / 2);
                transformationDIN70000.scale(m_scaleFactor, -m_scaleFactor);
                transformationDIN70000.rotate(m_rotation);

                // Transformation into the regular coordinate system.
                QTransform transformationCartesianCoordinateSystem;
                transformationCartesianCoordinateSystem.translate(evt->rect().width() / 2, evt->rect().height() / 2);
                transformationCartesianCoordinateSystem.scale(m_scaleFactor, -m_scaleFactor);
                transformationCartesianCoordinateSystem.rotate(m_rotation - 90);

                // Transformation for descriptions label in image coordinates.
                QTransform descriptionTrans;
                descriptionTrans.translate(0, 0);
                descriptionTrans.scale(1, 1);
                descriptionTrans.rotate(0);

                // Setup fonts.
                fontSettings.setPointSize(10);
                painter.setFont(fontSettings);

                // Setup axes parameters.
                const double scaleMultiplier = ceil((1 / ((m_scaleFactor * 100) / 50)));
                const double step = 100 * scaleMultiplier;
                const double zeroAxisWidth = 3;
                const QColor zeroAxisColor = Qt::black;
                const QColor gridAxisColor = Qt::gray;
                const QColor textColor = QPalette::Foreground;
                const double xStepFact = ceil(width() * offsetViewMaxFactor / step / m_scaleFactor);
                const double yStepFact = ceil(height() * offsetViewMaxFactor / step / m_scaleFactor);

                // Draw X-axes.
                for (double i = -yStepFact * step;i < yStepFact * step;i += step) {
                    if ((int)(i / step) % 2) {
                        painter.setPen(gridAxisColor);
                    }
                    else {
                        description.sprintf("%.0f", i/1000.0);

                        pt_description.setY(i);
                        painter.setTransform(descriptionTrans);
                        pt_description = transformationCartesianCoordinateSystem.map(pt_description);
                        pt_description.setX(10);
                        pt_description.setY(pt_description.y() - 5);

                        painter.setPen(QPen(textColor));
                        painter.drawText(pt_description, description);

                        if (fabs(i) < 1e-5) {
                            pen.setWidthF(zeroAxisWidth / m_scaleFactor);
                            pen.setColor(zeroAxisColor);
                        }
                        else {
                            pen.setWidth(0);
                        }
                        painter.setPen(pen);
                    }
                    painter.setTransform(transformationDIN70000);
                    painter.drawLine(-xStepFact * step, i, xStepFact * step, i);
                }

                // Draw Y-axis segments
                for (double i = -xStepFact * step;i < xStepFact * step;i += step) {
                    if ((int)(i / step) % 2) {
                        painter.setPen(gridAxisColor);
                    }
                    else {
                        description.sprintf("%.0f", i/1000.0*-1);

                        pt_description.setX(i);
                        pt_description.setY(0);
                        painter.setTransform(descriptionTrans);
                        pt_description = transformationCartesianCoordinateSystem.map(pt_description);
                        pt_description.setX(pt_description.x() + 5);
                        pt_description.setY(height() - 10);

                        painter.setPen(QPen(textColor));
                        painter.drawText(pt_description, description);

                        if (fabs(i) < 1e-5) {
                            pen.setWidthF(zeroAxisWidth / m_scaleFactor);
                            pen.setColor(zeroAxisColor);
                        }
                        else {
                            pen.setWidth(0);
                        }
                        painter.setPen(pen);
                    }

                    painter.setTransform(transformationDIN70000);
                    painter.drawLine(i, -yStepFact * step, i, yStepFact * step);
                }

                // Draw points.
                {
                    for (uint16_t i = 0; i < 3; i++) {
                        const double d = m_sensorBoardData.getValueForKey_MapOfDistances(i);
                        if (d > 0) {
                            double totalRot = 0 * cartesian::Constants::DEG2RAD;

                            Point3 measurementPoint(d, 0, 0);
                            measurementPoint.rotateZ(totalRot);
//                            measurementPoint += m_translation;

                            int _width = 200 * (m_scaleFactor * 300);
                            int _height = 200 * (m_scaleFactor * 300);

                            painter.fillRect(measurementPoint.getX() * 1000, measurementPoint.getY() * 1000, _width, _height, QBrush(Qt::red));
                        }
                    }
                }

                // Draw axes labels.
                pen.setColor(zeroAxisColor);
                painter.setPen(pen);
                painter.setTransform(descriptionTrans);
                fontSettings.setPointSize(10);
                painter.setFont(fontSettings);
                description = "x [m]";
                painter.drawText(evt->rect().width() / 2 + 15, 15, description);

                description = "y [m]";
                painter.drawText(15, evt->rect().height() / 2 + 15, description);

                // Arrows for the axes.
                pen.setWidthF(zeroAxisWidth);
                pen.setColor(zeroAxisColor);
                painter.setPen(pen);
                // Arrow X axis.
                painter.drawLine(evt->rect().width() / 2, 0, evt->rect().width() / 2 - 15, 15);
                painter.drawLine(evt->rect().width() / 2, 0, evt->rect().width() / 2 + 15, 15);
                // Arrow Y axis.
                painter.drawLine(0, evt->rect().height() / 2, 15, evt->rect().height() / 2 - 15);
                painter.drawLine(0, evt->rect().height() / 2, 15, evt->rect().height() / 2 + 15);

                painter.end();
            }

        }
    }
}

