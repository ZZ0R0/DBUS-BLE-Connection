// src/PipeManager.cpp

#include "PipeManager.h"
#include <algorithm>
#include <iostream>

// Constructor
PipeManager::PipeManager()
{
    std::cout << "[PipeManager] Constructor called." << std::endl;
}

// Destructor
PipeManager::~PipeManager()
{
    std::cout << "[PipeManager] Destructor called." << std::endl;
}

// Add a new pipe
void PipeManager::addPipe(const BLEPipe &pipe)
{
    std::string lowerUUID = pipe.uuid;
    // Convert UUID to lowercase for consistency
    std::transform(lowerUUID.begin(), lowerUUID.end(), lowerUUID.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });

    // Check for duplicate UUID
    if (pipes.find(lowerUUID) != pipes.end())
    {
        std::cerr << "[PipeManager] Warning: Pipe with UUID " << lowerUUID << " already exists. Skipping addition." << std::endl;
        return;
    }

    pipes[lowerUUID] = pipe;
    std::cout << "[PipeManager] Added pipe: UUID=" << lowerUUID << ", Path=" << pipe.path << ", Type=" << static_cast<int>(pipe.type) << std::endl;
}

// Remove a pipe by UUID
bool PipeManager::removePipeByUUID(const std::string &uuid)
{
    std::string lowerUUID = uuid;
    std::transform(lowerUUID.begin(), lowerUUID.end(), lowerUUID.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });

    auto it = pipes.find(lowerUUID);
    if (it != pipes.end())
    {
        pipes.erase(it);
        std::cout << "[PipeManager] Removed pipe with UUID " << lowerUUID << "." << std::endl;
        return true;
    }
    else
    {
        std::cerr << "[PipeManager] Error: Pipe with UUID " << lowerUUID << " not found." << std::endl;
        return false;
    }
}

// Get a pipe by UUID
BLEPipe PipeManager::getPipeByUUID(const std::string &uuid) const
{
    std::string lowerUUID = uuid;
    std::transform(lowerUUID.begin(), lowerUUID.end(), lowerUUID.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });

    auto it = pipes.find(lowerUUID);
    if (it != pipes.end())
    {
        std::cout << "[PipeManager] Found pipe for UUID: " << lowerUUID << " with Path: " << it->second.path << std::endl;
        return it->second;
    }
    else
    {
        std::cerr << "[PipeManager] Warning: Pipe with UUID " << lowerUUID << " not found." << std::endl;
        return BLEPipe(); // Return an empty pipe
    }
}

// Get all pipes
std::vector<BLEPipe> PipeManager::getAllPipes() const
{
    std::vector<BLEPipe> allPipes;
    for (const auto &pair : pipes)
    {
        allPipes.push_back(pair.second);
    }
    return allPipes;
}
