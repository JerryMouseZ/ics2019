#include "trap.h"
int main()
{
    volatile int i = 1;
    while (i > 0)
    {
        i += 1;
    }
    return 0;
}