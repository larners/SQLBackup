
#include "database.h"
#include <iostream>
#include "ftpClient.h"
#include "users.h"

struct AppConfig {
    std::string dbPath;
    std::string host;
    int port;
    std::string user;
    std::string pass;
    std::string ftpDir;
};

AppConfig parse_args(int argc, char* argv[]) {
    AppConfig cfg;
    // Simple parsing, improve with getopt or other libs
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.find("--db=") == 0)
            cfg.dbPath = arg.substr(5);
        else if (arg.find("--host=") == 0)
            cfg.host = arg.substr(7);
        else if (arg.find("--port=") == 0)
            cfg.port = std::stoi(arg.substr(7));
        else if (arg.find("--user=") == 0)
            cfg.user = arg.substr(7);
        else if (arg.find("--pass=") == 0)
            cfg.pass = arg.substr(7);
        else if (arg.find("--ftp-dir=") == 0)
            cfg.ftpDir = arg.substr(10);
    }
    return cfg;
}

int main(int argc, char* argv[])
{
    auto config = parse_args(argc, argv);
    Database db(config.dbPath);

    usersTableHelper users(db);
    users.fillTable();

    std::cout << "Database ready and filled with data.\n";
    std::string localPath = "users_backup.db";
    if (!db.dumpToFile(localPath))
    {
        std::cerr << "Failed to backup.\n";
        return 1;
    }

    FtpClient ftp(config.host, config.port, config.user, config.pass);
    if (!ftp.uploadFile(localPath, config.ftpDir +"/backup.db"))
        return 1;

    return 0;
}