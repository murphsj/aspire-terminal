#include <QApplication>
#include <QStandardItemModel>
#include <QFile>
#include <QStandardItem>
#include <QScrollArea>
#include <QPalette>
#include <qpalette.h>

#include "src/TerminalWidget.h"

int main(int argc, char **argv)
{
    // Force X11 because Wayland does not allow setting the position of popup windows (i.e. QCompleter list)
    qputenv("QT_QPA_PLATFORM", "xcb");

    QApplication app (argc, argv);



    QScrollArea* scrolling = new QScrollArea();
    QPalette p { scrolling->palette() };
    p.setColor(QPalette::Window, Qt::black);
    scrolling->setPalette(p);
    TerminalWidget* widget = new TerminalWidget(scrolling, 50, 50);
    scrolling->setWidget(widget);
    scrolling->show();

    return app.exec();
}