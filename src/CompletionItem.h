#include <QString>

class CompletionItem
{
public:
    explicit CompletionItem(QString name, QString description, CompletionItem* parentItem = nullptr);
    explicit CompletionItem();

    void appendChild(std::unique_ptr<CompletionItem>&& child);

    CompletionItem* child(int row);
    int childCount() const;
    QString name() const;
    QString description() const;
    int row() const;
    CompletionItem* parentItem();

private:
    std::vector<std::unique_ptr<CompletionItem>> m_childItems;
    CompletionItem* m_parentItem;
    QString m_name;
    QString m_description;
};