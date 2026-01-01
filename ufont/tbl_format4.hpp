#ifndef UFONT_TBL_FORMAT4
#define UFONT_TBL_FORMAT4

#include "parser.hpp"
#include <iostream>

namespace uf::detail {
	struct Format4 {
		uint16_t format; // Format number is set to 4.
		uint16_t length; // This is the length in bytes of the subtable.
		uint16_t language;
		uint16_t segCountX2; //  2 × segCount;
		uint16_t searchRange; //	Maximum power of 2 less than or equal to segCount, times 2 ((2 * *floor(log2(segCount))) * 2, where “ * *” is an exponentiation operator)
		uint16_t entrySelector;// Log2 of the maximum power of 2 less than or equal to segCount(log2(searchRange / 2), which is equal to floor(log2(segCount)))
		uint16_t rangeShift; //	segCount times 2, minus searchRange((segCount * 2) - searchRange)
		std::vector<uint16_t> endCode; //[segCount]	End characterCode for each segment, last = 0xFFFF.
		uint16_t reservedPad; //	Set to 0.
		std::vector<uint16_t> startCode; //[segCount]	Start character code for each segment.
		std::vector<int16_t>  idDelta; //[segCount]	Delta for all character codes in segment.
		std::vector<uint16_t> idRangeOffsets; //[segCount]	Offsets into glyphIdArray or 0
		std::vector<uint16_t> glyphIdArray; //	Glyph index array(arbitrary length)
		std::vector<std::pair<uint16_t, uint16_t>> glyphMap;

		uint16_t retrieve_glyf(int16_t letter) {
			uint16_t index = UINT16_MAX;
			for (auto pr : glyphMap) {
				if (pr.first == letter)
					index = pr.second;
			}
			return index;
		}

		void parse(Parser& p, uint32_t offset, uint32_t length, bool& formatted) {
			auto a1 = p.rd_position();

			format = p.u16();
			if (format == 4) {
			}
			else {
				formatted = false;
				std::cout << "Unsupported format : ${format}. Required : 4.\n";
				return;
				//throw "Unsupported format : ${format}. Required : 4.";
			}

			length = p.u16();
			language = p.u16();
			segCountX2 = p.u16();
			searchRange = p.u16();
			entrySelector = p.u16();
			rangeShift = p.u16();

			endCode = p.vec<uint16_t>(segCountX2 / 2);
			for (auto& ec : endCode)
				ec = p.u16();

			// resever pad
			reservedPad = p.u16();

			startCode = p.vec<uint16_t>(segCountX2 / 2);
			for (auto& sc : startCode)
				sc = p.u16();

			idDelta = p.vec<int16_t>(segCountX2 / 2);
			for (auto& id : idDelta)
				id = p.i16();

			auto idRangeOffsetStart = p.rd_position();
			idRangeOffsets = p.vec<uint16_t>(segCountX2 / 2);
			for (auto& idRangeOffset : idRangeOffsets)
				idRangeOffset = p.u16();

			auto offset2 = p.rd_position();

			auto remBytes = length - (offset2 - a1);
			glyphIdArray = p.vec<uint16_t>(remBytes / 2);
			for (int i = 0; i < (remBytes / 2); i++) {
				glyphIdArray[i] = p.u16();
			}

			for (auto i = 0; i < (segCountX2 / 2) - 1; i++) {
				auto glyphIndex = 0;
				auto const i_endCode = endCode[i];
				auto const i_startCode = startCode[i];
				auto const i_idDelta = idDelta[i];
				auto const i_idRangeOffset = idRangeOffsets[i];

				for (auto c = i_startCode; c <= i_endCode; c++) {
					if (i_idRangeOffset != 0) {
						auto const startCodeOffset = (c - i_startCode) * 2;
						auto const currentRangeOffset = i * 2; // 2 because the numbers are 2 byte big.

						auto glyphIndexOffset =
							idRangeOffsetStart + // where all offsets started
							currentRangeOffset + // offset for the current range
							i_idRangeOffset + // offset between the id range table and the glyphIdArray[]
							startCodeOffset; // gets us finally to the character

						p.set_position(glyphIndexOffset);
						glyphIndex = p.u16();
						if (glyphIndex != 0) {
							// & 0xffff is modulo 65536.
							glyphIndex = (glyphIndex + i_idDelta) & 0xffff;
						}
					}
					else {
						glyphIndex = (c + i_idDelta) & 0xffff;
					}

					//glyphMap[i][c] = glyphIndex;
					glyphMap.push_back({ c, glyphIndex });
				}
			}

			formatted = true;
		}
	};
}


#endif UFONT_TBL_FORMAT4