#include "defs.h"

void chEndianess(int32& nr)
{
  nr = (nr >> 24) | ((nr<<8) & 0xFF) | ((nr>>8) & 0xFF) | (nr << 24);
}

bool isLittleEndian()
{
    int16 nr = 0x01;
    char *p = (char*)&nr;
    return (p[0] == 1);
}
