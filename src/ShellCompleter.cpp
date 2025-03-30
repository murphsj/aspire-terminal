#include "ShellCompleter.h"

#include <QObject>
#include <QStringView>
#include <QRegularExpression>

ShellCompleter::ShellCompleter(QObject* parent)
    : QCompleter(parent)
{

};

ShellCompleter::ShellCompleter(ShellCompletionModel* model, QObject* parent)
    : QCompleter(model, parent)
{

};


QStringList ShellCompleter::splitPath(const QString& path) const
{
    static QRegularExpression whitespace {"\\s+"};
    qDebug() << "splitPath(): " << path;
    // TODO: include command name if present so arguments can be shown
    return path.split(QLatin1String("."));
};

QString ShellCompleter::pathFromIndex(const QModelIndex& index) const
{
    QStringList data;
    for (QModelIndex i = index; i.isValid(); i = i.parent()) {
        data.prepend(model()->data(i, Qt::DisplayRole).toString());
    }

    qDebug() << "pathFromIndex(): " << data.join(QLatin1String("/"));

    return data.join(QLatin1String("."));
};
