#ifndef QuESTIniter_H
#define QuESTIniter_H

#include "QuEST.h"

class CQuESTIniter
{
public:
    CQuESTIniter();
    virtual ~CQuESTIniter();

    void Init(const int qubits, const bool density);
    void finalize();

    void createWorkQureg(Qureg& workqureg);
    void destroyWorkQureg(Qureg& workqureg);
    
    void reInit(const int qubits);

public:
    QuESTEnv        m_env;
    Qureg           m_qureg;
    int             m_qubits{0};
    bool            m_isInit{false};
    bool            m_density{0};
};

#endif

