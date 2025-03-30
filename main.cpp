#include <QApplication>
#include <QTreeView>
#include <QAbstractItemModelTester>

#include <QStandardItemModel>
#include <QFile>
#include <QStandardItem>

#include <QLineEdit>

#include "src/TerminalWidget.h"
#include "src/ShellCompletionModel.h"
#include "src/ShellCompleter.h"

QAbstractItemModel* testModel() {
    QFile file("test.txt");
    if (!file.open(QFile::ReadOnly))
        return new QStringListModel();

    QStandardItemModel *model = new QStandardItemModel();
    QList<QStandardItem *> parents(10);
    parents[0] = model->invisibleRootItem();

    QRegularExpression re("^\\s+");
    while (!file.atEnd()) {
        const QString line = QString::fromUtf8(file.readLine());
        const QString trimmedLine = line.trimmed();
        if (trimmedLine.isEmpty())
            continue;

        const QRegularExpressionMatch match = re.match(line);
        int nonws = match.capturedStart();
        int level = 0;
        if (nonws == -1) {
            level = 0;
        } else {
            const int capLen = match.capturedLength();
            level = capLen / 4;
        }

        if (level + 1 >= parents.size())
            parents.resize(parents.size() * 2);

        QStandardItem *item = new QStandardItem;
        item->setText(trimmedLine);
        parents[level]->appendRow(item);
        parents[level + 1] = item;
    }

    return model;
}

int main(int argc, char **argv)
{
    QApplication app (argc, argv);

    
    TerminalWidget widget {};
    widget.show();

    /*
    QLineEdit edit {};
    ShellCompleter completer {};
    completer.setModel(new ShellCompletionModel());
    completer.setWidget(&edit);
    edit.setCompleter(&completer);
    completer.setCompletionRole(Qt::DisplayRole);
    completer.setCaseSensitivity(Qt::CaseInsensitive);
    completer.setCompletionColumn(0);
    edit.show();*/

    return app.exec();
}