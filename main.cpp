#include <QApplication>
#include <QTreeView>

#include "src/TerminalWidget.h"
#include "src/ShellCompletionModel.h"

int main(int argc, char **argv)
{
    QApplication app (argc, argv);

    TerminalWidget widget {};
    widget.show();

    /**

    CompletionModel completion {};

    QTreeView testView {};
    testView.setModel(&completion);
    testView.show();

    */
    

    return app.exec();
}