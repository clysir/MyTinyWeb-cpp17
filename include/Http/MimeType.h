#pragma once

#include <unordered_map>
#include <string>

class MimeType 
{
public:
    // 根据扩展名 获得 MIME类型
    static std::string fromExtension(const std::string& ext);
    
    // 根据文件名 获得 MIME类型
    static std::string fromPath(const std::string& path);
private:
    static const std::unordered_map<std::string, std::string> _mimeTypes;
};