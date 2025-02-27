#include "CompletionModel.h"
#include <QDir>
#include <zlib.h>

QStringList getManpagePaths() {
    // TODO: check $MANPAGE environment var and other locations based on OS
    // just returning the most common locations for now
    return QStringList {
        QStringLiteral("/usr/share/man"),
        QStringLiteral("/usr/local/man"),
        QStringLiteral("/usr/local/share/man")
    };
}

QStringList getAllManpageFiles() {
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



CompletionModel::CompletionModel()
{
    setupModelData(getAllManpageFiles(), this);
}

void CompletionModel::setupModelData(QStringList paths, CompletionModel* parent) {

}