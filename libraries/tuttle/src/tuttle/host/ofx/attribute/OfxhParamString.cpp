#include "OfxhParamString.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

void OfxhParamString::set( const char* v, const EChange change ) OFX_EXCEPTION_SPEC
{
	set( std::string(v), change );
}

void OfxhParamString::set( const OfxTime time, const char* v, const EChange change ) OFX_EXCEPTION_SPEC
{
	set( time, std::string(v), change );
}

void OfxhParamString::getV( va_list arg ) const OFX_EXCEPTION_SPEC
{
	const char** value = va_arg( arg, const char** );
	get( _returnValue ); /// @todo tuttle: "I so don't like this, temp storage should be delegated to the implementation"

	*value = _returnValue.c_str();
}

/**
 * implementation of var args function
 */
void OfxhParamString::getV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC
{
	const char** value = va_arg( arg, const char** );
	get( time, _returnValue ); // I so don't like this, temp storage should be delegated to the implementation

	*value = _returnValue.c_str();
}

/**
 * implementation of var args function
 */
void OfxhParamString::setV( va_list arg, const EChange change ) OFX_EXCEPTION_SPEC
{
	char* value = va_arg( arg, char* );
	set( value, change );
}

/**
 * implementation of var args function
 */
void OfxhParamString::setV( const OfxTime time, va_list arg, const EChange change ) OFX_EXCEPTION_SPEC
{
	char* value = va_arg( arg, char* );
	set( time, value, change );
}


}
}
}
}
