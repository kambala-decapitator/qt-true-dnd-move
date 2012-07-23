#include "tablemodel.h"

#include <QPixmap>

#include <QMimeData>
#include <QFile>


bool operator ==(const ImageInfo &a, const ImageInfo &b)
{
    return a.width == b.width && a.height == b.height && a.name == b.name;
}


const int TableModel::kSize = 10;
const int TableModel::kCellSize = 32;

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

    if (role == Qt::DecorationRole && _highlightIndexes.contains(index) && !_imageNamesHash.contains(qMakePair(index.row(), index.column())))
    {
        const ImageInfo &draggedImageInfo = imageInfoAtIndex(_dragOriginIndex);
        bool isGreen = _highlightIndexes.size() == draggedImageInfo.width * draggedImageInfo.height;
        if (isGreen) // false means that some part of image is beyond the grid
        {
            foreach (const QModelIndex &anIndex, _highlightIndexes)
            {
                if (anIndex.isValid() && !canStoreImageWithCoordinatesAtIndex(ImageInfo(QString(), 1, 1), anIndex))
                {
                    isGreen = false;
                    break;
                }
            }
        }

        QColor color(isGreen ? Qt::green : Qt::red);
        color.setAlpha(64);
        QPixmap pixmap(kCellSize, kCellSize);
        pixmap.fill(color);
        return pixmap;
    }

    const ImageInfo &imageInfo = imageInfoAtIndex(index);
    if (role == Qt::DecorationRole && (!_isCustomDragAndDrop || _dragOriginIndex != index) && !imageInfo.name.isEmpty())
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
    return canStoreImageWithCoordinatesAtIndex(_imageNamesHash[coordinatesFromMimeData(mimeData)], modelIndex);
}

bool TableModel::canStoreImageWithCoordinatesAtIndex(const ImageInfo &storeImageInfo, const QModelIndex &modelIndex) const
{
    QRect storeImageRect(modelIndex.column(), modelIndex.row(), storeImageInfo.width, storeImageInfo.height);
    if (storeImageRect.right() >= columnCount() || storeImageRect.bottom() >= rowCount()) // beyond grid
        return false;

    bool ok = true;
    TableKey draggedItemCoordinates = qMakePair(_dragOriginIndex.row(), _dragOriginIndex.column());
    for (QHash<TableKey, ImageInfo>::const_iterator iter = _imageNamesHash.constBegin(); iter != _imageNamesHash.constEnd(); ++iter)
    {
        const ImageInfo &imageInfo = iter.value();
        const TableKey &coordinates = iter.key();
        if (coordinates != draggedItemCoordinates && storeImageRect.intersects(QRect(coordinates.second, coordinates.first, imageInfo.width, imageInfo.height)))
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
