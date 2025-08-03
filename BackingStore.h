#pragma once
#include <string>
#include <vector>

/// BackingStore manages all disk‐style page writes/reads for swapped pages.
class BackingStore {
public:
    /// @param filename  file to append/read pages
    explicit BackingStore(const std::string& filename = "csopesy-backing-store.txt");

    /// Write a page’s content to backing store
    bool writePage(const std::string& processName,
        int pageNumber,
        const std::string& content);

    /// Read a page’s content from backing store
    std::vector<std::string> readPage(const std::string& processName,
        int pageNumber);

    /// Log that pageNumber of processName was evicted
    void logEviction(const std::string& processName,
        int pageNumber);

private:
    std::string storeFilename;
};
