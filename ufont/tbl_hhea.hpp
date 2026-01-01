#ifndef UFONT_TBL_HHEA
#define UFONT_TBL_HHEA

#include "parser.hpp"

namespace uf::detail {
	struct tbl_hhea {
		int32_t	version; //	0x00010000 (1.0) (16, 16)
		int16_t	ascent;//	Distance from baseline of highest ascender
		int16_t	descent;//	Distance from baseline of lowest descender
		int16_t	lineGap;//	typographic line gap
		uint16_t advanceWidthMax;//	must be consistent with horizontal metrics
		int16_t	minLeftSideBearing;//	must be consistent with horizontal metrics
		int16_t	minRightSideBearing;//	must be consistent with horizontal metrics
		int16_t	xMaxExtent;//	max(lsb + (xMax - xMin))
		int16_t	caretSlopeRise;//	used to calculate the slope of the caret(rise / run) set to 1 for vertical caret
		int16_t	caretSlopeRun;//	0 for vertical
		int16_t	caretOffset;//	set value to 0 for non - slanted fonts
		int16_t	reserved1;//	set value to 0
		int16_t	reserved2;//	set value to 0
		int16_t	reserved3;//	set value to 0
		int16_t	reserved4;//	set value to 0
		int16_t	metricDataFormat;//	0 for current format
		uint16_t numOfLongHorMetrics;//	number of advance widths in metrics table

		void parse(Parser& p, uint32_t offset = 0) {
			p.set_position(offset);
			version = p.i32();
			ascent = p.i16();
			descent = p.i16();
			lineGap = p.i16();
			advanceWidthMax = p.u16();
			minLeftSideBearing = p.i16();
			minRightSideBearing = p.i16();
			xMaxExtent = p.i16();
			caretSlopeRise = p.i16();
			caretSlopeRun = p.i16();
			caretOffset = p.i16();
			reserved1 = p.i16();
			reserved2 = p.i16();
			reserved3 = p.i16();
			reserved4 = p.i16();
			metricDataFormat = p.i16();
			numOfLongHorMetrics = p.u16();
		}
	};
}

#endif /* UFONT_TBL_HHEA */