#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>

#include "tablemodel.h"


class QTimer;

class TableView : public QTableView
{
    Q_OBJECT

public:
    explicit TableView(QWidget *parent = 0);
    virtual ~TableView() {}

    TableModel *model() const { return static_cast<TableModel *>(QTableView::model()); }

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dropEvent(QDropEvent *event);

private slots:
    void checkIfStillDragging();
    void updateSpansForIndexes(const QModelIndex &newIndex, const QModelIndex &oldIndex);

private:
    QTimer *_dragLeaveTimer;
    ImageInfo _draggedImage;

    void setCellSpanForImageInfoAtIndex(const QModelIndex &index);

    QModelIndex actualIndexAt(const QPoint &p);
    QModelIndex indexForDragDropEvent(QDropEvent *event);
    void updateHighlightIndexesForOriginIndex(const QModelIndex &originIndex) const;
    void dragStopped();
};

#endif // TABLEVIEW_H
