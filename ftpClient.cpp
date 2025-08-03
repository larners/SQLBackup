#include "ftpClient.h"
#include <curl/curl.h>
#include <fstream>
#include <iostream>

FtpClient::FtpClient(const std::string& host, int port, const std::string& username, const std::string& password)
    : host(host), port(port), username(username), password(password) {}

FtpClient::~FtpClient() {}

bool FtpClient::uploadFile(const std::string& localFilePath, const std::string& remotePath) {
    CURL* curl;
    CURLcode res;
    FILE* file;

#ifdef _WIN32
    fopen_s(&file, localFilePath.c_str(), "rb");
#else
    file = fopen(localFilePath.c_str(), "rb");
#endif

    if (!file) {
        std::cerr << "Failed to open local file : " << localFilePath << "\n";
        return false;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        std::string ftpUrl = "ftp://" + host + ":" + std::to_string(port) + remotePath;

        curl_easy_setopt(curl, CURLOPT_URL, ftpUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_USERPWD, (username + ":" + password).c_str());
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_READDATA, file);
        curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR_RETRY);
        curl_easy_setopt(curl, CURLOPT_FTP_RESPONSE_TIMEOUT, 30L);
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY); // Optional: FTP over SSL

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
            curl_easy_cleanup(curl);
            fclose(file);
            curl_global_cleanup();
            return false;
        }

        curl_easy_cleanup(curl);
    }

    fclose(file);
    curl_global_cleanup();
    std::cout << "File uploaded successfully to FTP: " << remotePath << "\n";
    return true;
}