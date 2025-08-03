
#include "database.h"
#include <iostream>
#include "ftpClient.h"

int main() {
    auto& db = Database::getInstance("users.db");

    if (!db.insertRandomRows(100)) {
        std::cerr << "Failed to insert random data.\n";
        return 1;
    }

    std::cout << "Database ready and filled with data.\n";
    std::string localPath = "backup.db";
    if (!db.exportToFile(localPath))
    {
        std::cerr << "Failed to backup.\n";
        return 1;
    }

    FtpClient ftp("127.0.0.1", 21, "testuser", "testpass");
    if (!ftp.uploadFile(localPath, "/FTP/backup.db"))
        return 1;

    return 0;
}