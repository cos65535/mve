#ifndef MVSTOOLS_H
#define MVSTOOLS_H

#include <iostream>

#include "defines.h"
#include "math/matrix.h"
#include "math/vector.h"
#include "mve/image.h"
#include "SingleView.h"
#include "util/refptr.h"


MVS_NAMESPACE_BEGIN


/** initialize mapping from SRGB to linear color space */
void initSRGB2linear();

/** interpolate color and derivative at given sample positions */
void colAndExactDeriv(util::RefPtr<mve::ImageBase> img,
    PixelCoords const& imgPos, PixelCoords const& gradDir,
    Samples& color, Samples& deriv);

/** get color at given pixel positions (no interpolation) */
void getXYZColorAtPix(util::RefPtr<mve::ImageBase> img,
    std::vector<math::Vec2i> const& imgPos, Samples* color);

/** interpolate only color at given sample positions */
void getXYZColorAtPos(util::RefPtr<mve::ImageBase> img,
    PixelCoords const& imgPos, Samples* color);

/** Computes the parallax between two views with respect to some 3D point p */
float parallax(math::Vec3f p, mvs::SingleViewPtr v1, mvs::SingleViewPtr v2);

/** Turns a parallax value (0 <= p <= 180) into a weight according
    to a bilateral Gaussian (see [Furukawa 2010] for details) */
float parallaxToWeight(float p);


/* ------------------------- Implementation ----------------------- */

inline float
parallax(math::Vec3f p, mvs::SingleViewPtr v1, mvs::SingleViewPtr v2)
{
    math::Vec3f dir1 = (p - v1->camPos).normalized();
    math::Vec3f dir2 = (p - v2->camPos).normalized();
    float dp = std::min(dir1.dot(dir2), 1.f);
    float plx = std::acos(dp) * 180.f / pi;
    return plx;
}

inline float
parallaxToWeight(float p)
{
    if (p < 0.f || p > 180.f) {
        std::cerr << "ERROR: invalid parallax value." << std::endl;
        return 0.f;
    }
    float sigma;
    if (p <= 20.f)
        sigma = 5.f;
    else
        sigma = 15.f;
    float mean = 20.f;
    return exp(- sqr(p - mean) / (2 * sigma * sigma));
}


MVS_NAMESPACE_END

#endif
