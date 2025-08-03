#include "ftpClient.h"
#include <curl/curl.h>
#include <fstream>
#include <iostream>

FtpClient::FtpClient(const std::string& host, int port, const std::string& username, const std::string& password)
    : host(host), port(port), username(username), password(password) {}

FtpClient::~FtpClient() {}

void FtpClient::uploadFile(const std::string& localFilePath, const std::string& remotePath) {
    CURL* curl;
    CURLcode res;
    FILE* file;

#ifdef _WIN32
    fopen_s(&file, localFilePath.c_str(), "rb");
#else
    file = fopen(localFilePath.c_str(), "rb");
#endif

    if (!file)
        throw std::runtime_error("Failed to open local file: " + localFilePath);

    try {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("Failed to initialize curl.");
        }

        std::string ftpUrl = "ftp://" + host + ":" + std::to_string(port) + remotePath;

        curl_easy_setopt(curl, CURLOPT_URL, ftpUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_USERPWD, (username + ":" + password).c_str());
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_READDATA, file);
        curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR_RETRY);
        curl_easy_setopt(curl, CURLOPT_FTP_RESPONSE_TIMEOUT, 30L);
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            throw std::runtime_error("curl_easy_perform() failed: " + std::string(curl_easy_strerror(res)));
        }

        std::cout << "File uploaded successfully to FTP: " << remotePath << "\n";
    }
    catch (...) {
        if (curl)
            curl_easy_cleanup(curl);
        fclose(file);
        curl_global_cleanup();
        throw;  // rethrow the exception
    }

    // Cleanup after success
    if (curl)
        curl_easy_cleanup(curl);
    fclose(file);
    curl_global_cleanup();
}