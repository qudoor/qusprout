#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include "common/gate.h"

#define ASSERT_RNT(rnt) \
if (false == (rnt)) { \
    return false; \
}

int main(int argc, char** argv)
{
    const int qubitnum = 2;

    CGate handle;
    bool rnt = handle.init(qubitnum);
    ASSERT_RNT(rnt);

    std::vector<int> targets;
    targets.push_back(0);
    rnt = handle.hGate(targets);
    ASSERT_RNT(rnt);

    std::vector<int> mczcontrols;
    std::vector<int> mcztargets;
    mczcontrols.push_back(0);
    mcztargets.push_back(1);
    rnt = handle.cnotGate(mczcontrols, mcztargets);
    ASSERT_RNT(rnt);

    targets.clear();
    targets.push_back(0);
    targets.push_back(1);
    rnt = handle.measureGate(targets);
    ASSERT_RNT(rnt);

    Result result;
    rnt = handle.run(100, result);
    ASSERT_RNT(rnt);

    std::cout << getPrint(result) << std::endl;

    return 0;
}
