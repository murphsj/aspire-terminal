#ifndef ASPIRE_TERMINAL_COMPLETION_MODEL
#define ASPIRE_TERMINAL_COMPLETION_MODEL

#include <QAbstractItemModel>
#include <QStringView>
#include "CompletionItem.h"

class CompletionModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    Q_DISABLE_COPY_MOVE(CompletionModel)

    explicit CompletionModel();
    ~CompletionModel() override;

    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags (const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = {}) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;

private:
    static void setupModelData(QStringList paths, CompletionModel* parent);

    std::unique_ptr<CompletionItem> rootItem;
};

#endif