#pragma once

#include "Matrix3.h"

namespace ColorMath {

struct RGB { double r = 0.0, g = 0.0, b = 0.0; };
struct CMYK { double c = 0.0, m = 0.0, y = 0.0, k = 0.0; };
struct HLS { double h = 0.0, l = 0.0, s = 0.0; };
struct XYZ { double x = 0.0, y = 0.0, z = 0.0; };
struct Lab { double l = 0.0, a = 0.0, b = 0.0; };

enum class Illuminant { D65, D50, E };
enum class CmykAlgorithm { GCR, UCR };
enum class GamutStrategy { Clipping, Scaling };

double clampValue(double v, double lo, double hi);

RGB fitToGamut(RGB c, GamutStrategy strategy, bool& wasOutOfGamut);

double srgbToLinear(double c);
double linearToSrgb(double c);

Vector3 whitePoint(Illuminant illuminant);

Matrix3 rgbToXyzMatrix(Illuminant illuminant);
Matrix3 xyzToRgbMatrix(Illuminant illuminant);

HLS rgbToHls(const RGB& c);
RGB hlsToRgb(const HLS& c, GamutStrategy strategy, bool& wasOutOfGamut);

CMYK rgbToCmyk(const RGB& c, CmykAlgorithm algorithm);
RGB cmykToRgb(const CMYK& c, GamutStrategy strategy, bool& wasOutOfGamut);

RGB xyzToRgb(const XYZ& c, Illuminant illuminant, GamutStrategy strategy, bool& wasOutOfGamut);
XYZ rgbToXyz(const RGB& c, Illuminant illuminant);

Lab xyzToLab(const XYZ& c, Illuminant illuminant);
XYZ labToXyz(const Lab& c, Illuminant illuminant);

} // namespace ColorMath