#include <iostream>
#include <string>
#include "config/Config.h"
#include "common/Singleton.h"
#include "QuESTIniter.h"

CQuESTIniter::CQuESTIniter()
{
    // init QuEST
    m_env = createQuESTEnv();
}

CQuESTIniter::~CQuESTIniter()
{
    // unload QuEST
    destroyQuESTEnv(m_env);
}

void CQuESTIniter::Init(const int qubits, const bool density) 
{
    if (qubits <= 0) {
        return;
    }

    m_isInit = true;
    m_qubits = qubits;
    m_density = density;
    
    if (m_density) {
        m_qureg = createDensityQureg(qubits, m_env);
    } else {
        m_qureg = createQureg(qubits, m_env);
    }

    if (SINGLETON(CConfig)->m_verbose) {
        startRecordingQASM(m_qureg);
        std::cout << "################# qutrunk cmd start! #################" << std::endl;
        std::cout << "qreg q[" << qubits << "]" << std::endl;
        std::cout << "creg c[" << qubits << "]" << std::endl;
    }

    initZeroState(m_qureg);
}

void CQuESTIniter::finalize() 
{
    if (m_isInit == false)
        return;
    
    if (SINGLETON(CConfig)->m_verbose) 
    {
        std::cout << "################# qupy cmd end! ###################" << std::endl;
        std::cout << "***************** QuEST CMD start! *****************" << std::endl;
        printRecordedQASM(m_qureg);
        std::cout << "***************** QuEST CMD  end! *****************" << std::endl;
        clearRecordedQASM(m_qureg);
        stopRecordingQASM(m_qureg);
    }
    
    destroyQureg(m_qureg, m_env);
    m_isInit = false;
}

void CQuESTIniter::createWorkQureg(Qureg& workqureg)
{
    if (m_density) {
        workqureg = createDensityQureg(m_qubits, m_env);
    } else {
        workqureg = createQureg(m_qubits, m_env);
    }
}

void CQuESTIniter::destroyWorkQureg(Qureg& workqureg)
{
    destroyQureg(workqureg, m_env);
}

void CQuESTIniter::reInit(const int qubits)
{
    finalize();
    Init(qubits, m_density);
}
