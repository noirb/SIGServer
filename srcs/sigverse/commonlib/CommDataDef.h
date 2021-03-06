/*
 * Modified by okamoto on 2011-03-31
 */

#ifndef CommDataDef_h
#define CommDataDef_h

#define COMM_DATA_PACKET_TOKEN_DATASIZE_BYTES 4

#define COMM_DATA_PACKET_START_TOKEN          0xabcd
#define COMM_DATA_PACKET_END_TOKEN            0xdcba
#define COMM_DATA_HEADER_MAX_SIZE             (10 + 100)
#define COMM_DATA_HEADER_MIN_SIZE             8
#define COMM_DATA_PACKET_SIZE_OFFSET          2
#define COMM_DATA_FOOTER_SIZE                 2

#define COMM_DATA_FORWARD                     0x01
#define COMM_DATA_FORWARD_RETURN_IMMEDIATE    0x02
#define COMM_DATA_FORWARD_REACH_RADIUS        0x04

#endif /* CommDataDef_h */
 
