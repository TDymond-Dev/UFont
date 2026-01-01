#ifndef UFONT_TBL_KERN
#define UFONT_TBL_KERN

#include "parser.hpp"

namespace uf::detail {
	struct tbl_kern {
		// Format 0 is the only subtable format supported by Windows.
		struct kern_subtable_0 {
			uint16_t nPairs; // This gives the number of kerning pairs in the table.
			uint16_t searchRange;
			uint16_t entrySelector;
			uint16_t rangeShift;

			std::vector<uint16_t> left; // The glyph index for the left-hand glyph in the kerning pair.
			std::vector<uint16_t> right; // The glyph index for the right-hand glyph in the kerning pair.
			std::vector<int32_t> value; // The kerning value for the above pair, in FUnits. If this value is greater than zero, the characters will be moved apart. If this value is less than zero, the character will be moved closer together.
		};

		struct kern_subtable_2 {

		};

		struct kern_header {
			uint16_t version; //	Kern subtable version number
			uint16_t length; //		Length of the subtable, in bytes(including this header).
			uint16_t coverage; //	What type of information is contained in this table.

			uint8_t format; // 0 format 0, 2 format 2
			uint16_t index; // index into the array of tables
		};

		uint16_t version;
		uint16_t nTables;
		std::vector<kern_header> kern_headers;
		std::vector<kern_subtable_0> subtables_0;
		std::vector<kern_subtable_2> subtables_2;

		void parse(Parser& p, size_t offset) {
			p.set_position(offset);

			version = p.u16();
			nTables = p.u16();

			for (int i = 0; i < nTables; i++) {
				kern_header header;
				header.version = p.u16();
				header.length = p.u16();
				header.coverage = p.u16();
				header.format = (uint16_t)(header.coverage >> 8);
				header.index = (header.format == 0) ? subtables_0.size() : subtables_2.size();

				if (header.format == 0) { // subtable 0
					kern_subtable_0 subtable_0;
					subtable_0.nPairs = p.u16();
					subtable_0.searchRange = p.u16();
					subtable_0.entrySelector = p.u16();
					subtable_0.rangeShift = p.u16();

					subtable_0.left = p.vec<uint16_t>(subtable_0.nPairs);
					subtable_0.right = p.vec<uint16_t>(subtable_0.nPairs);
					subtable_0.value = p.vec<int32_t>(subtable_0.nPairs);

					for (int i = 0; i < subtable_0.nPairs; i++) {
						uint32_t sign = p.u32();
						subtable_0.left[i] = (uint16_t)(sign >> 16), subtable_0.right[i] = (uint16_t)(sign >> 0);
						subtable_0.value[i] = p.i16();
					}
				}
			}
		}
	};
}


#endif /* UFONT_TBL_KERN */