#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>


class TableView;

class QLabel;
//class QCheckBox;

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    Widget(QWidget *parent = 0);
    virtual ~Widget() {}

private:
    TableView *_tableView;
    QLabel *_label;
//    QCheckBox *_checkBox;
};

#endif // WIDGET_H
