#ifndef UFONT_TBL_NAME
#define UFONT_TBL_NAME

#include "parser.hpp"

namespace uf::detail {
	struct tbl_name {
		struct name_record {
			uint16_t platformID;
			uint16_t platformSpecificID;
			uint16_t languageID;
			uint16_t nameID;
			uint16_t length;
			uint16_t offset;
		};

		uint16_t format;
		uint16_t count;
		uint16_t stringOffset;

		std::vector<name_record> nameRecords;
		std::vector<std::string> strings;

		void parse(Parser& p, size_t offset) {
			p.set_position(offset);

			format = p.u16();
			count = p.u16();
			stringOffset = p.u16();
			nameRecords = p.vec<name_record>(count);

			for (auto& r : nameRecords) {
				p + r.platformID + r.platformSpecificID + r.languageID + r.nameID + r.length + r.offset;
			}

			for (int i = 0; i < count; i++) {
				strings.push_back(std::string(nameRecords[i].length, ' '));
				for (int j = 0; j < nameRecords[i].length - 1; j++)
					strings.back()[j] = p.u8();
			}
		}
	};
}


#endif /* UFONT_TBL_NAME */