#ifndef CMDPARAMS_H
#define CMDPARAMS_H

#include <string>

struct CmdParams
{
    int port = 9090;
    int qubits = 15;
    std::string config = "/usr/local/etc/qusprout/qusprout-work.yaml";
    bool density = false;
    int execType = 0;
};

#endif

