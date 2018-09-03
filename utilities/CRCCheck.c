#include <utilities/CRCCheck.h>

/**
 * Usage [the_crc = CRCCCITT(buff, TEST_SIZE, 0xffff, 0);]
 * @param data
 * @param length
 * @param seed
 * @param final
 */

uint16_t CRCCCITT(unsigned char *data, uint32_t length, uint16_t seed,
		uint16_t final) {

	uint32_t count;
	uint32_t crc = seed;
	uint32_t temp;

	for (count = 0; count < length; ++count) {
		temp = (*data++ ^ (crc >> 8)) & 0xff;
		crc = crc_table[temp] ^ (crc << 8);
	}

	return (unsigned short) (crc ^ final);

}
