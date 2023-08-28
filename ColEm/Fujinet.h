#ifndef FUJI_VERSION



#define FUJI_VERSION "f0.1"
#define NET_DEV 0x09

    // Didn't want to do it this way, but
    // I also don't want to change the original code
    // so include this into AdamNet.c

void UpdateFujinet(byte Dev, int V);
void UpdateFujiClock(byte Dev, int V);
#endif


