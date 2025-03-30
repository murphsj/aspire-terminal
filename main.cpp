#include <QApplication>
#include <QTreeView>
#include <QAbstractItemModelTester>

#include "src/TerminalWidget.h"
#include "src/ShellCompletionModel.h"

int main(int argc, char **argv)
{
    QApplication app (argc, argv);

    TerminalWidget widget {};
    widget.show();

    ShellCompletionModel completion {};
    QTreeView tree {};
    tree.setModel(&completion);

    tree.show();

    return app.exec();
}