#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include "common/gate.h"

CGate g_handle;
const int g_qubitnum = 10;

#define ASSERT_RNT(rnt) \
if (false == rnt) { \
    return false; \
}

bool flip(const int operindexs)
{
    std::vector<int> targets;
    std::vector<int> mczcontrols;
    std::vector<int> mcztargets;
    for (int i = 0; i < g_qubitnum; i++)
    {
        if (((operindexs >> i) & 1) == 0)
        {
            targets.push_back(i);
        }    

        if (i == g_qubitnum - 1)
        {
            mcztargets.push_back(i);
        }   
        else
        {
            mczcontrols.push_back(i);
        }
    }
    bool rnt = g_handle.xGate(targets);
    ASSERT_RNT(rnt);

    rnt = g_handle.mczGate(mczcontrols, mcztargets);
    ASSERT_RNT(rnt);
    
    rnt = g_handle.xGate(targets);
    ASSERT_RNT(rnt);

    return true;
}

bool image()
{
    std::vector<int> targets;
    std::vector<int> mczcontrols;
    std::vector<int> mcztargets;
    for (int i = 0; i < g_qubitnum; i++)
    {
        targets.push_back(i); 

        if (i == g_qubitnum - 1)
        {
            mcztargets.push_back(i);
        }   
        else
        {
            mczcontrols.push_back(i);
        }
    }

    bool rnt = g_handle.hGate(targets);
    ASSERT_RNT(rnt);

    rnt = g_handle.xGate(targets);
    ASSERT_RNT(rnt);

    rnt = g_handle.mczGate(mczcontrols, mcztargets);
    ASSERT_RNT(rnt);
    
    rnt = g_handle.xGate(targets);
    ASSERT_RNT(rnt);

    rnt = g_handle.hGate(targets);
    ASSERT_RNT(rnt);

    return true;
}

int main(int argc, char** argv)
{
    bool rnt = g_handle.init(g_qubitnum);
    ASSERT_RNT(rnt);

    rnt = g_handle.createPlusState();
    ASSERT_RNT(rnt);

    int qubitindexs = (int)(1 << g_qubitnum);
    int count = ceil(M_PI/4 * sqrt(qubitindexs));
    std::cout << "init(qubitnum:" << g_qubitnum << ",qubitindexs:"<< qubitindexs << ",count:" << count << ")." << std::endl;

    srand(time(NULL));
    int operindexs = rand() % qubitindexs;

    for (int i = 0; i < count; i++) {
        rnt = flip(operindexs);
        ASSERT_RNT(rnt);

        rnt = image();
        ASSERT_RNT(rnt);
        
        double amp = 0.0;
        rnt = g_handle.getProbAmp(operindexs, amp);
        ASSERT_RNT(rnt);

        std::cout << "prob of state |"<< operindexs << "> = " << std::setprecision(14) << amp << std::endl;
    }

    return 0;
}
