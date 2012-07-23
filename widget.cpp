#include "widget.h"
#include "tableview.h"

#include <QLabel>
#include <QCheckBox>
#include <QGridLayout>


Widget::Widget(QWidget *parent) : QWidget(parent), _tableView(new TableView(this)), _label(new QLabel("undroppable area", this)), _checkBox(new QCheckBox("use custom drag and drop", this))
{
    QGridLayout *grid = new QGridLayout(this);
    grid->addWidget(_tableView, 0, 0);
    grid->addWidget(_label, 0, 1);
    grid->addWidget(_checkBox, 1, 0);

    connect(_checkBox, SIGNAL(toggled(bool)), _tableView->model(), SLOT(setCustomDragAndDrop(bool)));
    _checkBox->setChecked(true);
}
