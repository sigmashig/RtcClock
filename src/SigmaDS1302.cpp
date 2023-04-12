#include "SigmaDS1302.hpp"


byte SigmaDS1302::bcdToDec(const byte bcd) {
    return (10 * ((bcd & 0xF0) >> 4) + (bcd & 0x0F));
}

// Returns the binary-coded decimal of 'dec'. Inverse of bcdToDec.
byte SigmaDS1302::decToBcd(const byte dec) {
    const byte tens = dec / 10;
    const byte ones = dec % 10;
    return (tens << 4) | ones;
}

bool SigmaDS1302::isValid() {
    return (cePin != 0 && datPin != 0 && sclkPin != 0);
}

byte SigmaDS1302::hourFromRegisterValue(const byte value) {
    byte adj;
    if (value & 128)  // 12-hour mode
        adj = 12 * ((value & 32) >> 5);
    else           // 24-hour mode
        adj = 10 * ((value & (32 + 16)) >> 4);
    return (value & 15) + adj;
}

#ifdef ESP8266

#define REG_SECONDS           0x80
#define REG_MINUTES           0x82
#define REG_HOUR              0x84
#define REG_DATE              0x86
#define REG_MONTH             0x88
#define REG_DAY               0x8A
#define REG_YEAR              0x8C
#define REG_WP                0x8E
#define REG_BURST             0xBE

SigmaDS1302::SigmaDS1302(DS1302_Pins& pins): SigmaRTC(RTC_DS1302),
cePin(pins.cePin), datPin(pins.datPin), sclkPin(pins.clkPin) {
    datDirection = INPUT;
    init();
}

SigmaDS1302::~SigmaDS1302() {}


void SigmaDS1302::init() {
    pinMode(cePin, OUTPUT);
    pinMode(sclkPin, OUTPUT);
    pinMode(datPin, datDirection);

    digitalWrite(cePin, LOW);
    digitalWrite(sclkPin, LOW);
}


bool SigmaDS1302::IsHalted() {
    prepareRead(REG_SECONDS);
    byte seconds = readIn();
    end();
    return (seconds & 0b10000000);
}

tm SigmaDS1302::GetTime() {
    tm t;

    prepareRead(REG_BURST);
    t.tm_sec = bcdToDec(readIn() & 0x7F);
    t.tm_min = bcdToDec(readIn() & 0x7F);
    t.tm_hour = hourFromRegisterValue(readIn());
    t.tm_mday = bcdToDec(readIn());
    t.tm_mon = bcdToDec(readIn()) - 1;
    t.tm_wday = bcdToDec(readIn());
    t.tm_year = bcdToDec(readIn());
    t.tm_isdst = -1;  // DST information not available.
    t.tm_yday = -1;   // Day of year not available.
    end();
    return t;
}

void SigmaDS1302::SetTime(tm& t) {
    //if (!isValid()) return;

    prepareWrite(REG_WP);
    writeOut(0b00000000);
    end();
    prepareWrite(REG_BURST);

    writeOut(decToBcd(t.tm_sec));
    writeOut(decToBcd(t.tm_min));
    writeOut(decToBcd(t.tm_hour));
    writeOut(decToBcd(t.tm_mday));
    writeOut(decToBcd(t.tm_mon + 1));
    writeOut(decToBcd(t.tm_wday));
    writeOut(decToBcd(t.tm_year));
    writeOut(0b10000000);

    // All clock registers *and* the WP register have to be written for the time
    // to be set.
    end();  // Write protection register.
}

/*
void SigmaDS1302::halt() {
    prepareWrite(REG_SECONDS);
    writeOut(0b10000000);
    end();
}
*/

void SigmaDS1302::prepareRead(byte address) {
    setDirection(OUTPUT);
    digitalWrite(cePin, HIGH);
    byte command = 0b10000001 | address;
    writeOut(command);
    setDirection(INPUT);
}


void SigmaDS1302::prepareWrite(byte address) {
    setDirection(OUTPUT);
    digitalWrite(cePin, HIGH);
    byte command = 0b10000000 | address;
    writeOut(command);
}


void SigmaDS1302::end() {
    digitalWrite(cePin, LOW);
}


byte SigmaDS1302::readIn() {
    byte bt = 0;

    for (byte b = 0; b < 8; b++) {
        if (digitalRead(datPin) == HIGH) bt |= 0x01 << b;
        nextBit();
    }

    return bt;
}


void SigmaDS1302::writeOut(byte value) {
    for (byte b = 0; b < 8; b++) {
        digitalWrite(datPin, (value & 0x01) ? HIGH : LOW);
        nextBit();
        value >>= 1;
    }
}

void SigmaDS1302::nextBit() {
    digitalWrite(sclkPin, HIGH);
    delayMicroseconds(1);

    digitalWrite(sclkPin, LOW);
    delayMicroseconds(1);
}


void SigmaDS1302::setDirection(int direction) {
    if (datDirection != direction) {
        datDirection = direction;
        pinMode(datPin, direction);
    }
}





#else
enum Register {
    kSecondReg = 0,
    kMinuteReg = 1,
    kHourReg = 2,
    kDateReg = 3,
    kMonthReg = 4,
    kDayReg = 5,
    kYearReg = 6,
    kWriteProtectReg = 7,

    // The RAM register space follows the clock register space.
    kRamAddress0 = 32
};

enum Command {
    kClockBurstRead = 0xBF,
    kClockBurstWrite = 0xBE,
    kRamBurstRead = 0xFF,
    kRamBurstWrite = 0xFE
};

// Establishes and terminates a three-wire SPI session.
class SPISession {
public:
    SPISession(const byte ce_pin, const byte io_pin, const byte sclk_pin)
        : ce_pin(ce_pin), io_pin(io_pin), sclk_pin(sclk_pin) {
        digitalWrite(sclk_pin, LOW);
        digitalWrite(ce_pin, HIGH);
        delayMicroseconds(4);  // tCC
    }
    ~SPISession() {
        digitalWrite(ce_pin, LOW);
        delayMicroseconds(4);  // tCWH
    }

private:
    const int ce_pin;
    const int io_pin;
    const int sclk_pin;
};





SigmaDS1302::SigmaDS1302(DS1302_Pins& pins): SigmaRTC(RTC_DS1302),
cePin(pins.cePin), datPin(pins.datPin), sclkPin(pins.clkPin) {
    if (!isValid()) {
        digitalWrite(cePin, LOW);
        pinMode(cePin, OUTPUT);
        pinMode(datPin, INPUT);
        digitalWrite(sclkPin, LOW);
        pinMode(sclkPin, OUTPUT);
    }
}

void SigmaDS1302::writeOut(const byte value, bool readAfter) {
    pinMode(datPin, OUTPUT);

    for (int i = 0; i < 8; ++i) {
        digitalWrite(datPin, (value >> i) & 1);
        delayMicroseconds(1);
        digitalWrite(sclkPin, HIGH);
        delayMicroseconds(1);

        if (readAfter && i == 7) {
            // We're about to read data -- ensure the pin is back in input mode
            // before the clock is lowered.
            pinMode(datPin, INPUT);
        } else {
            digitalWrite(sclkPin, LOW);
            delayMicroseconds(1);
        }
    }
}

byte SigmaDS1302::readIn() {
    byte input_value = 0;
    byte bit = 0;
    pinMode(datPin, INPUT);

    // Bits from the DS1302 are output on the falling edge of the clock
    // cycle. This is called after readIn (which will leave the clock low) or
    // writeOut(..., true) (which will leave it high).
    for (int i = 0; i < 8; ++i) {
        digitalWrite(sclkPin, HIGH);
        delayMicroseconds(1);
        digitalWrite(sclkPin, LOW);
        delayMicroseconds(1);

        bit = digitalRead(datPin);
        input_value |= (bit << i);  // Bits are read LSB first.
    }
    Serial.print("readIn=");Serial.println(input_value);
    return input_value;
}

byte SigmaDS1302::readRegister(byte reg) {
    const SPISession s(cePin, datPin, sclkPin);

    const byte cmd_byte = (0x81 | (reg << 1));
    writeOut(cmd_byte, true);
    return readIn();
}

void SigmaDS1302::writeRegister(byte reg, byte value) {
    const SPISession s(cePin, datPin, sclkPin);

    const byte cmd_byte = (0x80 | (reg << 1));
    writeOut(cmd_byte);
    writeOut(value);
}

void SigmaDS1302::writeProtect(const bool enable) {
    writeRegister(kWriteProtectReg, (enable << 7));
}

void SigmaDS1302::halt(const bool enable) {
    byte sec = readRegister(kSecondReg);
    sec &= ~(1 << 7);
    sec |= (enable << 7);
    writeRegister(kSecondReg, sec);
}

tm SigmaDS1302::GetTime() {
    tm t;
    Serial.println("Point0");
    if (!isValid()) {
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_mday = 0;
        t.tm_mon = 0;
        t.tm_wday = 0;
        t.tm_year = 0;
        t.tm_isdst = -1;  // DST information not available.
        t.tm_yday = -1;   // Day of year not available.
        return t;
    }
    Serial.println("Point1");
    const SPISession s(cePin, datPin, sclkPin);
    writeOut(kClockBurstRead, true);
    t.tm_sec = bcdToDec(readIn() & 0x7F);
    t.tm_min = bcdToDec(readIn());
    t.tm_hour = hourFromRegisterValue(readIn());
    t.tm_mday = bcdToDec(readIn());
    t.tm_mon = bcdToDec(readIn()) - 1;
    t.tm_wday = bcdToDec(readIn());
    t.tm_year = bcdToDec(readIn());
    t.tm_isdst = -1;  // DST information not available.
    t.tm_yday = -1;   // Day of year not available.
    return t;
}

void SigmaDS1302::SetTime(tm& t) {
    if (!isValid()) return;
    // We want to maintain the Clock Halt flag if it is set.
    const byte ch_value = readRegister(kSecondReg) & 0x80;

    const SPISession s(cePin, datPin, sclkPin);

    writeOut(kClockBurstWrite);
    writeOut(ch_value | decToBcd(t.tm_sec));
    writeOut(decToBcd(t.tm_min));
    writeOut(decToBcd(t.tm_hour));
    writeOut(decToBcd(t.tm_mday));
    writeOut(decToBcd(t.tm_mon + 1));
    writeOut(decToBcd(t.tm_wday));
    writeOut(decToBcd(t.tm_year));
    // All clock registers *and* the WP register have to be written for the time
    // to be set.
    writeOut(0);  // Write protection register.
}
#endif