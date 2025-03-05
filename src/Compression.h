#ifndef ASPIRE_TERMINAL_COMPRESSION
#define ASPIRE_TERMINAL_COMPRESSION

#include <QString>

/**
  * Wrapper for compression/decompression utility libraries.
  */
namespace Compression {
    static const std::size_t CHUNK_SIZE = 65536;

    QString readGzipFile(QString filepath);
    QString readGzipFile(const char* filepath);
}

#endif