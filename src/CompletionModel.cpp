#include "CompletionModel.h"
#include <QDir>
#include <qglobal.h>
#include <zlib.h>

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
            QStringList foundFiles = directory.entryList(entryFilter);
            result.append(foundFiles);
        }
    }

    qDebug() << result;
    return result;
}

void readFromManpage(QString path)
{
    // Copy QString to a C-style string so zlib can use it
    char* cPath;
    std::string fname = path.toStdString();
    cPath = new char[fname.size() + 1];
    strcpy(cPath, fname.c_str());

    gzFile file = gzopen(cPath, "rb");

    char readBuffer[65536];
    ssize_t len {gzread(file, readBuffer, 65536)};
    QString contents {readBuffer};

    qDebug() << contents;
}

CompletionModel::CompletionModel(QObject* parent)
    : QAbstractItemModel(parent)
    , rootItem(std::make_unique<CompletionItem>())
{
    setupModelData(getAllManpageFiles(), this);
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

void CompletionModel::setupModelData(QStringList paths, CompletionModel* parent)
{

}