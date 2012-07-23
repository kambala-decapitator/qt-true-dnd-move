#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>


struct ImageInfo
{
    QString name;
    int width, height;

    ImageInfo() : width(0), height(0) {}
    ImageInfo(const QString &name_, int width_, int height_) : name(name_), width(width_), height(height_) {}
};


typedef QPair<int, int> TableKey; // row, column

class TableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    static const int kSize;
    static const int kCellSize;

    explicit TableModel(QObject *parent = 0);
    virtual ~TableModel() {}

    virtual int    rowCount(const QModelIndex &parent = QModelIndex()) const { Q_UNUSED(parent); return kSize; }
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const { Q_UNUSED(parent); return kSize; }
    virtual QVariant   data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    virtual Qt::DropActions supportedDropActions() const { return Qt::MoveAction; }
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

    ImageInfo imageInfoAtIndex(const QModelIndex &index)     const { return _imageNamesHash[qMakePair(index.row(), index.column())]; }
    ImageInfo imageInfoAtCoordinates(const TableKey &coords) const { return _imageNamesHash[coords]; }

    bool canStoreImageWithMimeDataAtIndex(const QMimeData *mimeData, const QModelIndex &modelIndex) const;
    bool canStoreImageWithCoordinatesAtIndex(const ImageInfo &storeImageInfo, const QModelIndex &modelIndex) const;
    TableKey coordinatesFromMimeData(const QMimeData *mimeData) const;

    void addItem(int row, int column, const ImageInfo &imageInfo) { addItem(qMakePair(row, column), imageInfo); }
    void addItem(const TableKey &key, const ImageInfo &imageInfo) { _imageNamesHash[key] = imageInfo; }

    bool isCustomDragAndDrop() const { return _isCustomDragAndDrop; }

    const QModelIndex &dragOriginIndex() const { return _dragOriginIndex; }
    void setDragOriginIndex(const QModelIndex &index) { _dragOriginIndex = index; }

    void setHighlightIndexes(const QModelIndexList &indexes) { _highlightIndexes = indexes; }

public slots:
    void setCustomDragAndDrop(bool b) { _isCustomDragAndDrop = b; }

signals:
    void itemMoved(const QModelIndex &newIndex, const QModelIndex &oldIndex);
    
private:
    QHash<TableKey, ImageInfo> _imageNamesHash;
    bool _isCustomDragAndDrop;
    QModelIndex _dragOriginIndex;
    QModelIndexList _highlightIndexes;
};

#endif // TABLEMODEL_H
