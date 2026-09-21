#pragma once
#include <expected>
#include <string>

namespace Utils::File {
    // Reads the whole file as text. On Windows this applies the platform's
    // newline translation; use readBinary for anything that is not text.
    std::expected<std::string, std::string> read(const std::string& path);

    // Reads the whole file verbatim -- no newline translation, embedded NULs
    // preserved. For binary payloads: DER, PKCS#12, images.
    std::expected<std::string, std::string> readBinary(const std::string& path);
}
