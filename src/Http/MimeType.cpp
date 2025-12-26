#include "../../include/Http/MimeType.h"

const std::unordered_map<std::string, std::string> MimeType::_mimeTypes = {
    {".html", "text/html"},
    {".htm",  "text/html"},
    {".css",  "text/css"},
    {".js",   "application/javascript"},
    {".json", "application/json"},
    {".png",  "image/png"},
    {".jpg",  "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".gif",  "image/gif"},
    {".svg",  "image/svg+xml"},
    {".ico",  "image/x-icon"},
    {".txt",  "text/plain"},
    {".xml",  "application/xml"},
};

std::string MimeType::fromPath(const std::string& path) {
    size_t dotPos = path.rfind( '.' );
    if(dotPos == std::string::npos) {
        return "application/octet-stream"; // 默认二进制流
    }
    return fromExtension(path.substr(dotPos));
}

std::string MimeType::fromExtension(const std::string& ext) {
    auto it = _mimeTypes.find(ext);
    if(it != _mimeTypes.end()) {
        return it->second;
    }
    return "application/octet-stream"; // 默认二进制流
}

