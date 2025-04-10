#include <QApplication>
#include <QStandardItemModel>
#include <QFile>
#include <QStandardItem>
#include <QScrollArea>

#include "src/TerminalWidget.h"

int main(int argc, char **argv)
{
    // Force X11 because Wayland does not allow setting the position of popup windows (i.e. QCompleter list)
    qputenv("QT_QPA_PLATFORM", "xcb");

    QApplication app (argc, argv);

    QScrollArea* scrolling = new QScrollArea();
    TerminalWidget* widget = new TerminalWidget(scrolling);
    scrolling->setWidget(widget);
    scrolling->show();

    return app.exec();
}