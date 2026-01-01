#ifndef UFONT_TBL_LOCA
#define UFONT_TBL_LOCA

#include "parser.hpp"

namespace uf::detail {
	struct tbl_loca {
		std::vector<uint32_t> offsets;

		void parse(uint32_t locaFormat, uint32_t size, Parser& p, uint32_t offset = 0) {
			p.set_position(offset);

			offsets = p.vec<uint32_t>(size + 1);

			for (auto& o : offsets) {
				if (locaFormat == 1) {
					o = p.u32();
				}
				else {
					o = p.u16() * 2;
				}
			}
		}
	};
}

#endif /* UFONT_TBL_LOCA */