#ifndef UFONT_TBL_HEAD
#define UFONT_TBL_HEAD

#include "parser.hpp"

namespace uf::detail {
	struct tbl_head {
		uint16_t major_version; // 0x00010000 if (version 1.0)
		uint16_t minor_version; // 0x00010000 if (version 1.0)
		uint32_t fontRevision; // set by font manufacturer
		uint32_t checkSumAdjusted; // To compute: set it to 0, calculate the checksum for the 'head' table and put it in the table directory, sum the entire font as a uint32_t, then store 0xB1B0AFBA - sum. (The checksum for the 'head' table will be wrong as a result. That is OK; do not reset it.)
		uint32_t magicNumber;
		uint16_t flags;
		uint16_t unitsPerEm;
		uint64_t created;
		uint64_t modified;
		int16_t xMin;
		int16_t yMin;
		int16_t xMax;
		int16_t yMax;
		uint16_t macStyle;
		uint16_t lowestRecPPEM;
		int16_t fontDirectionHint;
		int16_t indexToLocFormat;
		int16_t glyphDataFormat;

		void parse(Parser& p, uint32_t offset = 0) {
			p.set_position(offset);

			major_version = p.u16();
			minor_version = p.u16();
			fontRevision = p.u32();
			checkSumAdjusted = p.u32();
			magicNumber = p.u32();
			flags = p.u16();
			unitsPerEm = p.u16();
			created = p.u64();
			modified = p.u64();
			xMin = p.i16();
			yMin = p.i16();
			xMax = p.i16();
			yMax = p.i16();
			macStyle = p.u16();
			lowestRecPPEM = p.u16();
			fontDirectionHint = p.i16();
			indexToLocFormat = p.i16();
			glyphDataFormat = p.i16();
		}
	};
}

#endif // UFONT_TBL_HEAD