#include "src/prop.hpp"

#ifndef PIO_BUILD
PropSystem propSystem;

void setup()
{
    Serial.begin(115200);
    while (!Serial) { ; }
    propSystem.init();
}

void loop()
{
    propSystem.process();
}
#endif
