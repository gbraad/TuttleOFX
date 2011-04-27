#include "ResizePlugin.hpp"
#include "ResizeProcess.hpp"
#include "ResizeDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace resize {


ResizePlugin::ResizePlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
	_paramOptions		= fetchChoiceParam	( kParamOptions );

	_paramFormat		= fetchChoiceParam	( kParamFormat );

	_paramSplit		= fetchBooleanParam	( kParamSplit );

	_paramDirection		= fetchChoiceParam	( kParamDirection );
	_paramScale		= fetchDoubleParam	( kParamScale );
	_paramSize		= fetchDoubleParam	( kParamOutputDimension );

	_paramOutputFormat	= fetchDouble2DParam	( kParamOutputFormat );
	_paramScaleX		= fetchDoubleParam	( kParamScaleX );
	_paramScaleY		= fetchDoubleParam	( kParamScaleY );

	_paramCenter		= fetchBooleanParam	( kParamCenter );
	_paramCenterPoint	= fetchDouble2DParam	( kParamCenterPoint );

	_paramFilter		= fetchChoiceParam	( kParamFilter );

	updateVisibleTools();
}

void ResizePlugin::updateVisibleTools()
{
	switch(_paramOptions->getValue())
	{
		case eParamFormat :
			_paramFormat		-> setIsSecret	( false );
			_paramSplit		-> setIsSecret	( true );
			_paramDirection		-> setIsSecret	( true );
			_paramScale		-> setIsSecret	( true );
			_paramSize		-> setIsSecret	( true );
			_paramScaleX		-> setIsSecret	( true );
			_paramScaleY		-> setIsSecret	( true );
			_paramOutputFormat	-> setIsSecret	( true );
			break;
		case eParamBox :
			_paramFormat		-> setIsSecret	( true );
			_paramSplit		-> setIsSecret	( false );
			_paramDirection		-> setIsSecret	( false );
			_paramScale		-> setIsSecret	( true );
			_paramScaleX		-> setIsSecret	( true );
			_paramScaleY		-> setIsSecret	( true );

			if( _paramSplit->getValue() == false )
			{
				_paramOutputFormat	-> setIsSecret	( false );
				_paramDirection		-> setIsSecret	( true );
				_paramSize		-> setIsSecret	( true );
			}
			else
			{
				_paramOutputFormat	-> setIsSecret	( true );
				_paramDirection		-> setIsSecret	( false );
				_paramSize		-> setIsSecret	( false );
			}
			break;
		case eParamScale :
			_paramFormat		-> setIsSecret	( true );
			_paramSplit		-> setIsSecret	( false );
			_paramSize		-> setIsSecret	( true );
			_paramOutputFormat	-> setIsSecret	( true );

			if( _paramSplit->getValue() == false )
			{
				_paramScaleX		-> setIsSecret	( false );
				_paramScaleY		-> setIsSecret	( false );
				_paramDirection		-> setIsSecret	( true );
				_paramScale		-> setIsSecret	( true );
			}
			else
			{
				_paramScaleX		-> setIsSecret	( true );
				_paramScaleY		-> setIsSecret	( true );
				_paramDirection		-> setIsSecret	( true );
				_paramScale		-> setIsSecret	( false );
			}
			break;
	}
}


ResizeProcessParams<ResizePlugin::Scalar> ResizePlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	ResizeProcessParams<Scalar> params;

	OfxPointD size = _paramOutputFormat -> getValue();
	params._size.x = size.x;
	params._size.y = size.y;

	return params;
}

void ResizePlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	if( paramName == kParamOptions )
	{
		updateVisibleTools();
	}
	if( paramName == kParamSplit )
	{
		if( _paramSplit->getValue() == false )
		{
			if(_paramOptions->getValue() == eParamBox )
			{
				_paramDirection		-> setIsSecret	( true );
				_paramScale		-> setIsSecret	( true );
				_paramScaleX		-> setIsSecret	( true );
				_paramScaleY		-> setIsSecret	( true );
				_paramOutputFormat	-> setIsSecret	( false );
				_paramSize		-> setIsSecret	( true );
			}
			else
			{
				if(_paramOptions->getValue() == eParamScale )
				{
					_paramSize		-> setIsSecret	( true );
					_paramOutputFormat	-> setIsSecret	( true );
					_paramScaleX		-> setIsSecret	( false );
					_paramScaleY		-> setIsSecret	( false );
					_paramDirection		-> setIsSecret	( true );
					_paramScale		-> setIsSecret	( true );
				}
			}
		}
		else
		{
			if(_paramOptions->getValue() == eParamBox )
			{
				_paramDirection		-> setIsSecret	( false );
				_paramScale		-> setIsSecret	( true );
				_paramScaleX		-> setIsSecret	( true );
				_paramScaleY		-> setIsSecret	( true );
				_paramOutputFormat	-> setIsSecret	( true );
				_paramSize		-> setIsSecret	( false );
			}
			else
			{
				if(_paramOptions->getValue() == eParamScale )
				{
					_paramDirection		-> setIsSecret	( true );
					_paramSize		-> setIsSecret	( true );
					_paramOutputFormat	-> setIsSecret	( true );
					_paramScaleX		-> setIsSecret	( true );
					_paramScaleY		-> setIsSecret	( true );
					_paramScale		-> setIsSecret	( false );
				}
			}
		}
	}
}

bool ResizePlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	using namespace bgil;
	const OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
	const Point2 srcRodCorner( srcRod.x1, srcRod.y1 );
	const Point2 srcRodSize( srcRod.x2 - srcRod.x1, srcRod.y2 - srcRod.y1 );
	const OfxRectD srcRodInDstFrame = { 0, 0, srcRodSize.x, srcRodSize.y };

	bool modified = false;

	ResizeProcessParams<Scalar> params = getProcessParams();
	uint sizex = 0;
	uint sizey = 0;
	switch(_paramOptions->getValue())
	{
		case eParamFormat :
		{
			switch(_paramFormat->getValue())
			{
				case eParamPCVideo:		sizex =  640; sizey =  480; break;
				case eParamNTSC:		sizex =  720; sizey =  486; break;
				case eParamPAL:			sizex =  720; sizey =  576; break;
				case eParamHD:			sizex = 1920; sizey = 1080; break;
				case eParamNTSC169:		sizex =  720; sizey =  486; break;
				case eParamPAL169:		sizex =  720; sizey =  576; break;
				case eParam1kSuper35:		sizex = 1024; sizey =  778; break;
				case eParam1kCinemascope:	sizex =  914; sizey =  778; break;
				case eParam2kSuper35:		sizex = 2048; sizey = 1556; break;
				case eParam2kCinemascope:	sizex = 1828; sizey = 1556; break;
				case eParam4kSuper35:		sizex = 4096; sizey = 3112; break;
				case eParam4kCinemascope:	sizex = 3656; sizey = 3112; break;
				case eParamSquare256:		sizex =  256; sizey =  256; break;
				case eParamSquare512:		sizex =  512; sizey =  512; break;
				case eParamSquare1k:		sizex = 1024; sizey = 1024; break;
				case eParamSquare2k:		sizex = 2048; sizey = 2048; break;
			}
			if(_paramCenter->getValue() == false)
			{ // not center resizing
				rod.x1 = 0;
				rod.y1 = 0;
				rod.x2 = sizex;
				rod.y2 = sizey;
			}
			else
			{ // center resizing
				OfxPointD centerPoint = _paramCenterPoint->getValue();
				rod.x1 = centerPoint.x-sizex*0.5;
				rod.y1 = centerPoint.y-sizey*0.5;
				rod.x2 = centerPoint.x+sizex*0.5;
				rod.y2 = centerPoint.y+sizey*0.5;
			}
			modified = true;
			return true;
		}
		case eParamBox :
		{
			if(_paramSplit->getValue() == false )
			{
				sizex = params._size.x;
				sizey = params._size.y;
			}
			else
			{
				if(_paramDirection->getValue() == eParamSizeX )
				{
					sizex   = _paramSize->getValue();
					sizey   = 1.0*srcRodSize.y*_paramSize->getValue()/srcRodSize.x;
				}
				else
				{
					//eParamSizeY
					sizex   = _paramSize->getValue();
					sizey   = 1.0*srcRodSize.x*_paramSize->getValue()/srcRodSize.y;
				}
			}
			if(_paramCenter->getValue() == false)
			{ // not center resizing
				rod.x1 = 0;
				rod.y1 = 0;
				rod.x2 = sizex;
				rod.y2 = sizey;
			}
			else
			{ // center resizing
				OfxPointD centerPoint = _paramCenterPoint->getValue();
				rod.x1 = centerPoint.x-sizex*0.5;
				rod.y1 = centerPoint.y-sizey*0.5;
				rod.x2 = centerPoint.x+sizex*0.5;
				rod.y2 = centerPoint.y+sizey*0.5;
			}
			modified = true;
			return true;
		}
		case eParamScale :
		{
			const point2<double> pMin( srcRodInDstFrame.x1, srcRodInDstFrame.y1 ); // top left corner
			const point2<double> pMax( srcRodInDstFrame.x2, srcRodInDstFrame.y2 ); // down right corner
			const point2<double> center( srcRodSize * 0.5 );

			uint scalex = 0;
			uint scaley = 0;
			if(_paramSplit->getValue() == false )
			{
				const double pScaleX = _paramScaleX->getValue();
				const double pScaleY = _paramScaleY->getValue();
				if( pScaleX == 0.0 && pScaleY == 0.0 )
					return false;

				scalex = pScaleX;
				scaley = pScaleY;
			}
			else
			{
				const double scale = _paramScale->getValue();
				if( scale == 0.0 )
					return false;

				scalex = scale;
				scaley = scale;
			}
			if(_paramCenter->getValue() == false)
			{ // not center resizing
				rod.x1 = 0;
				rod.y1 = 0;
				rod.x2 = pMax.x * scalex;
				rod.y2 = pMax.y * scaley;
			}
			else
			{ // center resizing
				OfxPointD centerPoint = _paramCenterPoint->getValue();
				rod.x1 = centerPoint.x- ( srcRodSize.x * scalex * 0.5 );
				rod.y1 = centerPoint.y- ( srcRodSize.y * scaley * 0.5 );
				rod.x2 = centerPoint.x+ ( srcRodSize.x * scalex * 0.5 );
				rod.y2 = centerPoint.y+ ( srcRodSize.y * scaley * 0.5 );
			}
			modified = true;
			return true;
		}
	}

	rod.x1 += srcRodCorner.x;
	rod.y1 += srcRodCorner.y;
	rod.x2 += srcRodCorner.x;
	rod.y2 += srcRodCorner.y;
	return modified;
}

void ResizePlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	/*OfxRectD srcRoi;
	const OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
	srcRoi.x1 = srcRod.x1 - 1;
	srcRoi.y1 = srcRod.y1 - 1;
	srcRoi.x2 = srcRod.x2 + 1;
	srcRoi.y2 = srcRod.y2 + 1;
	rois.setRegionOfInterest( *_clipSrc, srcRoi );
*/
	/*ResizeProcessParams<Scalar> params = getProcessParams();
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
//
	OfxRectD srcRoi;
	srcRoi.x1 = srcRod.x1 - 1;
	srcRoi.y1 = srcRod.y1 - 1;
	srcRoi.x2 = srcRod.x2 + 1;
	srcRoi.y2 = srcRod.y2 + 1;
	rois.setRegionOfInterest( *_clipSrc, srcRoi );*/
	/*
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
	OfxRectD dstRod = _clipDst->getCanonicalRod( args.time );

	ResizeProcessParams<Scalar> params = getProcessParams();

	OfxRectD outputRoi = args.regionOfInterest;
	outputRoi.x1 -= dstRod.x1; // to dest rod coordinates
	outputRoi.y1 -= dstRod.y1;
	outputRoi.x2 -= dstRod.x1;
	outputRoi.y2 -= dstRod.y1;
	OfxRectD srcRoi = ;
	srcRoi.x1    += srcRod.x1; // to RoW coordinates
	srcRoi.y1    += srcRod.y1;
	srcRoi.x2    += srcRod.x1;
	srcRoi.y2    += srcRod.y1;
	outputRoi.x1 += dstRod.x1; // to RoW coordinates
	outputRoi.y1 += dstRod.y1;
	outputRoi.x2 += dstRod.x1;
	outputRoi.y2 += dstRod.y1;
	//    srcRoi.x1 += 2; // if we remove 2 pixels to the needed RoI the plugin crash, because it tries to access to this pixels
	//    srcRoi.y1 += 2; // so the calcul of the RoI has a precision of one pixel
	//    srcRoi.x2 -= 2;
	//    srcRoi.y2 -= 2;
	OfxRectD srcRealRoi = rectanglesIntersection( srcRoi, srcRod );
	srcRealRoi = srcRod;*/

	//rois.setRegionOfInterest( *_clipSrc, outputRoi );
/*
	if( _debugDisplayRoi->getValue() )
	{
		_srcRealRoi = srcRealRoi;
		_srcRoi     = srcRoi;
		_dstRoi     = outputRoi;
	}*/
}

bool ResizePlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	ResizeProcessParams<Scalar> params = getProcessParams();
//	if( params._in == params._out )
//	{
//		identityClip = _clipSrc;
//		identityTime = args.time;
//		return true;
//	}
	return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ResizePlugin::render( const OFX::RenderArguments &args )
{
	doGilRender<ResizeProcess>( *this, args );
}


}
}
}
