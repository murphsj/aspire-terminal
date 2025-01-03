#include <QApplication>
#include <QPushButton>

#include "src/TerminalWidget.h"

int main(int argc, char **argv)
{
    QApplication app (argc, argv);

    TerminalWidget widget {};
    widget.show();

    return app.exec();
}