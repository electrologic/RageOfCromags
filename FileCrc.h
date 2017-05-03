#ifndef MARK_CRC_H
#define MARK_CRC_H

extern const unsigned short markCRCTable[];

inline unsigned short updcrc( unsigned char cp, unsigned short crc )
{
	return (unsigned short)( markCRCTable[ ( ( crc >> 8 ) & 255 ) ] ^ ( crc << 8 ) ^ cp );
}

unsigned short fileCrc16(char name[200], unsigned short initValue);

#endif
