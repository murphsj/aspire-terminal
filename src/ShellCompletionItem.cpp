#include <QString>
#include "ShellCompletionItem.h"

/*
 * Code here is mostly adapted from the Qt docs "Simple Tree Model Example" tutorial
 * Using a tree to store command completions isn't the best solution, since it will only ever be 1 layer deep (arguments are leafs of commands)
 * However, I wanted a way to represent this hierarchy while still having arguments be part of the same searchable database
 * This also plays nice with QCompleter since it can handle searching in 'sub-trees'
 */

ShellCompletionItem::ShellCompletionItem(QString name, QString description, ShellCompletionItem* parentItem)
    : m_name(name)
    , m_description(description)
    , m_parentItem(parentItem)
{

}

// Root item constructor
ShellCompletionItem::ShellCompletionItem()
{

}

void ShellCompletionItem::appendChild(std::unique_ptr<ShellCompletionItem>&& child)
{
    m_childItems.push_back(std::move(child));
}

ShellCompletionItem* ShellCompletionItem::child(int row)
{
    return row >= 0 && row < childCount() ? m_childItems.at(row).get() : nullptr;
}

int ShellCompletionItem::childCount() const
{
    return static_cast<int>(m_childItems.size());
}

int ShellCompletionItem::row() const
{
    if (m_parentItem == nullptr) return 0;
    const auto it = std::find_if(m_parentItem->m_childItems.cbegin(), m_parentItem->m_childItems.cend(),
        [this](const std::unique_ptr<ShellCompletionItem>& completionItem) {
            return completionItem.get() == this;
        }
    );

    if (it != m_parentItem->m_childItems.cend())
        return std::distance(m_parentItem->m_childItems.cbegin(), it);
    // Unreachable
    Q_ASSERT(false);
    return -1;
}

ShellCompletionItem* ShellCompletionItem::parentItem()
{
    return m_parentItem;
}

QString ShellCompletionItem::name() const
{
    return m_name;
}

QString ShellCompletionItem::description() const
{
    return m_description;
}