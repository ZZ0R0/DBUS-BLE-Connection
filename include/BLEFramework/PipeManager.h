// include/PipeManager.h

#ifndef PIPEMANAGER_H
#define PIPEMANAGER_H

#include <string>
#include <map>
#include <vector>
#include "BLETypes.h"

class PipeManager
{
public:
    PipeManager();
    ~PipeManager();

    // Add a new pipe
    void addPipe(const BLEPipe &pipe);

    // Remove a pipe by UUID
    bool removePipeByUUID(const std::string &uuid);

    // Get a pipe by UUID
    BLEPipe getPipeByUUID(const std::string &uuid) const;

    // Get all pipes
    std::vector<BLEPipe> getAllPipes() const;

private:
    // Map of UUID to BLEPipe
    std::map<std::string, BLEPipe> pipes;
};

#endif // PIPEMANAGER_H
