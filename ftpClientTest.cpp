#include "ftpClient.h"
#include <gtest/gtest.h>
#include <stdexcept>
#include <fstream>

TEST(FtpClientTest, ThrowsIfFileDoesNotExist) {
    FtpClient client("localhost", 21, "user", "pass");
    EXPECT_THROW(client.uploadFile("nonexistent_file.txt", "/upload/nonexistent.txt"), std::runtime_error);
}

TEST(FtpClientTest, ThrowsIfCurlFails) {
    // Create dummy file
    const std::string dummyFile = "dummy.txt";
    std::ofstream out(dummyFile);
    out << "test content";
    out.close();

    FtpClient client("invalid.host", 21, "user", "pass");

    // Should throw because connection will fail
    EXPECT_THROW(client.uploadFile(dummyFile, "/upload/dummy.txt"), std::runtime_error);

    std::remove(dummyFile.c_str());
}