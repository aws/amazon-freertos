/*
 * Copyright (C) 2008-2014 Marvell International Ltd.
 * All Rights Reserved.
 */

#include <stdint.h>

#ifndef __BOOTROM_HEADER_H__
#define __BOOTROM_HEADER_H__

#if defined(CONFIG_CPU_MW300)
typedef struct ES {
	uint8_t oem_pub_key[294];
	uint8_t digital_sig[256];
	uint32_t encrypted_img_len;
	uint8_t nonce[16];
} __attribute__((packed)) es_t;

typedef struct Headers {
	struct bootInfoHeader {
		uint32_t rsvd1;
		uint32_t rsvd2;
		union {
			struct {
				unsigned dmaEn:1;
				unsigned reserved:7;
				unsigned clockSel:2;
				unsigned reserved1:7;
				unsigned flashintfPrescalarOff:1;
				unsigned flashintfPrescalar:5;
				unsigned reserved2:9;
			};
			uint32_t reg;
		} commonCfg0;
		uint32_t magicCode;
		uint32_t entryAddr;
		uint32_t sfllCfg;
		uint32_t rsvd3;
		uint32_t flashcCfg0;
		uint32_t flashcCfg1;
		uint32_t flashcCfg2;
		uint32_t flashcCfg3;
		uint32_t rsvd4[8];
		uint32_t CRCCheck;
	} bh;

	struct SectionHeader {
		uint32_t rsvd1;
		uint32_t codeLen;
		uint32_t rsvd2;
		uint32_t destStartAddr;
		uint32_t rsvd3;
		union {
			struct {
				unsigned reserved:31;
				unsigned emptyCfg:1;
			};
			uint32_t reg;
		} bootCfg0;
		union {
			struct {
				unsigned sramModeEn:1;
				unsigned reserved:5;
				unsigned flashReadMode:3;
				unsigned reserved1:4;
				unsigned LEDEnable:1;
				unsigned locationMode:1;
				unsigned flashProgMode:1;
				unsigned reserved2:4;
				unsigned flashcOff:1;
				unsigned reserved3:11;
			};
			uint32_t reg;
		} bootCfg1;
		uint32_t rsvd4[4];
		uint32_t CRCCheck;
	} sh;
} __attribute__((packed)) hdr_t;

#endif
#endif /* __BOOTROM_HEADER_H__ */
