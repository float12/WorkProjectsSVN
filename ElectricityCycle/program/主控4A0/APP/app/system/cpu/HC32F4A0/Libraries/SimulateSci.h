#ifndef __SIMULATESCI_H
#define __SIMULATESCI_H

BYTE SimulateInitSci(BYTE SciPhNum);
BYTE SimulateSciRcvEnable(BYTE SciPhNum);
BYTE SimulateSciRcvDisable(BYTE SciPhNum);
BYTE SimulateSciSendEnable(BYTE SciPhNum);
BYTE SimulateSciBeginSend(BYTE SciPhNum);
#endif