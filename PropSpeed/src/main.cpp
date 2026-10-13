#include <Arduino.h>
#include "prop.hpp"

#ifdef PIO_BUILD
PropSystem propSystem;

int main(void)
{
    init(); // Arduino hardware initialization

    Serial.begin(115200);
    while (!Serial) { ; }
    propSystem.init();

    while (true)
    {
        propSystem.process();
    }

    return 0;
}
#endif
