#pragma once
#include <Arduino.h>

class PropSystem
{
    public:
        explicit PropSystem(unsigned long transmitIntervalMs = DefaultIntervalMs, uint8_t bladeCount = DefaultBladeCount);

        void init();
        void process();
        void transmitTelemetry();

        // Hardware Interrupt Service Routine (INT4 on Pin 2)
        static void pulseISR();

    private:
        // Hardware
        static constexpr uint8_t IrInterruptPin {2};           // INT4 input
        static constexpr uint8_t DebugPin       {7};           // Debug output (PH4)
        static constexpr uint8_t LedPin         {LED_BUILTIN}; // Onboard LED

        void setLed(const bool state) { digitalWrite(LedPin, state ? HIGH : LOW); }

        // Timing & configuration
        static constexpr unsigned long DefaultIntervalMs {100};     // 100 ms transmission interval (10 Hz)
        static constexpr uint8_t DefaultBladeCount       {3};       // Propeller blade count
        static constexpr unsigned long StallTimeoutUs    {1000000}; // 1.0s without pulse = rotor stopped (0 RPM)
        static constexpr unsigned long MinPulsePeriodUs  {50};      // Filter sub-50us switch chatter

        // Shared static state accessed in ISR
        static volatile unsigned long _lastPulseTime;
        static volatile unsigned long _pulsePeriod;
        static volatile unsigned long _pulseCount;
        static volatile unsigned long _accumulatedPeriodUs;
        static volatile unsigned int _pulsesThisInterval;

        // Executive state
        unsigned long _transmitIntervalMs;
        uint8_t _bladeCount;
        unsigned long _sampleCount;
        unsigned long _lastTransmitTime;

        float _lastRpm;
        float _lastFrequency;
        unsigned long _lastPeriodUs;
        unsigned long _startTime;
};
