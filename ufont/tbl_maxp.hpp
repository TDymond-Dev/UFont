#ifndef UFONT_TBL_MAXP
#define UFONT_TBL_MAXP

#include "parser.hpp"

namespace uf::detail {
	struct tbl_maxp {
		uint32_t	version;	//0x00010000 (1.0) pack (16, 16)
		uint16_t	numGlyphs;	//the number of glyphs in the font
		uint16_t	maxPoints;	// points in non - compound glyph
		uint16_t	maxContours;	// contours in non - compound glyph
		uint16_t	maxComponentPoints;	// points in compound glyph
		uint16_t	maxComponentContours;	// contours in compound glyph
		uint16_t	maxZones; //	set to 2
		uint16_t	maxTwilightPoints;	// points used in Twilight Zone(Z0)
		uint16_t	maxStorage;			// number of Storage Area locations
		uint16_t	maxFunctionDefs;	// number of FDEFs
		uint16_t	maxInstructionDefs;	// number of IDEFs
		uint16_t	maxStackElements;	// maximum stack depth
		uint16_t	maxSizeOfInstructions; //  byte count for glyph instructions
		uint16_t	maxComponentElements; // number of glyphs referenced at top level
		uint16_t	maxComponentDepth; // levels of recursion, set to 0 if font has only simple glyphs

		void parse(Parser& p, uint32_t offset = 0) {
			p.set_position(offset);
			version = p.u32();
			numGlyphs = p.u16();
			maxPoints = p.u16();
			maxContours = p.u16();
			maxComponentPoints = p.u16();
			maxComponentContours = p.u16();
			maxZones = p.u16();
			maxTwilightPoints = p.u16();
			maxStorage = p.u16();
			maxFunctionDefs = p.u16();
			maxInstructionDefs = p.u16();
			maxStackElements = p.u16();
			maxSizeOfInstructions = p.u16();
			maxComponentElements = p.u16();
			maxComponentDepth = p.u16();
		}
	};
}


#endif UFONT_TBL_MAXP