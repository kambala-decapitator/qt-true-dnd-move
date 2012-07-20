#include <QApplication>

#include "widget.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Widget w;
    w.resize(500, 500);
    w.show();

    return a.exec();
}
