#include "ShellCompleter.h"
#include "ShellItemView.h"

#include <QObject>
#include <QWidget>
#include <QStringView>
#include <QRegularExpression>
#include <QEvent>
#include <QAbstractItemView>
#include <QListView>
#include <QHelpEvent>
#include <QRect>
#include <QItemSelectionModel>

ShellCompleter::ShellCompleter(QObject* parent)
    : QCompleter(parent)
{
    init();
};

ShellCompleter::ShellCompleter(ShellCompletionModel* model, QObject* parent)
    : QCompleter(model, parent)
{
    init();
};

void ShellCompleter::init()
{
    setPopup(new ShellItemView());
}


QStringList ShellCompleter::splitPath(const QString& path) const
{
    return path.split(QLatin1String("."));
};

QString ShellCompleter::pathFromIndex(const QModelIndex& index) const
{
    QStringList data;
    for (QModelIndex i = index; i.isValid(); i = i.parent()) {
        data.prepend(model()->data(i, Qt::DisplayRole).toString());
    }

    return data.join(QLatin1String("."));
};
