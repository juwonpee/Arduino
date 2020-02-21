extern "C"
{
#include "ArdOS.h"
}

TOSSema sema1, sema2;

void task1(void *p)
{
    while(1)
    {
        for(int i=0; i<5; i++)
        {
            digitalWrite(6, HIGH);
            OSSleep(125);
            digitalWrite(6, LOW);
            OSSleep(125);
        }

        // Release sema2
        OSGiveSema(&sema2);

        // Wait on sema1
        OSTakeSema(&sema1);
    }
}

void task2(void *p)
{
    while(1)
    {
        // Wait on sema2
        OSTakeSema(&sema2);

        for(int i=0; i<5; i++)
        {
            digitalWrite(9, HIGH);
            OSSleep(250);
            digitalWrite(9, LOW);
            OSSleep(250);
        }

        // Release sema1
        OSGiveSema(&sema1);
    }
}

unsigned long t1s[30], t2s[30];

void setup()
{
    OSInit();

    OSInitSema(&sema1, 0, 1);
    OSInitSema(&sema2, 0, 1);

    pinMode(6, OUTPUT);
    pinMode(9, OUTPUT);

    OSCreateTask(0, &t1s[29], task1, NULL);
    OSCreateTask(1, &t2s[29], task2, NULL);

    // Start the OS
    OSRun();
}

void loop()
{
    // Empty
}
