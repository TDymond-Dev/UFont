#ifndef UFONT_TBL_CMAP
#define UFONT_TBL_CMAP

#include "parser.hpp"

namespace uf::detail {
	struct tbl_cmap {
		uint16_t version;
		uint16_t numTables; // Number of encoding tables that follow.

		struct EncodingRecord {
			uint16_t platformID; // Platform ID, 0 Unicode, 1 mac, 2 iso(deprec), 3 windows, 4 custom
			uint16_t encodingID; // Platform-specific encoding ID( 0 uni 1.0, 1 uni 1.1, 2 iso/iec, 3 unicode 2.0 bmp only, 4 unicode 2.0+ full, 5 unicode var sequence, 6 unicode full rep)
			uint32_t subtableOffset; // Byte offset from beginning of table to the subtable for this encoding.
		};
		std::vector<EncodingRecord> records;

		void parse(Parser& p, size_t offset) {
			p.set_position(offset);

			version = p.u16();
			numTables = p.u16();
			records = p.vec<EncodingRecord>(numTables);
			for (auto& record : records)
				record.platformID = p.u16(), record.encodingID = p.u16(), record.subtableOffset = p.u32();
		}
	};
}


#endif UFONT_TBL_CMAP