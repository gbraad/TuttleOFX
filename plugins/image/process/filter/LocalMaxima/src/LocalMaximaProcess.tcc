#include "LocalMaximaPlugin.hpp"

#include <tuttle/plugin/exceptions.hpp>
#include <tuttle/plugin/numeric/rectOp.hpp>
#include <tuttle/plugin/ofxToGil/rect.hpp>

#include <terry/globals.hpp>
#include <terry/filter/localMaxima.hpp>
#include <terry/algorithm/transform_pixels_progress.hpp>
#include <terry/numeric/operations.hpp>

#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/algorithm.hpp>

#include <boost/math/constants/constants.hpp>

namespace tuttle
{
namespace plugin
{
namespace localmaxima
{

template <class SView, class DView>
LocalMaximaProcess<SView, DView>::LocalMaximaProcess(LocalMaximaPlugin& effect)
    : ImageGilFilterProcessor<SView, DView>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
}

template <class SView, class DView>
void LocalMaximaProcess<SView, DView>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<SView, DView>::setup(args);
    _params = _plugin.getProcessParams(args.renderScale);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class SView, class DView>
void LocalMaximaProcess<SView, DView>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace terry;
    using namespace terry::algorithm;
    using namespace terry::numeric;

    namespace bm = boost::math;

    typedef point2<std::ptrdiff_t> Point2;

    const OfxRectI procWindowOutput = translateRegion(procWindowRoW, this->_dstPixelRod);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    static const unsigned int border = 1;
    const OfxRectI srcRodCrop = rectangleReduce(this->_srcPixelRod, border);
    const OfxRectI procWindowRoWCrop = rectanglesIntersection(procWindowRoW, srcRodCrop);

    //	const OfxRectI procWindowOutputCrop = translateRegion( procWindowRoWCrop, this->_dstPixelRod );

    if(_params._border == eParamBorderBlack)
    {
        DView dst =
            subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

        // fill borders
        DPixel pixelZero;
        pixel_zeros_t<DPixel>()(pixelZero);
        boost::gil::fill_pixels(dst, pixelZero);
    }

    transform_pixels_locator_progress(this->_srcView, ofxToGil(this->_srcPixelRod), this->_dstView,
                                      ofxToGil(this->_dstPixelRod), ofxToGil(procWindowRoWCrop),
                                      terry::filter::pixel_locator_gradientLocalMaxima_t<SView, DView>(this->_srcView),
                                      this->getOfxProgress());
}
}
}
}
