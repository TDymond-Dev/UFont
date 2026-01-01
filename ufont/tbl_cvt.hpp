#ifndef UFONT_TBL_CVT
#define UFONT_TBL_CVT

#include "parser.hpp"

namespace uf::detail {
	struct tbl_cvt {
		std::vector<int16_t> instructionValues;

		void parse(Parser& p, size_t offset, size_t length) {
			p.set_position(offset);

			instructionValues = p.vec<int16_t>(length / sizeof(int16_t));
			for (auto& iv : instructionValues)
				iv = p.i16();
		}
	};

}

#endif /* UFONT_TBL_CVT */