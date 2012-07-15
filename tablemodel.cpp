#include "tablemodel.h"

#include <QPixmap>
#include <QMimeData>
#include <QFile>


const int TableModel::kSize = 10;

TableModel::TableModel(QObject *parent) : QAbstractTableModel(parent)
{
    addItem(0, 0, ImageInfo("bird4.png", 1, 2));
    addItem(5, 2, ImageInfo("apple.png", 1, 1));
    addItem(2, 1, ImageInfo("book02.png", 2, 2));
    addItem(4, 7, ImageInfo("bow_elefury.png", 2, 3));
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::BackgroundRole)
        return Qt::black;

    ImageInfo imageInfo = imageInfoAtIndex(index);
    if (_dragOriginIndex != index && !imageInfo.name.isEmpty() && role == Qt::DecorationRole)
        return QPixmap(":/images/" + imageInfo.name);

    return QVariant();
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags_ = QAbstractTableModel::flags(index);
    flags_ |= !imageInfoAtIndex(index).name.isEmpty() ? Qt::ItemIsDragEnabled : Qt::ItemIsDropEnabled;
    return flags_;
}

bool TableModel::dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(action); Q_UNUSED(parent);

    TableKey droppedImageCoordinates = coordinatesFromMimeData(mimeData);
    ImageInfo droppedImageInfo = _imageNamesHash[droppedImageCoordinates];
    QModelIndex newIndex = index(row, column), oldIndex = index(droppedImageCoordinates.first, droppedImageCoordinates.second);
    if (newIndex != oldIndex)
    {
        _imageNamesHash.remove(droppedImageCoordinates);
        addItem(row, column, droppedImageInfo);
    }
    emit itemMoved(newIndex, oldIndex);
    return true;
}

bool TableModel::canStoreImageWithMimeDataAtIndex(const QMimeData *mimeData, const QModelIndex &modelIndex) const
{
    TableKey storeImageCoordinates = coordinatesFromMimeData(mimeData);
    const ImageInfo &storeImageInfo = _imageNamesHash[storeImageCoordinates];
    QRect storeImageRect(modelIndex.column(), modelIndex.row(), storeImageInfo.w, storeImageInfo.h);
    if (storeImageRect.right() >= columnCount() || storeImageRect.bottom() >= rowCount()) // beyond grid
        return false;

    bool ok = true;
    QHash<TableKey, ImageInfo> hashCopy = _imageNamesHash;
    hashCopy.remove(storeImageCoordinates);
    for (QHash<TableKey, ImageInfo>::const_iterator iter = hashCopy.constBegin(); iter != hashCopy.constEnd(); ++iter)
    {
        const ImageInfo &imageInfo = iter.value();
        const TableKey &coordinates = iter.key();
        if (storeImageRect.intersects(QRect(coordinates.second, coordinates.first, imageInfo.w, imageInfo.h)))
        {
            ok = false;
            break;
        }
    }
    return ok;
}

TableKey TableModel::coordinatesFromMimeData(const QMimeData *mimeData) const
{
    QByteArray encoded = mimeData->data("application/x-qabstractitemmodeldatalist");
    QDataStream stream(&encoded, QIODevice::ReadOnly);
    int rowFrom, colFrom;
    stream >> rowFrom >> colFrom;
    return qMakePair(rowFrom, colFrom);
}
