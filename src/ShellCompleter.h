#ifndef ASPIRE_TERMINAL_SHELL_COMPLETER
#define ASPIRE_TERMINAL_SHELL_COMPLETER

#include "ShellCompletionModel.h"

#include <QCompleter>
#include <QString>
#include <QStringList>
#include <QModelIndex>
#include <QWidget>
#include <QLabel>
#include <qtmetamacros.h>

/**
 * QCompleter extended to take a ShellCompletionModel. Provides suggestions for commands, command arguments, and argument properties
 */
class ShellCompleter : public QCompleter
{
    Q_OBJECT

public:
    explicit ShellCompleter(QObject* parent = nullptr);
    explicit ShellCompleter(ShellCompletionModel* model, QObject* parent = nullptr);

protected:
    QStringList splitPath(const QString& path) const override;
    QString pathFromIndex(const QModelIndex& index) const override;

private:
    void init();
};

#endif