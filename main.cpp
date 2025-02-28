#include <QApplication>

#include "src/TerminalWidget.h"
#include "src/CompletionModel.h"

int main(int argc, char **argv)
{
    QApplication app (argc, argv);

    TerminalWidget widget {};
    widget.show();

    //CompletionModel testModel {};
    

    return app.exec();
}