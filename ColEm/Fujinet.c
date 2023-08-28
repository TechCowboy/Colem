#include "AdamNet.h"
#include "Fujinet.h"
#include "Coleco.h"
#include "EMULib.h"
#include "Z80.h"

#include <string.h>
#include <stdio.h>

static byte FNStatus;

void UpdateFujiClock(byte Dev, int V)
{
    // int J, N;
    // word A;

    if ((V & 0xFF) != 0xFF)
        printf("FUJICLK: %02x:%02x\n", (int)(Dev & 0xFF), V & 0xFF);

    switch (V)
    {
    case -1: // 0xFF
        printf("FUJICLK: 0xFF\n");
        SetDCB(Dev, DCB_CMD_STAT, FNStatus);
        break;
    case CMD_STATUS:     // 0x01
        printf("FUJICLK: STATUS\n");
        FNStatus = RSP_STATUS;
        break;
    case CMD_SOFT_RESET: // 0x02
        // Character-based device, single character buffer
        printf("FUJICLK: SOFT RESET\n");
        //ReportDevice(Dev, 0x0001, 0);
        FNStatus = RSP_STATUS;
        break;
    case CMD_WRITE: // 0x03
        printf("FUJICLK: WRITE\n");
        SetDCB(Dev, DCB_CMD_STAT, RSP_ACK + 0x0B);
        FNStatus = RSP_STATUS;
        break;
    case CMD_READ: // 0x04
        printf("FUJICLK: READ\n");
        break;
    }

    printf("FUJICLK: Done.\n");
}

void UpdateFujinet(byte Dev, int V)
{
    // int J, N;
    // word A;

    if ((V & 0xFF) != 0xFF)
        printf("FUJINET: %02x:%02x\n", (int)(Dev & 0xFF), V & 0xFF);

    switch (V)
    {
    case -1: // 0xFF
        SetDCB(Dev, DCB_CMD_STAT, FNStatus);
        break;
    case CMD_STATUS:     // 0x01
    case CMD_SOFT_RESET: // 0x02
        // Character-based device, single character buffer
        printf("FUJINET: SOFT RESET\n");
        ReportDevice(Dev, 0x0001, 0);
        FNStatus = RSP_STATUS;
        printf("FUJINET: Done.\n");
        break;
    case CMD_WRITE: // 0x03
        printf("FUJINET: WRITE\n");
        SetDCB(Dev, DCB_CMD_STAT, RSP_ACK + 0x0B);
        FNStatus = RSP_STATUS;
        printf("FUJINET: Done.\n");
        break;
    case CMD_READ: // 0x04
        printf("FUJINET: READ\n");
        printf("FUJINET: Done.\n");
        break;
    }

}
