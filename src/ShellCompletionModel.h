#ifndef ASPIRE_TERMINAL_SHELL_COMPLETION_MODEL
#define ASPIRE_TERMINAL_SHELL_COMPLETION_MODEL

#include <QAbstractItemModel>
#include <QStringView>
#include "ShellCompletionItem.h"

class ShellCompletionModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    Q_DISABLE_COPY_MOVE(ShellCompletionModel)

    explicit ShellCompletionModel(QObject* parent = nullptr);
    ~ShellCompletionModel() override;

    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags (const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = {}) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;

private:
    static void setupModelData(QStringList paths, ShellCompletionItem* parent);

    std::unique_ptr<ShellCompletionItem> rootItem;
};

#endif