
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

void databaseCreationAndBackupDemo(const AppConfig& cfg)
{
    // create database
    Database db{ cfg.dbPath };

    // create table
    db.createTable("users", { "id INTEGER", "name TEXT", "age INTEGER" });

    // fill with sample data (100 lines)
    for (int i = 0; i < 100; ++i) {
        auto name = "User" + std::to_string(i);
        int age = 18 + (rand() % 50);
        db.insert("users", { "NULL", name, std::to_string(age) });
    }

    // dump to file
    const std::string filename{"users_backup.db"};

    // upload to FTP server
    FtpClient ftp{ cfg.host, cfg.port, cfg.user, cfg.pass };
    try
    {
        db.dumpToFile(filename);
        ftp.uploadFile(filename, cfg.ftpDir + "/users_backup.db");
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error dumping database: " << ex.what() << "\n";
    }
}

int main(int argc, char* argv[])
{
    auto config = parse_args(argc, argv);
    databaseCreationAndBackupDemo(config);

    return 0;
}