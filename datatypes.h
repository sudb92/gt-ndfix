#ifndef DATATYPES_H
#define DATATYPES_H
#include <cstdint>
#include <TRandom3.h>
const long int S800_TIMESTAMP=0x5803;
const long int S800_EVENTNUMBER=0x5804;
const long int S800_TRIGGER=0x5801;
const long int S800_TOF=0x5802;
const long int S800_SCINT=0x5810;
const long int S800_IC1=0x5820;
const long int S800_IC2=0x5821;
const long int S800_CRDC1=0x5840;
const long int S800_CRDC2=0x5841;
const long int S800_CRDC_ANODE=0x5845;
const long int S800_HODO=0x58B0;
const long int S800_TPPAC1=0x5870;
const long int S800_TPPAC2=0x5871;
const long int S800_OBJ_PIN=0x58A0;
const long int S800_FP_PIN_STACK=0x5805;
const long int S800_GALOTTE=0x58D0;
const long int S800_LABR=0x58E0;
const long int S800_MTDC=0x58F0;

const long int S800MESY_E1UP  =0x00;
const long int S800MESY_E1DOWN=0x01;
const long int S800MESY_XFP   =0x02;
const long int S800MESY_OBJ   =0x03;
const long int S800MESY_RF    =0x05;
const long int S800MESY_CRDC1 =0x06;
const long int S800MESY_CRDC2 =0x07;
const long int S800MESY_HODO  =0x0C;
const long int S800MESY_E1REF =0x0F;

const  int S800_II_TRACK_CHANNELS     =256;
const  int S800_II_TRACK_MAX_WIDTH    =32;
const  int S800_II_TRACK_PARAMETERS   =6;
const  int S800_II_TRACK_COEFFICIENTS =80;
const  int S800_TRACK_PARAMETERS      =6;
const  int S800_TRACK_COEFFICIENTS    =80;

const long int MAX_M_WIDTH_S800=4; //maximum m_width expected to be seen in s800. all seen m_widths should be < this value, NOT <= note.

struct GEBHeader {
    int32_t type;
    int32_t length; /* length of payload following the header, in bytes */
    int64_t timestamp;
};

struct gheader {
    int length;
    long int timestamp;
};

const long int MAX_INTPTS=16;
const long int BUF_LEN=8192;
const long int DEFAULT_NULL=-987654321;
const long int MAX_ORRUBA_CHANS=1200;
const long int MAXNWIRES_TRACK=16;

static TRandom3 rnd(0);
#endif
