#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>


class QTimer;
class TableModel;
struct ImageInfo;

class TableView : public QTableView
{
    Q_OBJECT

public:
    static const int kCellSize;

    explicit TableView(QWidget *parent = 0);
    virtual ~TableView() {}

    TableModel *model() const;

    void setCellSpanForImageInfoAtIndex(const QModelIndex &index);

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

    QModelIndex actualIndexAt(const QPoint &p);
    QModelIndex indexForDragDropEvent(QDropEvent *event);
    void dragStopped();
};

#endif // TABLEVIEW_H
