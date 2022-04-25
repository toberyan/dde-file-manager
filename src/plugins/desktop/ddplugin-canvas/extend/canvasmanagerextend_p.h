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
#ifndef CANVASMANAGEREXTEND_P_H
#define CANVASMANAGEREXTEND_P_H

#include "canvasmanagerextend.h"
#include "canvaseventprovider.h"

DDP_CANVAS_BEGIN_NAMESPACE

static constexpr char kFilterCanvasManagerWallpaperSetting[] = "CanvasManager_Filter_wallpaperSetting";

class CanvasManagerExtendPrivate : public QObject, public CanvasEventProvider
{
    Q_OBJECT
public:
    explicit CanvasManagerExtendPrivate(CanvasManagerExtend *);
    ~CanvasManagerExtendPrivate() override;
protected:
    void registerEvent() override;
private:
    CanvasManagerExtend *q;
};

DDP_CANVAS_END_NAMESPACE

#endif // CANVASMANAGEREXTEND_P_H
