#pragma once
#include "IFtpClient.h"
#include "ftpClient.h"

class FtpClientAdapter : public IFtpClient {
public:
    FtpClientAdapter(const std::string& host, int port, const std::string& user, const std::string& pass)
        : client(host, port, user, pass) {}

    bool uploadFile(const std::string& localFilePath, const std::string& remotePath) override {
        return client.uploadFile(localFilePath, remotePath);
    }

private:
    FtpClient client;
};