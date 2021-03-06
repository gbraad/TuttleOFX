#include "BlurPlugin.hpp"

#include <terry/filter/gaussianKernel.hpp>
#include <terry/filter/convolve.hpp>

#include <tuttle/plugin/memory/OfxAllocator.hpp>

namespace tuttle
{
namespace plugin
{
namespace blur
{

template <class View>
BlurProcess<View>::BlurProcess(BlurPlugin& effect)
    : ImageGilFilterProcessor<View>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
}

template <class View>
void BlurProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);
    _params = _plugin.getProcessParams(args.renderScale);

    //	TUTTLE_LOG_VAR( TUTTLE_INFO, _params._size );
    //	TUTTLE_LOG_VAR2( TUTTLE_INFO, _params._gilKernelX.size(), _params._gilKernelY.size() );
    //	std::cout << "x [";
    //	std::for_each(_params._gilKernelX.begin(), _params._gilKernelX.end(), std::cout << boost::lambda::_1 << ',');
    //	std::cout << "]" << std::endl;
    //	std::cout << "y [";
    //	std::for_each(_params._gilKernelY.begin(), _params._gilKernelY.end(), std::cout << boost::lambda::_1 << ',');
    //	std::cout << "]" << std::endl;
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void BlurProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace terry;
    using namespace terry::filter;

    const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    View dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    const Point proc_tl(procWindowRoW.x1 - this->_srcPixelRod.x1, procWindowRoW.y1 - this->_srcPixelRod.y1);

    if(_params._size.x == 0)
    {
        correlate_cols_auto<Pixel>(this->_srcView, _params._gilKernelY, dst, proc_tl, _params._boundary_option);
    }
    else if(_params._size.y == 0)
    {
        correlate_rows_auto<Pixel>(this->_srcView, _params._gilKernelX, dst, proc_tl, _params._boundary_option);
    }
    else
    {
        correlate_rows_cols_auto<Pixel, OfxAllocator>(this->_srcView, _params._gilKernelX, _params._gilKernelY, dst, proc_tl,
                                                      _params._boundary_option);
    }
}
}
}
}
