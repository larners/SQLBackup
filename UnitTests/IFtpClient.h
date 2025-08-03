// IFtpClient.h
#pragma once
#include <string>

class IFtpClient {
public:
    virtual ~IFtpClient() = default;
    virtual bool uploadFile(const std::string& localFilePath, const std::string& remotePath) = 0;
};