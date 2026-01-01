#ifndef UFONT_TBL_OS2
#define UFONT_TBL_OS2

#include "parser.hpp"

namespace uf::detail {
	struct tbl_os2 {
		uint16_t version;
		int16_t	xAvgCharWidth;
		uint16_t usWeightClass;
		uint16_t usWidthClass;
		uint16_t fsType;
		int16_t	ySubscriptXSize;
		int16_t	ySubscriptYSize;
		int16_t	ySubscriptXOffset;
		int16_t	ySubscriptYOffset;
		int16_t	ySuperscriptXSize;
		int16_t	ySuperscriptYSize;
		int16_t	ySuperscriptXOffset;
		int16_t	ySuperscriptYOffset;
		int16_t	yStrikeoutSize;
		int16_t	yStrikeoutPosition;
		int16_t	sFamilyClass;
		uint8_t	panose[10];
		uint32_t ulUnicodeRange1;// Bits 0–31
		uint32_t ulUnicodeRange2;	// Bits 32–63
		uint32_t ulUnicodeRange3; //	Bits 64–95
		uint32_t ulUnicodeRange4; //	Bits 96–127
		uint8_t achVendID[4]; // Tag(4 8 bit ints)
		uint16_t fsSelection;
		uint16_t usFirstCharIndex;
		uint16_t usLastCharIndex;
		int16_t	sTypoAscender;
		int16_t	sTypoDescender;
		int16_t	sTypoLineGap;
		uint16_t usWinAscent;
		uint16_t usWinDescent;
		uint32_t ulCodePageRange1;//	Bits 0–31
		uint32_t ulCodePageRange2;//	Bits 32–63
		int16_t	sxHeight;
		int16_t	sCapHeight;
		uint16_t usDefaultChar;
		uint16_t usBreakChar;
		uint16_t usMaxContext;
		uint16_t usLowerOpticalPointSize;
		uint16_t usUpperOpticalPointSize;

		void parse(Parser& p, size_t offset) {
			p.set_position(offset);

			version = p.u16();
			xAvgCharWidth = p.i16();
			usWeightClass = p.u16();
			usWidthClass = p.u16();
			fsType = p.i16();
			ySubscriptXSize = p.i16();
			ySubscriptYSize = p.i16();
			ySubscriptXOffset = p.i16();
			ySubscriptYOffset = p.i16();
			ySuperscriptXSize = p.i16();
			ySuperscriptYSize = p.i16();
			ySuperscriptXOffset = p.i16();
			ySuperscriptYOffset = p.i16();
			yStrikeoutSize = p.i16();
			yStrikeoutPosition = p.i16();
			sFamilyClass = p.i16();

			for (int i = 0; i < 10; i++)
				panose[i] = p.u8();

			ulUnicodeRange1 = p.u32();
			ulUnicodeRange2 = p.u32();
			ulUnicodeRange3 = p.u32();
			ulUnicodeRange4 = p.u32();

			for (int i = 0; i < 4; i++)
				achVendID[i] = p.u8();

			fsSelection = p.u16();
			usFirstCharIndex = p.u16();
			usLastCharIndex = p.u16();
			sTypoAscender = p.i16();
			sTypoDescender = p.i16();
			sTypoLineGap = p.i16();
			usWinAscent = p.u16();
			usWinDescent = p.u16();
			ulCodePageRange1 = p.u32();
			ulCodePageRange2 = p.u32();
			sxHeight = p.i16();
			sCapHeight = p.i16();
			usDefaultChar = p.u16();
			usBreakChar = p.u16();
			usMaxContext = p.u16();
			usLowerOpticalPointSize = p.u16();
			usUpperOpticalPointSize = p.u16();
		}
	};
}

#endif /* UFONT_TBL_OS2 */