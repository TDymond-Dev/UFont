#ifndef UFONT_BITMAP
#define UFONT_BITMAP

#include "metric.hpp"
#include <functional>
#include <algorithm>

#define NOMINMAX

namespace uf::detail {
	
	/*
		Draw all the points of the glyph outline to a bitmap
		Draw the outline of the glyph to a bitmap
		Draw the interior of the glyph to a bitmap
		Draw the exterior of the glyph to a bitmap

		Scale The bitmap;
		Peform AA on the bitmap
		Scanline Bitmap
	*/



	auto cccw = [](detail::Coord A, detail::Coord B, detail::Coord C) {
		return (C.y - A.y) * (B.x - A.x) >= (B.y - A.y) * (C.x - A.x);
		};

	auto intersects_a = [&](detail::Coord A, detail::Coord B, detail::Coord C, detail::Coord D) {
		return cccw(A, C, D) != cccw(B, C, D) && cccw(A, B, C) != cccw(A, B, D);
		};

	auto inside = [&](std::vector<std::vector<detail::Coord>> const& outline, detail::Coord point) {
		int modCount = 0;
		for (auto const& contour : outline) {
			if (contour.empty())
				continue;

			for (int i = 0; i < contour.size() - 1; i++)
				if (intersects_a({ -100, point.y }, { point.x, point.y }, contour[i], contour[i + 1]))
					modCount += 1;

			if (intersects_a({ -100, point.y }, { point.x, point.y }, contour[contour.size() - 1], contour[0]))
				modCount += 1;
		}

		return modCount % 2 != 0;
		};

	auto fill(FontCharacter const& c, std::vector<uint8_t>& data, int w, int h) {
		std::vector<uint8_t> temp(w * h, 0);
		// loop through each pixel and check if inside
		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				float x = i, y = j;

				data[(h - j - 1) * w + i] = inside(c.outline, detail::Coord{ x + c.lsb, y + c.yMin }) ? 255 : 0;
			}
		}

		// shift all pixels to the left by two
		//for (int i = 2; i < w; i++) {
		//	for (int j = 2; j < h; j++) {
		//		data[j * w + i - 2] = temp[j * w + i];
		//	}
		//}
	};

	auto fill_inverse(FontCharacter const& c, std::vector<uint8_t>& data, int w, int h) {
		auto cccw = [](detail::Coord A, detail::Coord B, detail::Coord C) {
			return (C.y - A.y) * (B.x - A.x) >= (B.y - A.y) * (C.x - A.x);
			};

		auto intersects = [&](detail::Coord A, detail::Coord B, detail::Coord C, detail::Coord D) {
			return cccw(A, C, D) != cccw(B, C, D) && cccw(A, B, C) != cccw(A, B, D);
			};

		auto inside = [&](std::vector<std::vector<detail::Coord>> const& outline, detail::Coord point) {
			int modCount = 0;
			for (auto const& contour : outline) {
				if (contour.empty())
					continue;

				for (int i = 0; i < contour.size() - 1; i++)
					if (intersects({ -100, point.y }, { point.x, point.y }, contour[i], contour[i + 1]))
						modCount += 1;

				if (intersects({ -100, point.y }, { point.x, point.y }, contour[contour.size() - 1], contour[0]))
					modCount += 1;
			}

			return modCount % 2 == 0;
			};

		// loop through each pixel and check if inside
		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				float x = i, y = j;

				if (x >= 0 && x < w && y >= 0 && y < h)
					data[j * w + i] = inside(c.outline, detail::Coord{ x, y }) ? 255 : 0;
			}
		}
	};

	auto outline(FontCharacter const& c, std::vector<uint8_t>& data, int w, int h) {
		auto distance = [](uf::detail::Coord a, uf::detail::Coord b) {
			return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
			};

		for (int i = 0; i < c.outline.size(); i++) {
			auto& contour = c.outline[i];
			if (contour.empty())
				continue;

			for (int j = 0; j < contour.size() - 1; j++) {
				auto& a = contour[j];
				auto& b = contour[j + 1];
				auto d = distance(a, b);
				for (int k = 0; k < d; k++) {
					auto x = a.x + (b.x - a.x) * k / d;
					auto y = a.y + (b.y - a.y) * k / d;

					// Add padding to x and y coordinates
					//x += padding;
					//y += padding;

					// Check if coord falls within image range
					if (x > 0 && x < w && y > 0 && y < h) {
						data[(int)x + (int)y * w] = 255;
					}
				}
			}

			// first and last point
			auto& a = contour.back();
			auto& b = contour.front();
			auto d = distance(a, b);
			for (int k = 0; k < d; k++) {
				auto x = a.x + (b.x - a.x) * k / d;
				auto y = a.y + (b.y - a.y) * k / d;

				// Add padding to x and y coordinates
			

				// Check if coord falls within image range
				if (x >= 0 && x < w && y >= 0 && y < h) {
					data[(int)x + (int)y * w] = 255;
				}
			}
		}
	}

	auto points(FontCharacter const& c, std::vector<uint8_t>& data, int w, int h) {
		auto distance = [](uf::detail::Coord a, uf::detail::Coord b) {
			return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
			};

		for (int x = 0; x < w; x++) {
			for (int y = 0; y < h; y++) {
				for (auto& outline :  c.outline) {
					for (auto [cx, cy, offcurve] : outline) {
						if (x >= 0 && x < w && y >= 0 && y < h) {
							float d = offcurve ? 3 : 3;
							if (distance(detail::Coord{ (float)x, (float)y }, detail::Coord{ (float)cx, (float)cy }) <= d) {
								data[(int)x + (int)y * w] = 255;
								break;
							}
						}
					}
				}
			}
		}
	}

	bool onSegment(detail::Coord p, detail::Coord q, detail::Coord r) {
		if (q.x <= detail::smax(p.x, r.x) && q.x >= detail::smin(p.x, r.x) &&
			q.y <= detail::smax(p.y, r.y) && q.y >= detail::smin(p.y, r.y))
			return true;
		return false;
	}

	int orientation(detail::Coord p, detail::Coord q, detail::Coord r) {
		double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
		if (val == 0) return 0;  // colinear
		return (val > 0) ? 1 : 2; // clock or counterclock wise
	}

	bool intersects(detail::Coord p1, detail::Coord q1, detail::Coord p2, detail::Coord q2) {
		int o1 = orientation(p1, q1, p2);
		int o2 = orientation(p1, q1, q2);
		int o3 = orientation(p2, q2, p1);
		int o4 = orientation(p2, q2, q1);

		if (o1 != o2 && o3 != o4)
			return true;

		if (o1 == 0 && onSegment(p1, p2, q1)) return true;
		if (o2 == 0 && onSegment(p1, q2, q1)) return true;
		if (o3 == 0 && onSegment(p2, p1, q2)) return true;
		if (o4 == 0 && onSegment(p2, q1, q2)) return true;

		return false;
	}

	bool lineSegmentsIntersect(Coord p1, Coord p2, Coord p3, Coord p4, Coord& ipoint) {
		int o1 = orientation(p1, p2, p3);
		int o2 = orientation(p1, p2, p4);
		int o3 = orientation(p3, p4, p1);
		int o4 = orientation(p3, p4, p2);

		// General case
		if (o1 != o2 && o3 != o4) {
			// Calculate intersection point
			double a1 = p2.y - p1.y;
			double b1 = p1.x - p2.x;
			double c1 = a1 * p1.x + b1 * p1.y;

			double a2 = p4.y - p3.y;
			double b2 = p3.x - p4.x;
			double c2 = a2 * p3.x + b2 * p3.y;

			double determinant = a1 * b2 - a2 * b1;

			if (determinant != 0) {
				ipoint.x = (b2 * c1 - b1 * c2) / determinant;
				ipoint.y = (a1 * c2 - a2 * c1) / determinant;
				return true;
			}
			return false;
		}

		// Special cases
		if (o1 == 0 && onSegment(p1, p3, p2)) { ipoint = p3; return true; }
		if (o2 == 0 && onSegment(p1, p4, p2)) { ipoint = p4; return true; }
		if (o3 == 0 && onSegment(p3, p1, p4)) { ipoint = p1; return true; }
		if (o4 == 0 && onSegment(p3, p2, p4)) { ipoint = p2; return true; }

		return false; // Doesn't fall in any of the above cases
	}

	auto scanline(FontCharacter const& c, std::vector<uint8_t>& data, int w = -1, int h = -1) {
	

		for (int j = 0; j < c.height(); j++) {
			Coord p1 = { -100, j }, p2 = { c.width() + 100, j};
			std::vector<Coord> intersections;
			Coord lastIPoint = { -20, -20 };
			for (auto contour : c.outline) {
				for (int i = 0; i < contour.size() - 1; i++) {
					Coord p3 = contour[i], p4 = contour[i + 1];
					Coord ipoint;
					if (lineSegmentsIntersect(p1, p2, p3, p4, ipoint)) {
						if (lastIPoint.x == ipoint.x && lastIPoint.y == ipoint.y)
							intersections.push_back(ipoint);
						intersections.push_back(ipoint), lastIPoint = ipoint;
					}
				}
			}

			std::sort(intersections.begin(), intersections.end(), [](Coord a, Coord b) { return a.x < b.x; });
			if (intersections.size() % 2 != 0 || intersections.empty())
				continue;

			for (int i = 0; i < intersections.size(); i += 2) {
				for (int x = intersections[i].x; x < intersections[i + 1].x; x++) {
					auto xx = std::clamp(x, 0, c.width() - 1);
					data[j * c.width() + xx] = 255;
				}

			}
		}
	}


	static float gDistance = 5;
	float dist(Coord a, Coord b) {
		return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
	}

	float dotProduct(Coord a, Coord b) {
		return a.x * b.x + a.y * b.y;
	}

	float lengthSquared(Coord a) {
		return a.x * a.x + a.y * a.y;
	}

	float shortestDistToSegment(Coord p, Coord v, Coord w) {
		Coord vw;
		vw.x = w.x - v.x;
		vw.y = w.y - v.y;

		float l2 = lengthSquared(vw);
		if (l2 == 0.0) return dist(p, v);
		float t = dotProduct({ p.x - v.x, p.y - v.y }, vw) / l2;
		if (t < 0.0) return dist(p, v);
		if (t > 1.0) return dist(p, w);
		return dist(p, { v.x + t * vw.x, v.y + t * vw.y });
	}

	bool isCloseToOutline(Coord pixel, std::vector<std::vector<Coord>> outline, float distance) {
		for (const auto& polygon : outline) {
			for (size_t i = 0; i < polygon.size(); ++i) {
				if (shortestDistToSegment(pixel, polygon[i], polygon[(i + 1) % polygon.size()]) < distance) {
					return true;
				}
			}
		}
		return false;
	}

	auto outline_distance(FontCharacter const& c, std::vector<uint8_t>& data, int w, int h) {
		for (int x = 0; x < w; x++) {
			for (int y = 0; y < h; y++) {
				if (isCloseToOutline({ (float)x, (float)y }, c.outline, gDistance)) {
					data[y * w + x] = 255;
				}
			}
		}
	}

	auto distance_field(FontCharacter const& c, std::vector<uint8_t>& data, int w, int h) {
		for (int x = 0; x < w; x++) {
			for (int y = 0; y < h; y++) {
				float minDist = FLT_MAX;
				bool inside_b = inside(c.outline, { (float)x, (float)y });
				if (!inside_b) {
					continue;
				}
				
				for (auto const& contour : c.outline) {
					for (size_t i = 0; i < contour.size(); ++i) {
						float dist = shortestDistToSegment({ (float)x, (float)y }, contour[i], contour[(i + 1) % contour.size()]);

						if (dist < minDist) {
							minDist = dist;
						}
					}
				}

				if (inside_b) {
					data[y * w + x] = (255 * ((float)(uint8_t)minDist / (float)h)) * -1.0;
				}
				else {
					data[y * w + x] = 0; // set the pixel to black if it's outside the shape
				}
			}
		}
	}



	void perform_aa_advanced(std::vector<uint8_t>&data, int width, int height, int c) {
		std::vector<int> integral_image(width * height);
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				integral_image[j * width + i] = data[j * width + i]
					+ (i > 0 ? integral_image[j * width + i - 1] : 0)
					+ (j > 0 ? integral_image[(j - 1) * width + i] : 0)
					- (i > 0 && j > 0 ? integral_image[(j - 1) * width + i - 1] : 0);
			}
		}

		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				if (data[j * width + i] == 255)
					continue;

				int x1 = detail::smax(i - c, 0);
				int x2 = detail::smin(i + c, width - 1);
				int y1 = detail::smax(j - c, 0);
				int y2 = detail::smin(j + c, height - 1);

				int count = (x2 - x1 + 1) * (y2 - y1 + 1);
				int total = integral_image[y2 * width + x2]
					- (x1 > 0 ? integral_image[y2 * width + x1 - 1] : 0)
					- (y1 > 0 ? integral_image[(y1 - 1) * width + x2] : 0)
					+ (x1 > 0 && y1 > 0 ? integral_image[(y1 - 1) * width + x1 - 1] : 0);

				data[j * width + i] = total / count;
			}
		}
	}
	
	auto bitmap(FontCharacter const& c, int height, std::function<void(FontCharacter, std::vector<uint8_t>&, int, int)>&& f) {
		float scale = (float)height / (float)2048;

		FontCharacter scaledChar = c.scale(scale);

		auto [sx, sy, sw, sh] = scaledChar.outline_bounds();
		sx = 0, sy = 0;

		std::vector<uint8_t> scaledData((sw + 2) * (sh + 2), 0);

		f(scaledChar, scaledData, sw + 2, sh + 2);

		

		return std::make_tuple(scaledData, sw + 2, sh + 2);
	}

	auto anti_alias(std::vector<uint8_t>& data, int w, int h, int kernel = 2) {

		perform_aa_advanced(data, w, h, kernel);
	}
}

#endif // UFONT_BITMAP