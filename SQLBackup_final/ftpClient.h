#pragma once

#include <curl/curl.h>
#include <string>

class FtpClient {
public:
    FtpClient(const std::string& host, int port, const std::string& username, const std::string& password);
    ~FtpClient();

    void uploadFile(const std::string& localFilePath, const std::string& remotePath);

private:
    std::string host;
    int port;
    std::string username;
    std::string password;

    CURL* curl{ nullptr };
};