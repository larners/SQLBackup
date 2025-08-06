#include "ftpClient.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

FtpClient::FtpClient(const std::string& host, int port, const std::string& username, const std::string& password)
    : host{ host }, port{ port }, username{ username }, password{ password }
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL\n";
        throw std::runtime_error("CURL initialization failed");
    }
}

FtpClient::~FtpClient()
{
    if (curl) {
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

void FtpClient::uploadFile(const std::string& localFilePath, const std::string& remotePath)
{
    FILE* file{ nullptr };

    fopen_s(&file, localFilePath.c_str(), "rb");
    if (!file) {
        throw std::runtime_error{"Failed to open local file : " + localFilePath};
    }

    auto url = "ftp://" + host + ":" + std::to_string(port) + remotePath;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERPWD, (username + ":" + password).c_str());
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_READDATA, file);
    curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR_RETRY);
    curl_easy_setopt(curl, CURLOPT_FTP_RESPONSE_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY); // Optional: FTP over SSL

    auto res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        throw std::runtime_error{"curl_easy_perform() failed: " + std::string(curl_easy_strerror(res))};
    }

    fclose(file);

    std::cout << "File uploaded successfully to FTP: " << remotePath << "\n";
}