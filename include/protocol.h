#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

union HaierAc176Protocol
{
  uint8_t raw[22]; ///< The state in native form
  struct
  {
    // Byte 0
    uint8_t Prefix : 8;
    // Byte 1
    uint8_t Swing : 4;
    uint8_t Temp : 4; // 16C~30C
    // Byte 2
    uint8_t : 8;
    // Byte 3
    uint8_t : 1;
    uint8_t Health : 1;
    uint8_t : 3;
    uint8_t TimerMode : 3;
    // Byte 4
    uint8_t : 6;
    uint8_t Power : 1;
    uint8_t : 1;
    // Byte 5
    uint8_t OffTimerHrs : 5;
    uint8_t Fan : 3;
    // Byte 6
    uint8_t OffTimerMins : 6;
    uint8_t Turbo : 2;
    // Byte 7
    uint8_t OnTimerHrs : 5;
    uint8_t Mode : 3;
    // Byte 8
    uint8_t OnTimerMins : 6;
    uint8_t : 1;
    uint8_t Sleep : 1;
    // Byte 9
    uint8_t : 8;
    // Byte 10
    uint8_t : 8;
    // Byte 11
    uint8_t : 8;
    // Byte 12
    uint8_t Button : 4;
    uint8_t : 4;
    // Byte 13
    uint8_t Sum : 8;
    // Byte 14
    uint8_t Prefix2 : 8;
    // Byte 15
    uint8_t : 8;
    // Byte 16
    uint8_t : 6;
    uint8_t Fan2 : 2;
    // Byte 17
    uint8_t : 8;
    // Byte 18
    uint8_t : 8;
    // Byte 19
    uint8_t : 8;
    // Byte 20
    uint8_t : 8;
    // Byte 21
    uint8_t Sum2 : 8;
  };
};

#endif /* _PROTOCOL_H_ */