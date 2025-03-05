#include "Compression.h"
#include "zlib.h"

#include <QLatin1String>
#include <QString>
#include <qbytearrayview.h>

QString Compression::readGzipFile(const char* filepath)
{
    gzFile file = gzopen(filepath, "rb");

    QString output {};

    while (true) {
        char readBuffer[CHUNK_SIZE];
        ssize_t len {gzread(file, readBuffer, CHUNK_SIZE)};
        if (len > 0) {
            output.append(QLatin1String {readBuffer, len});
        } else {
            break;
        }
    };
    
    return output;
}

QString Compression::readGzipFile(QString filepath)
{
    // Copy QString to a C-style string so zlib can use it
    char* cPath;
    std::string fname = filepath.toStdString();
    cPath = new char[fname.size() + 1];
    strcpy(cPath, fname.c_str());
    
    return readGzipFile(cPath);
}