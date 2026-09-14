#include "ColorMath.h"

#include <cmath>
#include <algorithm>

namespace ColorMath {

double clampValue(double v, double lo, double hi)
{
    return std::max(lo, std::min(hi, v));
}

RGB fitToGamut(RGB c, GamutStrategy strategy, bool& wasOutOfGamut)
{
    const double eps = 1e-9;
    wasOutOfGamut = (c.r < -eps || c.r > 1.0 + eps ||
                     c.g < -eps || c.g > 1.0 + eps ||
                     c.b < -eps || c.b > 1.0 + eps);

    if (!wasOutOfGamut) {
        return c;
    }

    if (strategy == GamutStrategy::Clipping) {
        c.r = clampValue(c.r, 0.0, 1.0);
        c.g = clampValue(c.g, 0.0, 1.0);
        c.b = clampValue(c.b, 0.0, 1.0);
        return c;
    }

    double minV = std::min({c.r, c.g, c.b});
    if (minV < 0.0) {
        c.r -= minV;
        c.g -= minV;
        c.b -= minV;
    }
    double maxV = std::max({c.r, c.g, c.b});
    if (maxV > 1.0) {
        double f = 1.0 / maxV;
        c.r *= f;
        c.g *= f;
        c.b *= f;
    }
    c.r = clampValue(c.r, 0.0, 1.0);
    c.g = clampValue(c.g, 0.0, 1.0);
    c.b = clampValue(c.b, 0.0, 1.0);
    return c;
}

double srgbToLinear(double c)
{
    c = clampValue(c, 0.0, 1.0);
    return (c <= 0.04045) ? (c / 12.92) : std::pow((c + 0.055) / 1.055, 2.4);
}

double linearToSrgb(double c)
{
    c = clampValue(c, 0.0, 1.0);
    double v = (c <= 0.0031308) ? (12.92 * c) : (1.055 * std::pow(c, 1.0 / 2.4) - 0.055);
    return clampValue(v, 0.0, 1.0);
}

HLS rgbToHls(const RGB& c)
{
    double r = clampValue(c.r, 0.0, 1.0);
    double g = clampValue(c.g, 0.0, 1.0);
    double b = clampValue(c.b, 0.0, 1.0);

    double maxV = std::max({r, g, b});
    double minV = std::min({r, g, b});
    double delta = maxV - minV;

    HLS out;
    out.l = (maxV + minV) / 2.0;

    if (delta < 1e-9) {
        out.h = 0.0;
        out.s = 0.0;
        return out;
    }

    out.s = delta / (1.0 - std::fabs(2.0 * out.l - 1.0));

    double h;
    if (maxV == r) {
        h = 60.0 * std::fmod(((g - b) / delta), 6.0);
    } else if (maxV == g) {
        h = 60.0 * (((b - r) / delta) + 2.0);
    } else {
        h = 60.0 * (((r - g) / delta) + 4.0);
    }
    if (h < 0.0) h += 360.0;
    out.h = h;
    return out;
}

RGB hlsToRgb(const HLS& c, GamutStrategy strategy, bool& wasOutOfGamut)
{
    double h = c.h;
    while (h < 0.0) h += 360.0;
    h = std::fmod(h, 360.0);
    double l = c.l;
    double s = c.s;

    double cc = (1.0 - std::fabs(2.0 * l - 1.0)) * s;
    double x = cc * (1.0 - std::fabs(std::fmod(h / 60.0, 2.0) - 1.0));
    double m = l - cc / 2.0;

    double r1, g1, b1;
    if (h < 60.0)       { r1 = cc; g1 = x;  b1 = 0.0; }
    else if (h < 120.0) { r1 = x;  g1 = cc; b1 = 0.0; }
    else if (h < 180.0) { r1 = 0.0; g1 = cc; b1 = x;  }
    else if (h < 240.0) { r1 = 0.0; g1 = x;  b1 = cc; }
    else if (h < 300.0) { r1 = x;  g1 = 0.0; b1 = cc; }
    else                { r1 = cc; g1 = 0.0; b1 = x;  }

    RGB raw{ r1 + m, g1 + m, b1 + m };
    return fitToGamut(raw, strategy, wasOutOfGamut);
}

CMYK rgbToCmyk(const RGB& c, CmykAlgorithm algorithm)
{
    double r = clampValue(c.r, 0.0, 1.0);
    double g = clampValue(c.g, 0.0, 1.0);
    double b = clampValue(c.b, 0.0, 1.0);

    double c0 = 1.0 - r;
    double m0 = 1.0 - g;
    double y0 = 1.0 - b;

    double rawK = std::min({c0, m0, y0});

    if (rawK >= 1.0 - 1e-9) {
        return CMYK{0.0, 0.0, 0.0, 1.0};
    }

    double k;
    if (algorithm == CmykAlgorithm::GCR) {
        k = rawK;
    } else {
        k = rawK * rawK;
    }

    double denom = 1.0 - k;
    CMYK out;
    out.k = k;
    out.c = clampValue((c0 - k) / denom, 0.0, 1.0);
    out.m = clampValue((m0 - k) / denom, 0.0, 1.0);
    out.y = clampValue((y0 - k) / denom, 0.0, 1.0);
    return out;
}

RGB cmykToRgb(const CMYK& c, GamutStrategy strategy, bool& wasOutOfGamut)
{
    double cc = c.c, mm = c.m, yy = c.y, kk = c.k;
    RGB raw;
    raw.r = (1.0 - cc) * (1.0 - kk);
    raw.g = (1.0 - mm) * (1.0 - kk);
    raw.b = (1.0 - yy) * (1.0 - kk);
    return fitToGamut(raw, strategy, wasOutOfGamut);
}

Vector3 whitePoint(Illuminant illuminant)
{
    switch (illuminant) {
    case Illuminant::D65: return Vector3{0.95047, 1.00000, 1.08883};
    case Illuminant::D50: return Vector3{0.96422, 1.00000, 0.82521};
    case Illuminant::E:   return Vector3{1.00000, 1.00000, 1.00000};
    }
    return Vector3{0.95047, 1.00000, 1.08883};
}

static Matrix3 srgbToXyzD65()
{
    Matrix3 m;
    m.m[0][0] = 0.4124564; m.m[0][1] = 0.3575761; m.m[0][2] = 0.1804375;
    m.m[1][0] = 0.2126729; m.m[1][1] = 0.7151522; m.m[1][2] = 0.0721750;
    m.m[2][0] = 0.0193339; m.m[2][1] = 0.1191920; m.m[2][2] = 0.9503041;
    return m;
}

static Matrix3 bradfordAdaptation(const Vector3& srcWhite, const Vector3& dstWhite)
{
    Matrix3 MA;
    MA.m[0][0] =  0.8951; MA.m[0][1] =  0.2664; MA.m[0][2] = -0.1614;
    MA.m[1][0] = -0.7502; MA.m[1][1] =  1.7135; MA.m[1][2] =  0.0367;
    MA.m[2][0] =  0.0389; MA.m[2][1] = -0.0685; MA.m[2][2] =  1.0296;

    Matrix3 MAinv = MA.inverse();

    Vector3 rhoS = MA.multiply(srcWhite);
    Vector3 rhoD = MA.multiply(dstWhite);

    Matrix3 D = Matrix3::identity();
    D.m[0][0] = rhoD.x / rhoS.x;
    D.m[1][1] = rhoD.y / rhoS.y;
    D.m[2][2] = rhoD.z / rhoS.z;

    return MAinv.multiply(D).multiply(MA);
}

Matrix3 rgbToXyzMatrix(Illuminant illuminant)
{
    static const Matrix3 baseD65 = srgbToXyzD65();

    if (illuminant == Illuminant::D65) {
        return baseD65;
    }

    Vector3 srcWhite = whitePoint(Illuminant::D65);
    Vector3 dstWhite = whitePoint(illuminant);
    Matrix3 adapt = bradfordAdaptation(srcWhite, dstWhite);

    return adapt.multiply(baseD65);
}

Matrix3 xyzToRgbMatrix(Illuminant illuminant)
{
    return rgbToXyzMatrix(illuminant).inverse();
}

XYZ rgbToXyz(const RGB& c, Illuminant illuminant)
{
    Vector3 lin{ srgbToLinear(c.r), srgbToLinear(c.g), srgbToLinear(c.b) };
    Matrix3 M = rgbToXyzMatrix(illuminant);
    Vector3 xyz = M.multiply(lin);
    return XYZ{xyz.x, xyz.y, xyz.z};
}

RGB xyzToRgb(const XYZ& c, Illuminant illuminant, GamutStrategy strategy, bool& wasOutOfGamut)
{
    Matrix3 M = xyzToRgbMatrix(illuminant);
    Vector3 lin = M.multiply(Vector3{c.x, c.y, c.z});

    auto safeLinToSrgb = [](double v) {
        double sign = (v < 0.0) ? -1.0 : 1.0;
        double av = std::fabs(v);
        double s = (av <= 0.0031308) ? (12.92 * av) : (1.055 * std::pow(av, 1.0/2.4) - 0.055);
        return sign * s;
    };

    RGB raw{ safeLinToSrgb(lin.x), safeLinToSrgb(lin.y), safeLinToSrgb(lin.z) };
    return fitToGamut(raw, strategy, wasOutOfGamut);
}

static double labF(double t)
{
    const double delta = 6.0 / 29.0;
    if (t > delta * delta * delta) {
        return std::cbrt(t);
    }
    return t / (3.0 * delta * delta) + 4.0 / 29.0;
}

static double labFInv(double t)
{
    const double delta = 6.0 / 29.0;
    if (t > delta) {
        return t * t * t;
    }
    return 3.0 * delta * delta * (t - 4.0 / 29.0);
}

Lab xyzToLab(const XYZ& c, Illuminant illuminant)
{
    Vector3 w = whitePoint(illuminant);

    double fx = labF(c.x / w.x);
    double fy = labF(c.y / w.y);
    double fz = labF(c.z / w.z);

    Lab out;
    out.l = 116.0 * fy - 16.0;
    out.a = 500.0 * (fx - fy);
    out.b = 200.0 * (fy - fz);
    return out;
}

XYZ labToXyz(const Lab& c, Illuminant illuminant)
{
    Vector3 w = whitePoint(illuminant);

    double fy = (c.l + 16.0) / 116.0;
    double fx = fy + c.a / 500.0;
    double fz = fy - c.b / 200.0;

    XYZ out;
    out.x = w.x * labFInv(fx);
    out.y = w.y * labFInv(fy);
    out.z = w.z * labFInv(fz);
    return out;
}

} // namespace ColorMath