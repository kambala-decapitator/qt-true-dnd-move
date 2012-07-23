#include "tableview.h"

#include <QHeaderView>
#include <QDropEvent>

#include <QTimer>


TableView::TableView(QWidget *parent) : QTableView(parent), _dragLeaveTimer(new QTimer(this))
{
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::SingleSelection);

    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    setGridStyle(Qt::SolidLine);
    setStyleSheet("QTableView { background-color: black; gridline-color: #808080; }"
                  "QTableView::item { background-color: transparent; }"
                 );

    setDragDropMode(QAbstractItemView::DragDrop);
    setDragDropOverwriteMode(false);
    setDropIndicatorShown(true);

    setCornerButtonEnabled(false);
    horizontalHeader()->hide();
    verticalHeader()->hide();

    TableModel *model_ = new TableModel(this);
    setModel(model_);
    connect(model_, SIGNAL(itemMoved(const QModelIndex &, const QModelIndex &)), SLOT(updateSpansForIndexes(const QModelIndex &, const QModelIndex &)));

    int rows = model_->rowCount(), cols = model_->columnCount();
    for (int i = 0; i < rows; ++i)
        setRowHeight(i, TableModel::kCellSize);
    for (int i = 0; i < cols; ++i)
        setColumnWidth(i, TableModel::kCellSize);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            setCellSpanForImageInfoAtIndex(model_->index(i, j));

    _dragLeaveTimer->setInterval(100);
    connect(_dragLeaveTimer, SIGNAL(timeout()), SLOT(checkIfStillDragging()));
}

void TableView::dragEnterEvent(QDragEnterEvent *event)
{
    TableModel *model_ = model();
    QModelIndex index = indexAt(event->pos());

    if (!_draggedImage.width)
        _draggedImage = model_->imageInfoAtCoordinates(model_->coordinatesFromMimeData(event->mimeData()));
    updateHighlightIndexesForOriginIndex(index);

    if (!model_->dragOriginIndex().isValid())
    {
        _dragLeaveTimer->stop();
        model_->setDragOriginIndex(index);

        if (model_->isCustomDragAndDrop())
        {
            if (_draggedImage.width > 1 || _draggedImage.height > 1)
                setSpan(index.row(), index.column(), 1, 1);
            selectionModel()->clearSelection();
        }
    }

    QTableView::dragEnterEvent(event);
}

void TableView::dragMoveEvent(QDragMoveEvent *event)
{
    QModelIndex index = indexForDragDropEvent(event);
    updateHighlightIndexesForOriginIndex(index);
    viewport()->update();

    if (index.isValid() && model()->canStoreImageWithMimeDataAtIndex(event->mimeData(), index))
        event->acceptProposedAction();
    else
        event->ignore();
}

void TableView::dragLeaveEvent(QDragLeaveEvent *event)
{
    QTableView::dragLeaveEvent(event);

    model()->setHighlightIndexes(QModelIndexList());

    _dragLeaveTimer->setSingleShot(true);
    _dragLeaveTimer->start();
}

void TableView::dropEvent(QDropEvent *event)
{
    dragStopped();
    model()->setHighlightIndexes(QModelIndexList());

    QModelIndex index = indexForDragDropEvent(event);
    if (index.isValid() && model()->dropMimeData(event->mimeData(), event->dropAction(), index.row(), index.column(), index.parent()))
        event->acceptProposedAction();
    else
        event->ignore();
}

void TableView::checkIfStillDragging()
{
    if (findChild<QDrag *>()) // item has left current view
    {
        _dragLeaveTimer->setSingleShot(false);
        if (!_dragLeaveTimer->isActive())
            _dragLeaveTimer->start();
    }
    else // dragging stopped
    {
        _dragLeaveTimer->stop();

        QModelIndex originalIndex = model()->dragOriginIndex();
        dragStopped();
        setCellSpanForImageInfoAtIndex(originalIndex);
        setCurrentIndex(originalIndex);
    }
}

void TableView::updateSpansForIndexes(const QModelIndex &newIndex, const QModelIndex &oldIndex)
{
    int row = oldIndex.row(), col = oldIndex.column();
    if (rowSpan(row, col) > 1 || columnSpan(row, col) > 1)
        setSpan(row, col, 1, 1);
    setCellSpanForImageInfoAtIndex(newIndex);
    setCurrentIndex(newIndex);
}

void TableView::setCellSpanForImageInfoAtIndex(const QModelIndex &index)
{
    const ImageInfo &imageInfo = model()->imageInfoAtIndex(index);
    if (imageInfo.height > 1 || imageInfo.width > 1)
        setSpan(index.row(), index.column(), imageInfo.height, imageInfo.width);
}

QModelIndex TableView::actualIndexAt(const QPoint &p)
{
    return model()->index(rowAt(p.y()), columnAt(p.x()));
}

QModelIndex TableView::indexForDragDropEvent(QDropEvent *event)
{
    return actualIndexAt(event->pos() - findChild<QDrag *>()->hotSpot() + QPoint(rowHeight(0), columnWidth(0)) / 3);
}

void TableView::updateHighlightIndexesForOriginIndex(const QModelIndex &originIndex) const
{
    TableModel *model_ = model();
    QModelIndexList highlightIndexes;
    if (originIndex.isValid())
    {
        highlightIndexes += originIndex;
        for (int i = 0; i < _draggedImage.width; ++i)
            for (int j = 0; j < _draggedImage.height; ++j)
                if (i || j) // first index is already in the list
                {
                    QModelIndex anIndex = model_->index(originIndex.row() + j, originIndex.column() + i);
                    if (anIndex.isValid())
                        highlightIndexes += anIndex;
                }
    }

    model_->setHighlightIndexes(highlightIndexes);
}

void TableView::dragStopped()
{
    _draggedImage = ImageInfo();
    model()->setDragOriginIndex(QModelIndex());
}
