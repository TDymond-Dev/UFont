#ifndef UFONT_TBL_DIRECTORY
#define UFONT_TBL_DIRECTORY

#include "parser.hpp"
#include <unordered_map>

namespace uf::detail {
	struct tbl_directory {
		struct record {
			uint32_t checksum;
			uint32_t offset;
			uint32_t length;
		};
		uint32_t sfntVersion; // 0x00010000 or 0x4F54544F ('OTTO') — see below.
		uint16_t numTables; // Number of tables.
		uint16_t searchRange; // Maximum power of 2 less than or equal to numTables, times 16 ((2**floor(log2(numTables))) * 16, where “**” is an exponentiation operator).
		uint16_t entrySelector; // Log2 of the maximum power of 2 less than or equal to numTables (log2(searchRange/16), which is equal to floor(log2(numTables))).
		uint16_t rangeShift; // numTables times 16, minus searchRange ((numTables * 16) - searchRange)

		//std::vector<record> tableRecords; // Table records array—one for each top-level table in the font
		std::unordered_map<std::string, record> tableRecords;
		void parse(Parser& p, size_t offset = 0) {
			p + sfntVersion + numTables + searchRange + entrySelector + rangeShift;

			tableRecords = p.umap<std::string, record>(numTables);
			for (int i = 0; i < numTables; i++) {
				uint32_t tag = p.u32(), checksum = p.u32(), offset = p.u32(), length = p.u32();
				tableRecords.try_emplace(u32_to_str(tag), record{ checksum, offset, length });
			}
		}
	};
}

#endif // UFONT_TBL_DIRECTORY