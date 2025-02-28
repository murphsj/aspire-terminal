#include <QString>
#include "CompletionItem.h"

/*
 * Code here is mostly adapted from the Qt docs "Simple Tree Model Example" tutorial
 * Using a tree to store command completions isn't the best solution, since it will only ever be 1 layer deep (arguments are leafs of commands)
 * However, I wanted a way to represent this hierarchy while still having arguments be part of the same searchable database
 */

CompletionItem::CompletionItem(QString name, QString description, CompletionItem* parentItem)
    : m_name(name)
    , m_description(description)
    , m_parentItem(parentItem)
{

}

// Root item constructor
CompletionItem::CompletionItem()
{

}

void CompletionItem::appendChild(std::unique_ptr<CompletionItem>&& child)
{
    m_childItems.push_back(std::move(child));
}

CompletionItem* CompletionItem::child(int row)
{
    return row >= 0 && row < childCount() ? m_childItems.at(row).get() : nullptr;
}

int CompletionItem::childCount() const
{
    return static_cast<int>(m_childItems.size());
}

int CompletionItem::row() const
{
    if (m_parentItem == nullptr) return 0;
    const auto it = std::find_if(m_parentItem->m_childItems.cbegin(), m_parentItem->m_childItems.cend(),
        [this](const std::unique_ptr<CompletionItem>& completionItem) {
            return completionItem.get() == this;
        }
    );

    if (it != m_parentItem->m_childItems.cend())
        return std::distance(m_parentItem->m_childItems.cbegin(), it);
    // Unreachable
    Q_ASSERT(false);
    return -1;
}

CompletionItem* CompletionItem::parentItem()
{
    return m_parentItem;
}

QString CompletionItem::name() const
{
    return m_name;
}

QString CompletionItem::description() const
{
    return m_description;
}