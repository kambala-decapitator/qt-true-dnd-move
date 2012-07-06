#include "tableview.h"
#include "tablemodel.h"

#include <QHeaderView>
#include <QDropEvent>

#include <QTimer>


const int TableView::kCellSize = 32;

TableView::TableView(QWidget *parent) : QTableView(parent), _dragLeaveTimer(new QTimer(this))
{
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::SingleSelection);

    setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    setGridStyle(Qt::SolidLine);
    setStyleSheet("QTableView { background-color: black; gridline-color: #808080; }"
                  "QTableView::item:selected { background-color: black; border: 1px solid #d9d9d9; }"
                  "QTableView::icon:selected { right: 1px; }"
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

    for (int i = 0; i < model_->rowCount(); ++i)
        setRowHeight(i, kCellSize);
    for (int i = 0; i < model_->columnCount(); ++i)
        setColumnWidth(i, kCellSize);
    for (int i = 0; i < model_->rowCount(); ++i)
        for (int j = 0; j < model_->columnCount(); ++j)
            setCellSpanForImageInfoAtIndex(model_->index(i, j));

    _dragLeaveTimer->setInterval(100);
    connect(_dragLeaveTimer, SIGNAL(timeout()), SLOT(checkIfStillDragging()));
}

TableModel *TableView::model() const
{
    return static_cast<TableModel *>(QTableView::model());
}

void TableView::setCellSpanForImageInfoAtIndex(const QModelIndex &index)
{
    const ImageInfo &imageInfo = model()->imageInfoAtIndex(index);
    if (imageInfo.h && imageInfo.w)
        setSpan(index.row(), index.column(), imageInfo.h, imageInfo.w);
}

void TableView::dragEnterEvent(QDragEnterEvent *event)
{
    _dragLeaveTimer->stop();

    TableModel *model_ = model();
    if (!model_->dragOriginIndex().isValid())
    {
        QModelIndex index = indexAt(event->pos());
        model_->setDragOriginIndex(index);
        setSpan(index.row(), index.column(), 1, 1);
    }
    selectionModel()->clearSelection();

    QTableView::dragEnterEvent(event);
}

void TableView::dragMoveEvent(QDragMoveEvent *event)
{
    if (model()->canStoreImageWithMimeDataAtIndex(event->mimeData(), indexForDragDropEvent(event)))
        event->acceptProposedAction();
    else
        event->ignore();
}

void TableView::dragLeaveEvent(QDragLeaveEvent *event)
{
    QTableView::dragLeaveEvent(event);

    _dragLeaveTimer->setSingleShot(true);
    _dragLeaveTimer->start();
}

void TableView::dropEvent(QDropEvent *event)
{
    dragStopped();

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
    setSpan(oldIndex.row(), oldIndex.column(), 1, 1);
    setCellSpanForImageInfoAtIndex(newIndex);
    setCurrentIndex(newIndex);
}

QModelIndex TableView::actualIndexAt(const QPoint &p)
{
    return model()->index(rowAt(p.y()), columnAt(p.x()));
}

QModelIndex TableView::indexForDragDropEvent(QDropEvent *event)
{
    return actualIndexAt(event->pos() - findChild<QDrag *>()->hotSpot() + QPoint(rowHeight(0), columnWidth(0)) / 3);
}

void TableView::dragStopped()
{
    model()->setDragOriginIndex(QModelIndex());
}
