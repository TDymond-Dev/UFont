#ifndef UFONT_TBL_HMTX
#define UFONT_TBL_HMTX

#include "parser.hpp"

namespace uf::detail {
	struct tbl_hmtx {
		struct hmtx_entry {
			uint16_t advanceWidth;
			int16_t leftSideBearing;
		};
		std::vector<hmtx_entry> entries;

		void parse(uint32_t noOfEntries, Parser& p, uint32_t offset = 0) {
			p.set_position(offset);
			entries = p.vec<hmtx_entry>(noOfEntries);
			for (auto& e : entries)
				e.advanceWidth = p.u16(), e.leftSideBearing = p.i16();
		}
	};
}


#endif /* UFONT_TBL_HMTX */