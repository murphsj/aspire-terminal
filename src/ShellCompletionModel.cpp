#include "ShellCompletionModel.h"
#include "ShellCompletionItem.h"

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

void readFromManpage(QString path, ShellCompletionItem* parent)
{
    static QRegularExpression getCommandDescription {"^\\.SH DESCRIPTION(?:[\\S\\s]*?)\\.PP[\\s]?(.*?)$", 
        QRegularExpression::MultilineOption | QRegularExpression::CaseInsensitiveOption};
    static QRegularExpression getArgumentInfo {"^\\.TP\\s((?:\\\\fB)?\\\\-.*?)\\n([\\S\\s]*?)(?=\\.TP|\\.SH|\\.PP)",
                QRegularExpression::MultilineOption | QRegularExpression::CaseInsensitiveOption};
    QFileInfo file {path};

    QString commandName = file.baseName();
    QString page = Compression::readGzipFile(path);
    QRegularExpressionMatch descMatch = getCommandDescription.match(page);
    QString commandDescription = descMatch.captured(1);

    std::unique_ptr<ShellCompletionItem> commandItem = std::make_unique<ShellCompletionItem>(commandName, commandDescription, parent);

    QRegularExpressionMatchIterator argMatches = getArgumentInfo.globalMatch(page);
    qDebug() << page;
    for (QRegularExpressionMatch argumentInfo : argMatches) {
        QString argumentName {argumentInfo.captured(1)};
        QString argumentDescription {argumentInfo.captured(2)};
        trimGroff(argumentName);
        trimGroff(argumentDescription);

        commandItem->appendChild(std::make_unique<ShellCompletionItem>(argumentName, argumentDescription, commandItem.get()));
    }

    parent->appendChild(std::move(commandItem));
}

void ShellCompletionModel::setupModelData(QStringList paths, ShellCompletionItem* parent)
{
    // TODO: use given paths
    readFromManpage(QStringLiteral("/usr/share/man/man1/cat.1.gz"), parent);
    readFromManpage(QStringLiteral("/usr/share/man/man1/ls.1.gz"), parent);
    readFromManpage(QStringLiteral("/usr/share/man/man1/whatis.1.gz"), parent);
}

ShellCompletionModel::ShellCompletionModel(QObject* parent)
    : QAbstractItemModel(parent)
    , rootItem(std::make_unique<ShellCompletionItem>())
{
    setupModelData(getAllManpageFiles(), rootItem.get());
}

ShellCompletionModel::~ShellCompletionModel() = default;


QModelIndex ShellCompletionModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return {};

    ShellCompletionItem* parentItem = parent.isValid()
        ? static_cast<ShellCompletionItem*>(parent.internalPointer())
        : rootItem.get();
    
    if (auto *childItem = parentItem->child(row))
        return createIndex(row, column, childItem);

    return {};
}

QModelIndex ShellCompletionModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return {};

    auto *childItem = static_cast<ShellCompletionItem*>(index.internalPointer());
    ShellCompletionItem *parentItem = childItem->parentItem();

    return parentItem != rootItem.get()
        ? createIndex(parentItem->row(), 0, parentItem) : QModelIndex {};
}

int ShellCompletionModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0) return 0;
    const ShellCompletionItem* parentItem = parent.isValid()
        ? static_cast<const ShellCompletionItem*>(parent.internalPointer())
        : rootItem.get();

    return parentItem->childCount();
}

int ShellCompletionModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    return 2;
}

QVariant ShellCompletionModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return {};

    const auto *item = static_cast<const ShellCompletionItem*>(index.internalPointer());
    return index.column() == 0 ? item->name() : item->description();
}

Qt::ItemFlags ShellCompletionModel::flags(const QModelIndex& index) const
{
    return index.isValid()
        ? QAbstractItemModel::flags(index) : Qt::ItemFlags(Qt::NoItemFlags);
}

QVariant ShellCompletionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    return QVariant{};
}