#pragma once

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Arduino basic types
typedef unsigned char byte;
typedef bool boolean;

// Math constants
#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105

// Arduino constants
#define HIGH 0x1
#define LOW 0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

#ifndef PROGMEM
#define PROGMEM
#endif

// Memory functions
#define pgm_read_byte(addr) (*(const uint8_t *)(addr))
#define pgm_read_word(addr) (*(const uint16_t *)(addr))
#define pgm_read_dword(addr) (*(const uint32_t *)(addr))

// Arduino functions
inline void pinMode(uint8_t pin, uint8_t mode) {
  (void)pin;
  (void)mode;
}
inline void digitalWrite(uint8_t pin, uint8_t val) {
  (void)pin;
  (void)val;
}
inline int digitalRead(uint8_t pin) {
  (void)pin;
  return LOW;
}
inline void delay(unsigned long ms) { (void)ms; }
inline void delayMicroseconds(unsigned int us) { (void)us; }
inline unsigned long millis(void) { return 0; }
inline unsigned long micros(void) { return 0; }
inline void yield(void) {}
inline long random(long max) { return rand() % max; }
inline long random(long min, long max) { return min + (rand() % (max - min)); }

// Pin mapping functions (stubs for PC)
inline uint8_t digitalPinToBitMask(uint8_t pin) {
  (void)pin;
  return 0x01;
}

// String conversion functions
inline void ltoa(long val, char *s, int radix) {
  if (radix == 10)
    snprintf(s, 32, "%ld", val);
  else if (radix == 16)
    snprintf(s, 32, "%lx", val);
  else
    snprintf(s, 32, "%ld", val);
}

// Math functions
#define abs(x) ((x) > 0 ? (x) : -(x))
#define constrain(amt, low, high)                                              \
  ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define round(x) ((x) >= 0 ? (long)((x) + 0.5) : (long)((x) - 0.5))
#define sq(x) ((x) * (x))

// Bit operations
#define lowByte(w) ((uint8_t)((w) & 0xff))
#define highByte(w) ((uint8_t)((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue)                                         \
  (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

// String class
class String {
public:
  String() : str(nullptr), len(0) {}
  String(const char *s) {
    if (s) {
      len = strlen(s);
      str = (char *)malloc(len + 1);
      strcpy(str, s);
    } else {
      str = nullptr;
      len = 0;
    }
  }
  String(const String &s) {
    if (s.str) {
      len = s.len;
      str = (char *)malloc(len + 1);
      strcpy(str, s.str);
    } else {
      str = nullptr;
      len = 0;
    }
  }
  ~String() {
    if (str)
      free(str);
  }

  const char *c_str() const { return str ? str : ""; }
  size_t length() const { return len; }

  void toCharArray(char *buf, unsigned int bufsize,
                   unsigned int index = 0) const {
    if (!bufsize || !buf)
      return;
    if (index >= len) {
      buf[0] = 0;
      return;
    }
    unsigned int n = bufsize - 1;
    if (n > len - index)
      n = len - index;
    strncpy(buf, str + index, n);
    buf[n] = 0;
  }

  bool operator==(const char *cstr) const {
    if (!str)
      return !cstr || !cstr[0];
    if (!cstr)
      return !str[0];
    return strcmp(str, cstr) == 0;
  }

  bool operator==(const String &s) const {
    if (!str)
      return !s.str || !s.str[0];
    if (!s.str)
      return !str[0];
    return strcmp(str, s.str) == 0;
  }

private:
  char *str;
  size_t len;
};

// Print class
class Print {
public:
  virtual ~Print() {}
  virtual size_t write(uint8_t c) = 0;
  virtual size_t write(const uint8_t *buffer, size_t size) {
    size_t n = 0;
    while (size--) {
      if (write(*buffer++))
        n++;
      else
        break;
    }
    return n;
  }

  size_t print(const char *str) {
    return write((const uint8_t *)str, strlen(str));
  }
  size_t print(int n) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", n);
    return print(buf);
  }
  size_t println(const char *str) {
    size_t n = print(str);
    n += write('\r');
    n += write('\n');
    return n;
  }
  size_t println() {
    size_t n = write('\r');
    n += write('\n');
    return n;
  }
};

// Serial stub
class SerialClass : public Print {
public:
  void begin(unsigned long baud) { (void)baud; }
  size_t write(uint8_t c) override {
    putchar(c);
    return 1;
  }
};

extern SerialClass Serial;

// SPI stub class
class SPIClass {
public:
  void begin() {}
  void end() {}
  void beginTransaction(uint32_t freq) { (void)freq; }
  void endTransaction() {}
  void setFrequency(uint32_t freq) { (void)freq; }
  uint8_t transfer(uint8_t data) { return data; }
  uint16_t transfer16(uint16_t data) { return data; }
  void transfer(void *buf, size_t count) {
    (void)buf;
    (void)count;
  }
};

extern SPIClass SPI;

// SPI modes
#define SPI_MODE0 0x00
#define SPI_MODE1 0x01
#define SPI_MODE2 0x02
#define SPI_MODE3 0x03
