#include "prop.hpp"

// Define static member variables accessed across ISR
volatile unsigned long PropSystem::_lastPulseTime = 0;
volatile unsigned long PropSystem::_pulsePeriod = 0;
volatile unsigned long PropSystem::_pulseCount = 0;
volatile unsigned long PropSystem::_accumulatedPeriodUs = 0;
volatile unsigned int PropSystem::_pulsesThisInterval = 0;

PropSystem::PropSystem(unsigned long transmitIntervalMs, uint8_t bladeCount)
    : _transmitIntervalMs(transmitIntervalMs > 0 ? transmitIntervalMs : DefaultIntervalMs),
      _bladeCount(bladeCount > 0 ? bladeCount : DefaultBladeCount),
      _sampleCount(0),
      _lastTransmitTime(0),
      _lastRpm(0.0f),
      _lastFrequency(0.0f),
      _lastPeriodUs(0),
      _startTime(0)
{
}

void PropSystem::init()
{
    delay(500); // Allow USB connection to stabilize
    _startTime = millis(); // Establish t=0 for session

    // Turn LED off
    pinMode(LedPin, OUTPUT);
    setLed(false);

    // Direct Port Manipulation for Diagnostic Pin 7 (Port H, Pin 4 on ATmega2560)
    DDRH |= (1 << 4);   // Set Pin 7 direction to OUTPUT
    PORTH &= ~(1 << 4); // Start Pin 7 LOW

    // Configure IR detector pin with internal pullup
    pinMode(IrInterruptPin, INPUT_PULLUP);

    // Attach hardware interrupt INT4 on falling edge (propeller breaks infrared beam)
    attachInterrupt(digitalPinToInterrupt(IrInterruptPin), PropSystem::pulseISR, FALLING);

    // Print header
    Serial.println(F("# Propeller Speed Optical Tachometer System"));
    Serial.println(F("Time_s,Sample,RPM,Frequency_Hz,Period_us"));

    transmitTelemetry();
}

void PropSystem::pulseISR()
{
    // Drive Diagnostic Pin 7 HIGH instantly (1 clock cycle / 62.5ns)
    PORTH |= (1 << 4);

    unsigned long currentTime = micros();

    if (_lastPulseTime > 0)
    {
        unsigned long interval = currentTime - _lastPulseTime;

        if (interval > StallTimeoutUs)
        {
            // Rotor was previously stalled, establish new baseline without corrupting accumulator
            _pulseCount++;
        }
        else if (interval > MinPulsePeriodUs)
        {
            _pulsePeriod = interval;
            _pulseCount++;
            _accumulatedPeriodUs += interval;
            _pulsesThisInterval++;
        }
    }
    else
    {
        _pulseCount++;
    }
    _lastPulseTime = currentTime;

    // Drive Diagnostic Pin 7 LOW instantly right before exit (1 clock cycle / 62.5ns)
    PORTH &= ~(1 << 4);
}

void PropSystem::process()
{
    unsigned long currentMillis = millis();

    // LED is ON when beam is blocked, OFF when clear
    setLed(digitalRead(IrInterruptPin) == HIGH);

    // Periodic telemetry calculation and transmission
    if (currentMillis - _lastTransmitTime >= _transmitIntervalMs)
    {
        _lastTransmitTime = currentMillis;

        unsigned long accumPeriod = 0;
        unsigned int pulseCount = 0;
        unsigned long lastPeriod = 0;
        unsigned long timeSinceLastPulse = 0;

        // Atomically snapshot and reset window accumulator
        noInterrupts();
        accumPeriod = _accumulatedPeriodUs;
        pulseCount = _pulsesThisInterval;
        _accumulatedPeriodUs = 0;
        _pulsesThisInterval = 0;
        lastPeriod = _pulsePeriod;
        timeSinceLastPulse = micros() - _lastPulseTime;
        interrupts();

        // Check for stopped rotor condition (>1.0s without pulse)
        if (timeSinceLastPulse > StallTimeoutUs || _lastPulseTime == 0)
        {
            _lastRpm = 0.0f;
            _lastFrequency = 0.0f;
            _lastPeriodUs = 0;
        }
        else if (pulseCount > 0 && accumPeriod > 0)
        {
            // Window average across all pulses in this 100ms interval
            _lastPeriodUs = accumPeriod / pulseCount;
            _lastFrequency = 1000000.0f / static_cast<float>(_lastPeriodUs);
            _lastRpm = (_lastFrequency * 60.0f) / static_cast<float>(_bladeCount);
        }
        else if (lastPeriod > 0 && timeSinceLastPulse < StallTimeoutUs)
        {
            // Low-speed rotation (< 1 pulse in this specific 100ms window)
            _lastPeriodUs = lastPeriod;
            _lastFrequency = 1000000.0f / static_cast<float>(_lastPeriodUs);
            _lastRpm = (_lastFrequency * 60.0f) / static_cast<float>(_bladeCount);
        }

        transmitTelemetry();
    }
}

void PropSystem::transmitTelemetry()
{
    _sampleCount++;
    float timeSec = static_cast<float>(millis() - _startTime) / 1000.0f;

    Serial.print(timeSec, 2);
    Serial.print(F(","));
    Serial.print(_sampleCount);
    Serial.print(F(","));
    Serial.print(_lastRpm, 1);
    Serial.print(F(","));
    Serial.print(_lastFrequency, 2);
    Serial.print(F(","));
    Serial.println(_lastPeriodUs);
}
