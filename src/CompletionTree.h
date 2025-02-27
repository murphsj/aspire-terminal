#include <QTreeView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

class CompletionTree
{
public:
    CompletionTree();
private:
    QTreeView* m_treeView;
    QStandardItemModel* m_completionItems;
    QSortFilterProxyModel* m_sortModel;
};