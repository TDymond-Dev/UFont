#ifndef UFONT_TBL_GLYF
#define UFONT_TBL_GLYF

#include "parser.hpp"

namespace uf::detail {
	struct tbl_glyf {
		struct Glyph {
			int16_t noOfContours;
			int16_t xMin;
			int16_t yMin;
			int16_t xMax;
			int16_t yMax;

			uint16_t index;

		};

		struct SimpleGlyph {
			struct Flag {
				bool offCurve;
				bool xShort;
				bool yShort;
				bool repeat;
				bool xDual;
				bool yDual;
				bool overlap_deprec;
				bool resevred;
			};


			struct Outline {
				std::vector<uint16_t> endPtsOfContours;
				uint16_t instructionLength;
				std::vector<uint8_t> instructions;
				std::vector<Flag> flags;
				std::vector<int16_t> xCoords;
				std::vector<int16_t> yCoords;
			};
			Outline outline;
		};

		struct CompoundGlyph {
			struct Flag {
				bool ARG_1_AND_2_ARE_WORDS; // If set, the arguments are words, If not set, they are bytes.
				bool ARGS_ARE_XY_VALUES; // If set, the arguments are xy values, If set, the arguments are xy values.
				bool ROUND_XY_TO_GRID; // If set, the arguments are xy values, if not set do not round xy values to grid (relevant only to bit 1 is set)
				bool WE_HAVE_A_SCALE; // If set, there is a simple scale for the component. If not set, scale is 1.0.
				bool OBSOLETE; //  If not set, scale is 1.0.
				bool MORE_COMPONENTS; // If set, at least one additional glyph follows this one.
				bool WE_HAVE_AN_X_AND_Y_SCALE; // If set the x direction will use a different scale than the y direction.
				bool WE_HAVE_A_TWO_BY_TWO; // If set there is a 2-by-2 transformation that will be used to scale the component.
				bool WE_HAVE_INSTRUCTIONS; // If set, instructions for the component character follow the last component.
				bool USE_MY_METRICS; // Use metrics from this component for the compound glyph.
				bool OVERLAP_COMPOUND; // If set, the components of this compound glyph overlap.
			};

			struct CompoundGlyphComponent {
				Flag flag;
				uint16_t index;
				int16_t argument1;
				int16_t argument2;
				float transformationType;
			};

			int index;
			std::vector<CompoundGlyphComponent> components;
		};

		std::vector<Glyph> glyphs;
		std::vector<SimpleGlyph> simpleGlyphs;
		std::vector<CompoundGlyph> compoundGlyphs;

		void parse(uint32_t size, std::vector<uint32_t> const& offsets, Parser& p, uint32_t offset = 0) {
			p.set_position(offset);

			glyphs = p.vec<Glyph>(size);
			int k = 0, i = 0;
			int totalGlyphCount = 0;
			int couter = 0;
			for (auto& glyph : glyphs) {
				//std::cout << couter << std::endl;
				//std::cout << "Glyph Index: " << i << std::endl;
				p.set_position(offset + offsets[i++]);

				glyph.noOfContours = p.i16();

				// Bounding Box
				glyph.xMin = p.i16();
				glyph.yMin = p.i16();
				glyph.xMax = p.i16();
				glyph.yMax = p.i16();

				if (glyph.noOfContours >= 0) { // Simple Glpyh
					SimpleGlyph sGlyph;

					SimpleGlyph::Outline outline;

					// contours
					outline.endPtsOfContours = p.vec<uint16_t>(glyph.noOfContours);
					for (auto& c : outline.endPtsOfContours)
						c = p.u16();

					// instructions
					outline.instructionLength = p.u16();
					outline.instructions = p.vec<uint8_t>(outline.instructionLength);
					for (auto& ist : outline.instructions)
						ist = p.u8();

					// flags
					//outline.flags = p.vec<Glyph::Flag>(outline.endPtsOfContours.back() + 1);
					for (int j = 0; j < outline.endPtsOfContours.back() + 1; j++) {
						auto byte = p.u8();
						SimpleGlyph::Flag flag;
						flag.offCurve = byte & 0x01;
						flag.xShort = byte & 0x02;
						flag.yShort = byte & 0x04;
						flag.repeat = byte & 0x08;
						flag.xDual = byte & 0x10;
						flag.yDual = byte & 0x20;
						outline.flags.push_back(flag);

						if (flag.repeat) {
							auto repeatCount = p.u8();
							while (repeatCount--) {
								outline.flags.push_back(flag);
								j++;
							}
						}
					}

					// coordinates
					for (int i = 0; i < outline.flags.size(); i++) {
						auto flag = outline.flags[i];

						if (flag.xShort && flag.xDual) // 1 byte +
							outline.xCoords.push_back(abs((int16_t)p.u8()));
						else if (flag.xShort && !flag.xDual)// 1 byte -
							outline.xCoords.push_back(-abs((int16_t)p.u8()));
						else if (!flag.xShort && flag.xDual) // same as previous
							outline.xCoords.push_back(0);
						else if (!flag.xShort && !flag.xDual) // same as previous
							outline.xCoords.push_back(p.i16());
					}

					for (int i = 0; i < outline.flags.size(); i++) {
						auto flag = outline.flags[i];

						if (flag.yShort && flag.yDual) // 1 byte +
							outline.yCoords.push_back(abs((int16_t)p.u8()));
						else if (flag.yShort && !flag.yDual)// 1 byte -
							outline.yCoords.push_back(-abs((int16_t)p.u8()));
						else if (!flag.yShort && flag.yDual) // same as previous
							outline.yCoords.push_back(0);
						else if (!flag.yShort && !flag.yDual) // same as previous
							outline.yCoords.push_back(p.i16());
					}

					sGlyph.outline = outline;
					glyph.index = simpleGlyphs.size();
					simpleGlyphs.push_back(sGlyph);
				}
				else { // Compound Glyph
					CompoundGlyph compoundGlyph;

					bool endOfGlyphs = false;

					while (true) {
						CompoundGlyph::CompoundGlyphComponent component;

						uint16_t flags = p.u16();

						component.flag.ARG_1_AND_2_ARE_WORDS = flags & 0x01;
						component.flag.ARGS_ARE_XY_VALUES = flags & 0x02;
						component.flag.ROUND_XY_TO_GRID = flags & 0x04;
						component.flag.WE_HAVE_A_SCALE = flags & 0x08;
						component.flag.OBSOLETE = flags & 0x10;
						component.flag.MORE_COMPONENTS = flags & 0x20;
						component.flag.WE_HAVE_AN_X_AND_Y_SCALE = flags & 0x40;
						component.flag.WE_HAVE_A_TWO_BY_TWO = flags & 0x80;
						component.flag.WE_HAVE_INSTRUCTIONS = flags & 0x100;
						component.flag.USE_MY_METRICS = flags & 0x200;
						component.flag.OVERLAP_COMPOUND = flags & 0x400;

				
						component.index = p.u16();

						if (component.flag.ARG_1_AND_2_ARE_WORDS) {
							component.argument1 = p.i16();
							component.argument2 = p.i16();
						}
						else {
							component.argument1 = (int16_t)p.i8();
							component.argument2 = (int16_t)p.i8();
						}

						compoundGlyph.components.push_back(component);
						if (!component.flag.MORE_COMPONENTS)
							break;
					}

					compoundGlyph.index = couter;
					compoundGlyphs.push_back(compoundGlyph);

					//Each component glyph parts consists of a flag entry, two offset entries and from one to four transformation entries
				}

				couter++;
			}
		}
	};

}

#endif // !UFONT_TBL_GLYF
