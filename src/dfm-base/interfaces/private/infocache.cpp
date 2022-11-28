/*
 * Copyright (C) 2021 ~ 2022 Uniontech Software Technology Co., Ltd.
 *
 * Author:     huanyu<huanyub@uniontech.com>
 *
 * Maintainer: zhengyouge<zhengyouge@uniontech.com>
 *             yanghao<yanghao@uniontech.com>
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
#include "infocache.h"
#include "interfaces/private/infocache_p.h"
#include "base/schemefactory.h"

#include <dfm-io/core/dfileinfo.h>

// 更新间隔时间
static constexpr int kRefreshSpaceTime = 500;
// 文件缓存文件的总个数
static constexpr int kCacheFileinfoCount = 100000;
// 文件缓存移除的时间
static constexpr int kCacheRemoveTime = (60 * 1000);

namespace dfmbase {
Q_GLOBAL_STATIC(InfoCache, _InfoCacheManager)

/*!
 * \class ReFreshThread
 * \brief
 * 这个类是对Url的强对应，必须保证url的唯一性
 * 一、缓存规则
 *     1.fileinfo创建时缓存它的共享指针
 *     2.刷新它的refreshtime
 * 二、刷新规则
 *     1.收到刷新信号，判断文件的refreshtime是否大于给定值500ms
 *     2.大于直接刷新
 *     3.小于不刷新，插入到待刷新队列
 *     4.开启刷新线程，每500ms遍历一次待刷新队列
 * 三、析构规则
 *     1.启动一个5分钟定时器
 *     2.每5分钟遍历缓存查询，将共享指针引用计数小于1的缓存文件信息的url加入到待析构队列
 *     3.当每次getCacheInfo时，移除在待析构队列的url
 *     4.启动一个333ms延迟于前一个计时器的定时器
 *     5.每5分钟遍历待析构队里，移除cacheinfo
 *     6.补充，当文件缓存数量操过100000，移除前面的缓存
 */

ReFreshThread::ReFreshThread(QObject *parent)
    : QThread(parent)
{
    time.start();
}
/*!
 * \brief refreshFileInfoByUrl 根据URL刷新fileinfo
 *
 * 判断fileinfo中上次更新的时间是否超过更新间隔时间，超过立即更新，不然加入待更新链表
 *
 * \param QUrl 文件的URL
 *
 * \return
 */
void ReFreshThread::refreshFileInfoByUrl(const QUrl &url)
{
    AbstractFileInfoPointer fileinfo = InfoCache::instance().getCacheInfo(url);
    if (!fileinfo) {
        if (needRefreshMap.contains(url))
            removedCacheList.push_back(url);
        refreshMap.remove(url);
        return;
    }
    qint64 reflreshTime = refreshMap.value(url);
    if (time.elapsed() - reflreshTime > 500) {
        updateRefreshTimeByUrl(url);
        fileinfo->refresh();
    } else {
        needRefreshMap.insert(url, reflreshTime);
    }
}
/*!
 * \brief updateRefreshTimeByUrl 根据URL更新文件的刷新事件
 *
 * 判断上次更新的时间是否超过更新间隔时,跟新刷新时间，不然加入待更map
 *
 * \param QUrl 文件的URL
 *
 * \return
 */
void ReFreshThread::updateRefreshTimeByUrl(const QUrl &url)
{
    refreshMap.insert(url, time.elapsed());
}
/*!
 * \brief removeRefreshByUrl 根据URL移除更新
 *
 * 移除更新时间信息和待更新信息，一般是在移除fileinfo，cache析构的时候
 *
 * \param QUrl 文件的URL
 *
 * \return
 */
void ReFreshThread::removeRefreshByUrl(const QUrl &url)
{
    refreshMap.remove(url);
    if (needRefreshMap.contains(url)) {
        removedCacheList.push_back(url);
    }
}
/*!
 * \brief stopRefresh 停止更新线程
 *
 * 设置为停止状态，并且阻塞等待线程结束
 *
 * \return
 */
void ReFreshThread::stopRefresh()
{
    bStop = true;
    wait();
}
/*!
 * \brief run 线程执行函数
 *
 * 固定时间去遍历待更新list，判断fileinfo中上次更新的时间是否超过更新间隔时间，
 *
 * 超过立即更新，否则退出当前遍历，线程沉睡更新间隔时间
 *
 * \return
 */
void ReFreshThread::run()
{
    if (bStop)
        return;
    QMap<QUrl, qint64>::iterator itr = needRefreshMap.begin();
    QMap<QUrl, qint64>::iterator itrEnd = needRefreshMap.end();
    while (itr != itrEnd) {
        AbstractFileInfoPointer fileinfo = InfoCache::instance().getCacheInfo(itr.key());
        if (!fileinfo)
            continue;
        if (bStop)
            return;
        if (removedCacheList.contains(itr.key())) {
            removedCacheList.removeAll(itr.key());
            itr = needRefreshMap.erase(itr);
            continue;
        }

        if (time.elapsed() - itr.value() >= kRefreshSpaceTime) {
            updateRefreshTimeByUrl(itr.key());
            fileinfo->refresh();
            itr = needRefreshMap.erase(itr);
            itrEnd = needRefreshMap.end();
        } else
            ++itr;
    }
    QThread::msleep(kRefreshSpaceTime);
}

InfoCachePrivate::InfoCachePrivate(InfoCache *qq)
    : q(qq)
{
    refreshThread.reset(new ReFreshThread);
}

InfoCachePrivate::~InfoCachePrivate()
{
    if (refreshThread) {
        refreshThread->stopRefresh();
        refreshThread->deleteLater();
    }
}
/*!
 * \brief updateSortByTimeCacheUrlList 跟新需要根据时间来刷新的file
 *
 * \param QUrl 文件的URL
 *
 * \return
 */
void InfoCachePrivate::updateSortByTimeCacheUrlList(const QUrl &url)
{
    if (sortByTimeCacheUrl.lContains(url))
        sortByTimeCacheUrl.lRemove(url);
    sortByTimeCacheUrl.lPushBack(url);
}

InfoCache::InfoCache(QObject *parent)
    : QObject(parent), d(new InfoCachePrivate(this))
{
    connect(&d->removeTimer, &QTimer::timeout, this, &InfoCache::timeRemoveCache);
    d->removeTimer.setInterval(kCacheRemoveTime);
    d->removeTimer.start();
    connect(&d->needRemoveTimer, &QTimer::timeout, this, &InfoCache::timeNeedRemoveCache);
    d->needRemoveTimer.start(1000);
    d->needRemoveTimer.setInterval(kCacheRemoveTime);
}

InfoCache::~InfoCache()
{
    Q_D(InfoCache);
    d->removeTimer.stop();
    d->removeTimer.deleteLater();
}
/*!
 * \brief instance 获取DFileInfoManager的对象
 *
 * \return DFileInfoManager的对象
 */
InfoCache &InfoCache::instance()
{
    return *_InfoCacheManager;
}
/*!
 * \brief getFileInfo 获取fileinfo
 *
 * 获取缓存的fileinfo的智能指针，不需要自己析构和refresh
 *
 * \param QUrl 文件的URL
 *
 * \return fileinfo的智能指针
 */
AbstractFileInfoPointer InfoCache::getCacheInfo(const QUrl &url)
{
    Q_D(InfoCache);
    if (d->fileInfos.contains(url)) {
        // TODO(liyigang)
        // TODO(perf) the whole Cache mechanism should be refactored.
        d->updateSortByTimeCacheUrlList(url);
        if (d->needRemoveCacheList.containsByLock(url)) {
            d->removedCacheList.push_backByLock(url);
        }
    }
    return d->fileInfos.value(url);
}
/*!
 * \brief cacheInfo 缓存fileinfo
 *
 * 线程安全，缓存fileinfo，又就返回，没有就缓存
 *
 * \param DAbstractFileInfoPointer fileinfo的智能指针
 *
 * \return
 */
void InfoCache::cacheInfo(const QUrl &url, const AbstractFileInfoPointer &info)
{
    Q_D(InfoCache);

    if (!info)
        return;

    //获取监视器，监听当前的file的改变
    QSharedPointer<AbstractFileWatcher> watcher { nullptr };

    watcher = WatcherFactory::create<AbstractFileWatcher>(UrlRoute::urlParent(url));

    if (watcher) {
        if (watcher->getCacheInfoConnectSize() == 0) {
            connect(watcher.data(), &AbstractFileWatcher::fileDeleted, this, &InfoCache::refreshFileInfo);
            connect(watcher.data(), &AbstractFileWatcher::fileAttributeChanged, this, &InfoCache::refreshFileInfo);
            connect(watcher.data(), &AbstractFileWatcher::subfileCreated, this, &InfoCache::refreshFileInfo);
            connect(watcher.data(), &AbstractFileWatcher::fileRename, this, &InfoCache::refreshFileInfo);
        }
        watcher->addCacheInfoConnectSize();
    }

    d->fileInfos.insert(url, info);

    d->updateSortByTimeCacheUrlList(url);
    d->refreshThread->updateRefreshTimeByUrl(url);
    //超过缓存总数量时，直接移除缓存，将移除的url加入到已移除队列
    if (d->sortByTimeCacheUrl.lCount() > kCacheFileinfoCount) {
        QUrl removeUrl = d->sortByTimeCacheUrl.lFirst() /* == nullptr ? QUrl() : *(d->sortByTimeCacheUrl.first())*/;
        removeCacheInfo(removeUrl);
        //已加入到待remove的链表，加入到m_removedCacheList链表
        if (d->needRemoveCacheList.containsByLock(removeUrl))
            d->removedCacheList.push_backByLock(removeUrl);
        //还没加入待remove的链表，加入到m_removedSortByTimeCacheList
        if (d->sortByTimeCacheUrl.lContains(removeUrl))
            d->removedSortByTimeCacheList.push_backByLock(removeUrl);
    }
}
/*!
 * \brief removeCacheInfo 移除缓存的fileinfo
 *
 * 线程安全，缓存fileinfo，有就移除，没有就返回
 *
 * \param QUrl 文件的URL
 *
 * \param DAbstractFileInfoPointer fileinfo的智能指针
 *
 * \return
 */
void InfoCache::removeCacheInfo(const QUrl &url)
{
    Q_D(InfoCache);
    // 断开信号连接
    QSharedPointer<AbstractFileWatcher> watcher = WatcherCache::instance().getCacheWatcher(url);
    if (watcher) {
        watcher->reduceCacheInfoConnectSize();
        if (watcher->getCacheInfoConnectSize() <= 0) {
            disconnect(watcher.data(), &AbstractFileWatcher::fileDeleted, this, &InfoCache::refreshFileInfo);
            disconnect(watcher.data(), &AbstractFileWatcher::fileAttributeChanged, this, &InfoCache::refreshFileInfo);
            disconnect(watcher.data(), &AbstractFileWatcher::subfileCreated, this, &InfoCache::refreshFileInfo);
            disconnect(watcher.data(), &AbstractFileWatcher::fileRename, this, &InfoCache::refreshFileInfo);
            WatcherCache::instance().removeCacheWatcher(url);
        }
    }
    d->fileInfos.remove(url);
    //移除刷新的url
    d->refreshThread->removeRefreshByUrl(url);
}

bool InfoCache::cacheDisable(const QString &scheme)
{
    return d->disableCahceSchemes.contains(scheme);
}

void InfoCache::setCacheDisbale(const QString &scheme, bool disbale)
{
    if (!d->disableCahceSchemes.contains(scheme) && disbale) {
        d->disableCahceSchemes.push_backByLock(scheme);
        return;
    }
    if (d->disableCahceSchemes.contains(scheme) && !disbale) {
        d->disableCahceSchemes.removeOneByLock(scheme);
        return;
    }
}
/*!
 * \brief refreshFileInfo 刷新缓存fileinfo
 *
 * \param QUrl 文件的URL
 *
 * \param const QSharedPointer<DFMIO::DFileInfo> &fileInfo 文件最新的dfm-io
 *
 * \return
 */
void InfoCache::refreshFileInfo(const QUrl &url)
{
    AbstractFileInfoPointer info = getCacheInfo(url);
    if (info) {
        info->setFile(url);
    }
}
/*!
 * \brief timeNeedRemoveCache 将需要移除cache的fileurl添加到待移除列表
 *
 * 应用计数小于等于1的
 *
 * \return
 */
void InfoCache::timeNeedRemoveCache()
{
    Q_D(InfoCache);
    d->sortByTimeCacheUrl.lock();
    QList<QUrl> allUrl = d->sortByTimeCacheUrl.list();
    QList<QUrl> removeUrl;
    for (const auto &url : allUrl) {
        if (d->removedSortByTimeCacheList.containsByLock(url)) {
            d->removedSortByTimeCacheList.removeAllByLock(url);
            removeUrl << url;
            continue;
        }
        //如果文件缓存中没有当前url的直接移除当前的url，和按时间排序的url
        AbstractFileInfoPointer info = d->fileInfos.value(url);
        if (!info) {
            removeUrl << url;
            continue;
        }
        //插入待移除队列并移除时间排序的url
    }

    // removeUrl 太多 可能导致主线程卡主
    // todo lanxs
    for (const auto &url : removeUrl) {
        d->sortByTimeCacheUrl.remove(url);
    }

    d->sortByTimeCacheUrl.unlock();
}
/*!
 * \brief timeNeedRemoveCache 将待移除文件列表的文件移除caches
 *
 * 将共享指针引用计数小于1的缓存文件信息的url加入到待析构队列
 *
 * \return
 */
void InfoCache::timeRemoveCache()
{
    Q_D(InfoCache);
    d->needRemoveCacheList.lock();
    QList<QUrl> allUrl = d->needRemoveCacheList.list();
    QList<QUrl> removeUrl;
    for (const auto &url : allUrl) {
        if (d->removedCacheList.containsByLock(url)) {
            d->removedCacheList.removeAllByLock(url);
        } else {
            // 移除文件信息缓存
            removeCacheInfo(url);
        }

        // 移除当前需要移除缓存列表
        removeUrl << url;
    }

    for (const auto &url : removeUrl) {
        d->needRemoveCacheList.removeAll(url);
    }

    d->needRemoveCacheList.unlock();
}

}
