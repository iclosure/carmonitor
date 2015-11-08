#ifndef _CNI_PROTOCOL_H_
#define _CNI_PROTOCOL_H_

#include <QMetaType>

/*****************************
 * Serialport
 *****************************/

const quint8 SerialHeader[] = {0xAA, 0x55};
const quint8 SerialTail[] = {0x9F};

#pragma pack(push)
#pragma pack(1)

// sizeof(...) => 37 bytes
struct SerialPortSend {
    quint16 header;
    quint8  length;
    quint8  index;
    quint32 lWheelSpd;  // left-wheel-speed
    quint32 rWheelSpd;  // right-wheel-speed
    struct {
        quint8 lTracksip : 1;
        quint8 rTracksip : 1;
        quint8 lPowerSw  : 1;
        quint8 rPowerSw  : 1;
        quint8           : 4;
    } ctrlWord;         // control-word
    quint8  data[15];   // reserved
    quint8  sum;        // parity sum
    quint8  tail;
};

// sizeof(...) => 36 bytes
struct SerialPortRecv
{
    quint16 header;
    quint8 length;
    quint8 index;

    quint16 lMBrakeP;   // left-major-brake-press
    quint16 lABrakeP;   // left-assistant-brake-press
    quint16 rMBrakeP;   // right-major-brake-press
    quint16 rABrakeP;   // right-assistant-brake-press

    quint16 lMRotateR;   // left-major-Rotate-rate
    quint16 lARotateR;   // left-assistant-Rotate-rate
    quint16 rMRotateR;   // right-major-Rotate-rate
    quint16 rARotateR;   // right-assistant-Rotate-rate

    quint32 rWheelSpd;  // right-wheel-speed
    quint32 lWheelSpd;  // left-wheel-speed

    quint8  data[6];    // reserved
    quint8  sum;        // parity sum
    quint8  tail;
};

Q_DECLARE_METATYPE(SerialPortSend)
Q_DECLARE_METATYPE(SerialPortRecv)

#pragma pack()
#pragma pack(pop)

#endif // _CNI_PROTOCOL_H_
