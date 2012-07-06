#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>


struct ImageInfo
{
    QString name;
    int w, h;

    ImageInfo() : w(0), h(0) {}
    ImageInfo(const QString &name_, int w_, int h_) : name(name_), w(w_), h(h_) {}
};


typedef QPair<int, int> TableKey; // row, column

class TableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    static const int kSize;

    explicit TableModel(QObject *parent = 0);
    virtual ~TableModel() {}

    virtual int    rowCount(const QModelIndex &parent = QModelIndex()) const { Q_UNUSED(parent); return kSize; }
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const { Q_UNUSED(parent); return kSize; }
    virtual QVariant   data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    virtual Qt::DropActions supportedDropActions() const { return Qt::MoveAction; }
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

    ImageInfo imageInfoAtIndex(const QModelIndex &index) const { return _imageNamesHash[qMakePair(index.row(), index.column())]; }
    bool canStoreImageWithMimeDataAtIndex(const QMimeData *mimeData, const QModelIndex &modelIndex) const;
    TableKey coordinatesFromMimeData(const QMimeData *mimeData) const;

    void    addItem(int row, int column, const ImageInfo &imageInfo) { addItem(qMakePair(row, column), imageInfo); }
    void    addItem(const TableKey &key, const ImageInfo &imageInfo) { _imageNamesHash[key] = imageInfo; }

    const QModelIndex &dragOriginIndex() const { return _dragOriginIndex; }
    void setDragOriginIndex(const QModelIndex &index) { _dragOriginIndex = index; }

public slots:

signals:
    void itemMoved(const QModelIndex &newIndex, const QModelIndex &oldIndex);
    
private:
    QHash<TableKey, ImageInfo> _imageNamesHash;
    QModelIndex _dragOriginIndex;
};

#endif // TABLEMODEL_H
