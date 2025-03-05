#include "CompletionModel.h"

#include <QDir>
#include <QString>
#include <QStringView>
#include <QList>
#include <QRegularExpression>
#include <qglobal.h>

#include "Compression.h"

QStringList getManpagePaths()
{
    // TODO: check $MANPAGE environment var and other locations based on OS
    // just returning the most common locations for now
    return QStringList {
        QStringLiteral("/usr/share/man"),
        QStringLiteral("/usr/local/man"),
        QStringLiteral("/usr/local/share/man")
    };
}

QStringList getAllManpageFiles()
{
    static QStringList manSections {
        QStringLiteral("man1")
    };
    static QStringList entryFilter {QStringLiteral("*.gz"), QStringLiteral("*.GZ")};
    QStringList paths = getManpagePaths();
    QStringList result = QStringList();

    for (QString filepath : paths) {
        for (QString section : manSections) {
            QDir directory {filepath + QDir::separator() + section};
            QFileInfoList foundFiles = directory.entryInfoList(entryFilter);
            for (QFileInfo file : foundFiles) {
                result.append(file.absoluteFilePath());
            }
        }
    }

    qDebug() << result;
    return result;
}

void trimGroff(QString& text) {
    // \f is followed by a character to do certain formatting macros, most notably bold/unbold
    static QRegularExpression groffEscapes {"\\\\f."};
    text.replace(groffEscapes, "");
    // For some reason dashes are escaped in roff
    text.replace("\\-", "-");
}

void readFromManpage(QString path, CompletionItem* parent)
{
    static QRegularExpression getCommandDescription {"^\\.SH DESCRIPTION(?:[\\S\\s]*?)\\.PP[\\s]?(.*?)$", 
        QRegularExpression::MultilineOption | QRegularExpression::CaseInsensitiveOption};
    static QRegularExpression getArgumentInfo {"^\\.TP\\s((?:\\\\fB)?\\\\-.*?)\\n([\\S\\s]*?)(?=\\.TP|\\.SH)",
                QRegularExpression::MultilineOption | QRegularExpression::CaseInsensitiveOption};
    QFileInfo file {path};

    QString commandName = file.baseName();
    QString page = Compression::readGzipFile(path);
    QRegularExpressionMatch descMatch = getCommandDescription.match(page);
    QString commandDescription = descMatch.captured(1);

    std::unique_ptr<CompletionItem> commandItem = std::make_unique<CompletionItem>(commandName, commandDescription, parent);
    

    QRegularExpressionMatchIterator argMatches = getArgumentInfo.globalMatch(page);
    qDebug() << page;
    for (QRegularExpressionMatch argumentInfo : argMatches) {
        QString argumentName {argumentInfo.captured(1)};
        QString argumentDescription {argumentInfo.captured(2)};
        trimGroff(argumentName);
        trimGroff(argumentDescription);

        commandItem->appendChild(std::make_unique<CompletionItem>(argumentName, argumentDescription, commandItem.get()));
    }

    parent->appendChild(std::move(commandItem));
}

void CompletionModel::setupModelData(QStringList paths, CompletionItem* parent)
{
    // TODO: use given paths
    readFromManpage(QStringLiteral("/usr/share/man/man1/cat.1.gz"), parent);
    readFromManpage(QStringLiteral("/usr/share/man/man1/ls.1.gz"), parent);
    readFromManpage(QStringLiteral("/usr/share/man/man1/whatis.1.gz"), parent);
}

CompletionModel::CompletionModel(QObject* parent)
    : QAbstractItemModel(parent)
    , rootItem(std::make_unique<CompletionItem>())
{
    setupModelData(getAllManpageFiles(), rootItem.get());
}

CompletionModel::~CompletionModel() = default;


QModelIndex CompletionModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return {};

    CompletionItem* parentItem = parent.isValid()
        ? static_cast<CompletionItem*>(parent.internalPointer())
        : rootItem.get();
    
    if (auto *childItem = parentItem->child(row))
        return createIndex(row, column, childItem);

    return {};
}

QModelIndex CompletionModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return {};

    auto *childItem = static_cast<CompletionItem*>(index.internalPointer());
    CompletionItem *parentItem = childItem->parentItem();

    return parentItem != rootItem.get()
        ? createIndex(parentItem->row(), 0, parentItem) : QModelIndex {};
}

int CompletionModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0) return 0;
    const CompletionItem* parentItem = parent.isValid()
        ? static_cast<const CompletionItem*>(parent.internalPointer())
        : rootItem.get();

    return parentItem->childCount();
}

int CompletionModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    return 2;
}

QVariant CompletionModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const auto *item = static_cast<const CompletionItem*>(index.internalPointer());
    return index.column() == 0 ? item->name() : item->description();
}

Qt::ItemFlags CompletionModel::flags(const QModelIndex& index) const
{
    return index.isValid()
        ? QAbstractItemModel::flags(index) : Qt::ItemFlags(Qt::NoItemFlags);
}

QVariant CompletionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    return QVariant{};
}