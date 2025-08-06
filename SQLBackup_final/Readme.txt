# SQLBackup

A C++ console application that creates an SQLite database with dummy user data, exports the database to a file, and uploads it to an FTP server using `libcurl`.

## 🛠 Requirements

- Visual Studio 2022 (or newer)
- Installed and linked sqlite3 and libcurl
- FTP server for testing (e.g., FileZilla Server)

## 🚀 Usage
SQLBackup.exe --db=users.db --host=127.0.0.1 --port=21 --user=testuser --pass=testpass --ftp-dir=/FTP
