#ifndef ASPIRE_TERMINAL_SHELL_COMPLETION_ITEM
#define ASPIRE_TERMINAL_SHELL_COMPLETION_ITEM

#include <QString>

class ShellCompletionItem
{
public:
    explicit ShellCompletionItem(QString name, QString description, ShellCompletionItem* parentItem = nullptr);
    explicit ShellCompletionItem();

    void appendChild(std::unique_ptr<ShellCompletionItem>&& child);

    ShellCompletionItem* child(int row);
    int childCount() const;
    QString name() const;
    QString description() const;
    int row() const;
    ShellCompletionItem* parentItem();

private:
    std::vector<std::unique_ptr<ShellCompletionItem>> m_childItems;
    ShellCompletionItem* m_parentItem;
    QString m_name;
    QString m_description;
};

#endif