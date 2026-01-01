#ifndef UFONT_TBL_POST
#define UFONT_TBL_POST

#include "parser.hpp"

namespace uf::detail {
	struct tbl_post {
		uint32_t format; // Format of this table
		uint32_t italicAngle; // italic angle in counter-clockwise degrees from the vertical
		int16_t underlinePosition; // underline position 
		int16_t underlineThickness; // underline thickness
		uint32_t isFixedPitch; // Font is monospaced; set to 1 if the font is monospaced and 0 otherwise (N.B., to maintain compatibility with older versions of the TrueType spec, accept any non-zero value as meaning that the font is monospaced)
		uint32_t minMemType42; // Minimum memory usage when a TrueType font is downloaded as a Type 42 font
		uint32_t maxMemType42; // Maximum memory usage when a TrueType font is downloaded as a Type 42 font
		uint32_t minMemType1; // Minimum memory usage when a TrueType font is downloaded as a Type 1 font
		uint32_t maxMemType1; // Maximum memory usage when a TrueType font is downloaded as a Type 1 font


		void parse(Parser& p, size_t offset) {
			p.set_position(offset);

			format = p.u32();
			italicAngle = p.u32();
			underlinePosition = p.i16();
			underlineThickness = p.i16();
			isFixedPitch = p.u32();
			minMemType42 = p.u32();
			maxMemType42 = p.u32();
			minMemType1 = p.u32();
			maxMemType1 = p.u32();
		}
	};
}
#endif UFONT_TBL_POST