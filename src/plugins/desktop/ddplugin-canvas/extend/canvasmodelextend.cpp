/*
 * Copyright (C) 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     zhangyu<zhangyub@uniontech.com>
 *
 * Maintainer: zhangyu<zhangyub@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "canvasmodelextend_p.h"
#include "model/canvasproxymodel.h"

#include <QVariant>

// register type for EventSequenceManager
Q_DECLARE_METATYPE(QVariant *)

DDP_CANVAS_USE_NAMESPACE
DSB_D_USE_NAMESPACE

CanvasModelExtendPrivate::CanvasModelExtendPrivate(CanvasModelExtend *qq)
    : QObject(qq)
    , CanvasEventProvider()
    , q(qq)
{
    qRegisterMetaType<QVariant *>();
}

CanvasModelExtendPrivate::~CanvasModelExtendPrivate()
{

}

void CanvasModelExtendPrivate::registerEvent()
{
    RegCanvasSeqSigID(this, kFilterCanvasModelData);
}

CanvasModelExtend::CanvasModelExtend(QObject *parent)
    : QObject(parent)
    , ModelExtendInterface()
    , d(new CanvasModelExtendPrivate(this))
{

}

bool CanvasModelExtend::init()
{
    return d->initEvent();
}

bool CanvasModelExtend::modelData(const QUrl &url, int role, QVariant *out, void *userData) const
{
    return dpfInstance.eventSequence().run(GetCanvasSeqSigID(d, kFilterCanvasModelData),
                                    url, role, out, userData);
}
