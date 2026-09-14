#include <QtTest/QtTest>

#include "../src/model/ColorMath.h"

using namespace ColorMath;

namespace {
const double EPS = 1e-4;
}

class TestColorMath : public QObject
{
    Q_OBJECT

private slots:

    void redToHls()
    {
        RGB red{1.0, 0.0, 0.0};
        HLS hls = rgbToHls(red);
        QVERIFY(qAbs(hls.h - 0.0) < EPS);
        QVERIFY(qAbs(hls.l - 0.5) < EPS);
        QVERIFY(qAbs(hls.s - 1.0) < EPS);
    }

    void whiteToHls()
    {
        RGB white{1.0, 1.0, 1.0};
        HLS hls = rgbToHls(white);
        QVERIFY(qAbs(hls.l - 1.0) < EPS);
        QVERIFY(qAbs(hls.s - 0.0) < EPS);
    }

    void hlsRoundTrip()
    {
        RGB original{0.2, 0.7, 0.4};
        HLS hls = rgbToHls(original);
        bool outOfGamut = false;
        RGB back = hlsToRgb(hls, GamutStrategy::Clipping, outOfGamut);
        QVERIFY(!outOfGamut);
        QVERIFY(qAbs(back.r - original.r) < EPS);
        QVERIFY(qAbs(back.g - original.g) < EPS);
        QVERIFY(qAbs(back.b - original.b) < EPS);
    }

    void redToCmykGcr()
    {
        RGB red{1.0, 0.0, 0.0};
        CMYK cmyk = rgbToCmyk(red, CmykAlgorithm::GCR);
        QVERIFY(qAbs(cmyk.c - 0.0) < EPS);
        QVERIFY(qAbs(cmyk.m - 1.0) < EPS);
        QVERIFY(qAbs(cmyk.y - 1.0) < EPS);
        QVERIFY(qAbs(cmyk.k - 0.0) < EPS);
    }

    void blackToCmyk()
    {
        RGB black{0.0, 0.0, 0.0};
        CMYK cmykGcr = rgbToCmyk(black, CmykAlgorithm::GCR);
        CMYK cmykUcr = rgbToCmyk(black, CmykAlgorithm::UCR);
        QVERIFY(qAbs(cmykGcr.k - 1.0) < EPS);
        QVERIFY(qAbs(cmykUcr.k - 1.0) < EPS);
        QVERIFY(qAbs(cmykGcr.c) < EPS && qAbs(cmykGcr.m) < EPS && qAbs(cmykGcr.y) < EPS);
    }

    void whiteToCmyk()
    {
        RGB white{1.0, 1.0, 1.0};
        CMYK cmyk = rgbToCmyk(white, CmykAlgorithm::GCR);
        QVERIFY(qAbs(cmyk.c) < EPS && qAbs(cmyk.m) < EPS && qAbs(cmyk.y) < EPS && qAbs(cmyk.k) < EPS);
    }

    void gcrProducesMoreOrEqualBlackThanUcr()
    {
        RGB c{0.5, 0.35, 0.2};
        CMYK gcr = rgbToCmyk(c, CmykAlgorithm::GCR);
        CMYK ucr = rgbToCmyk(c, CmykAlgorithm::UCR);
        QVERIFY(gcr.k >= ucr.k - EPS);
    }

    void cmykRoundTrip()
    {
        RGB original{0.6, 0.3, 0.8};
        CMYK cmyk = rgbToCmyk(original, CmykAlgorithm::GCR);
        bool outOfGamut = false;
        RGB back = cmykToRgb(cmyk, GamutStrategy::Clipping, outOfGamut);
        QVERIFY(!outOfGamut);
        QVERIFY(qAbs(back.r - original.r) < EPS);
        QVERIFY(qAbs(back.g - original.g) < EPS);
        QVERIFY(qAbs(back.b - original.b) < EPS);
    }

    void whiteXyzUnderD65()
    {
        RGB white{1.0, 1.0, 1.0};
        XYZ xyz = rgbToXyz(white, Illuminant::D65);
        QVERIFY(qAbs(xyz.x - 0.95047) < 1e-3);
        QVERIFY(qAbs(xyz.y - 1.00000) < 1e-3);
        QVERIFY(qAbs(xyz.z - 1.08883) < 1e-3);

        Lab lab = xyzToLab(xyz, Illuminant::D65);
        QVERIFY(qAbs(lab.l - 100.0) < 1e-2);
        QVERIFY(qAbs(lab.a) < 1e-2);
        QVERIFY(qAbs(lab.b) < 1e-2);
    }

    void whiteXyzUnderE()
    {
        RGB white{1.0, 1.0, 1.0};
        XYZ xyz = rgbToXyz(white, Illuminant::E);
        QVERIFY(qAbs(xyz.x - 1.0) < 1e-3);
        QVERIFY(qAbs(xyz.y - 1.0) < 1e-3);
        QVERIFY(qAbs(xyz.z - 1.0) < 1e-3);
    }

    void matricesDifferByIlluminant()
    {
        Matrix3 mD65 = rgbToXyzMatrix(Illuminant::D65);
        Matrix3 mD50 = rgbToXyzMatrix(Illuminant::D50);
        bool anyDifferent = false;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                if (qAbs(mD65.m[i][j] - mD50.m[i][j]) > 1e-6)
                    anyDifferent = true;
        QVERIFY(anyDifferent);
    }

    void labRoundTrip()
    {
        RGB original{0.3, 0.6, 0.9};
        XYZ xyz = rgbToXyz(original, Illuminant::D65);
        Lab lab = xyzToLab(xyz, Illuminant::D65);
        XYZ backXyz = labToXyz(lab, Illuminant::D65);
        QVERIFY(qAbs(backXyz.x - xyz.x) < EPS);
        QVERIFY(qAbs(backXyz.y - xyz.y) < EPS);
        QVERIFY(qAbs(backXyz.z - xyz.z) < EPS);
    }

    void clippingStaysInRange()
    {
        Lab extremeLab{50.0, 300.0, -300.0};
        XYZ xyz = labToXyz(extremeLab, Illuminant::D65);
        bool outOfGamut = false;
        RGB clipped = xyzToRgb(xyz, Illuminant::D65, GamutStrategy::Clipping, outOfGamut);
        QVERIFY(outOfGamut);
        QVERIFY(clipped.r >= -EPS && clipped.r <= 1.0 + EPS);
        QVERIFY(clipped.g >= -EPS && clipped.g <= 1.0 + EPS);
        QVERIFY(clipped.b >= -EPS && clipped.b <= 1.0 + EPS);
    }

    void scalingStaysInRange()
    {
        Lab extremeLab{50.0, 300.0, -300.0};
        XYZ xyz = labToXyz(extremeLab, Illuminant::D65);
        bool outOfGamut = false;
        RGB scaled = xyzToRgb(xyz, Illuminant::D65, GamutStrategy::Scaling, outOfGamut);
        QVERIFY(outOfGamut);
        QVERIFY(scaled.r >= -EPS && scaled.r <= 1.0 + EPS);
        QVERIFY(scaled.g >= -EPS && scaled.g <= 1.0 + EPS);
        QVERIFY(scaled.b >= -EPS && scaled.b <= 1.0 + EPS);
    }

    void normalColorNeverFlaggedOutOfGamut()
    {
        RGB original{0.5, 0.5, 0.5};
        HLS hls = rgbToHls(original);
        bool outOfGamut = false;
        hlsToRgb(hls, GamutStrategy::Clipping, outOfGamut);
        QVERIFY(!outOfGamut);
    }
};

QTEST_MAIN(TestColorMath)
#include "test_colormath.moc"