#ifndef UFONT_TBL_BSLN
#define UFONT_TBL_BSLN

#include "parser.hpp"

namespace uf::detail {
	struct tbl_bsln {
		uint32_t version;
		uint16_t format;
		uint16_t defaultBaseline;

		void parse(Parser& p, size_t offset) {
			p.set_position(offset);

			version = p.u32();
			format = p.u16();
			defaultBaseline = p.u16();
			
		}
	};
}
#endif UFONT_TBL_BSLN