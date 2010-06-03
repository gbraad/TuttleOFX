#ifndef _TUTTLE_HOST_OFX_PARAM_HPP_
#define _TUTTLE_HOST_OFX_PARAM_HPP_

#include "OfxhAttribute.hpp"
#include "OfxhParamAccessor.hpp"

#include <tuttle/host/ofx/OfxhCore.hpp>
#include <tuttle/host/ofx/OfxhException.hpp>
#include <tuttle/host/ofx/OfxhIObject.hpp>

#include <tuttle/host/ofx/property/OfxhSet.hpp>
#include <tuttle/host/ofx/property/OfxhGetHook.hpp>
#include <tuttle/host/ofx/property/OfxhNotifyHook.hpp>

#include <string>

#define TUTTLE_DEFINE_OFXHPARAM_ACCESSORS( Name, Type ) \
	protected:\
	inline virtual void get( Type& ) const OFX_EXCEPTION_SPEC \
	{\
		throw OfxhException( kOfxStatErrBadHandle, "Not a " #Name " parameter." ); \
	}\
	inline virtual void get( const OfxTime, Type& ) const OFX_EXCEPTION_SPEC \
	{\
		throw OfxhException( kOfxStatErrBadHandle, "Not a " #Name " parameter." ); \
	}\
	inline virtual void getAt( Type&, const std::size_t ) const OFX_EXCEPTION_SPEC \
	{\
		throw OfxhException( kOfxStatErrBadHandle, "Not a " #Name " multidimentional parameter." ); \
	}\
	inline virtual void getAt( const OfxTime, Type&, const std::size_t ) const OFX_EXCEPTION_SPEC \
	{\
		throw OfxhException( kOfxStatErrBadHandle, "Not a " #Name " multidimentional parameter." ); \
	}\
	public:\
	inline Type get##Name##At( const std::size_t index ) const OFX_EXCEPTION_SPEC \
	{\
		Type dst;\
		getAt( dst, index );\
		return dst;\
	}\
	inline Type get##Name##At( const OfxTime time, const std::size_t index ) const OFX_EXCEPTION_SPEC \
	{\
		Type dst;\
		getAt( time, dst, index );\
		return dst;\
	}\
	inline Type get##Name() const OFX_EXCEPTION_SPEC \
	{\
		Type dst;\
		get( dst );\
		return dst;\
	}\
	inline Type get##Name( const OfxTime time ) const OFX_EXCEPTION_SPEC \
	{\
		Type dst;\
		get( time, dst );\
		return dst;\
	}\
	inline virtual void set( const Type& value, const EChange change ) OFX_EXCEPTION_SPEC { throw OfxhException( kOfxStatErrBadHandle, "Not a " #Name " parameter." ); } \
	inline virtual void set( const Type& value ) OFX_EXCEPTION_SPEC { set( value, eChangeUserEdited ); } \
	inline virtual void setAtTime( const OfxTime time, const Type& value, const EChange change ) OFX_EXCEPTION_SPEC { throw OfxhException( kOfxStatErrBadHandle, "Not a " #Name " parameter." ); }\
	inline virtual void setAtTime( const OfxTime time, const Type& value ) OFX_EXCEPTION_SPEC { setAtTime( time, value, eChangeUserEdited ); }\
	inline virtual void setAt( const Type& value, const std::size_t index, const EChange change ) OFX_EXCEPTION_SPEC { throw OfxhException( kOfxStatErrBadHandle, "Not a multi-" #Name " parameter." ); } \
	inline virtual void setAt( const Type& value, const std::size_t index ) OFX_EXCEPTION_SPEC { setAt( value, index, eChangeUserEdited ); } \
	inline virtual void set( const Type& value1, const Type& value2, const EChange change ) OFX_EXCEPTION_SPEC { throw OfxhException( kOfxStatErrBadHandle, "Not a multi-" #Name " parameter (2)." ); } \
	inline virtual void set( const Type& value1, const Type& value2 ) OFX_EXCEPTION_SPEC { set( value1, value2, eChangeUserEdited ); } \
	inline virtual void setAtTime( const OfxTime time, const Type& value1, const Type& value2, const EChange change ) OFX_EXCEPTION_SPEC { throw OfxhException( kOfxStatErrBadHandle, "Not a multi-" #Name " parameter (2)." ); }\
	inline virtual void setAtTime( const OfxTime time, const Type& value1, const Type& value2 ) OFX_EXCEPTION_SPEC { setAtTime( time, value1, value2, eChangeUserEdited ); }\
	inline virtual void set( const Type& value1, const Type& value2, const Type& value3, const EChange change ) OFX_EXCEPTION_SPEC { throw OfxhException( kOfxStatErrBadHandle, "Not a multi-" #Name " parameter (3)." ); } \
	inline virtual void set( const Type& value1, const Type& value2, const Type& value3 ) OFX_EXCEPTION_SPEC { set( value1, value2, value3, eChangeUserEdited ); } \
	inline virtual void setAtTime( const OfxTime time, const Type& value1, const Type& value2, const Type& value3, const EChange change ) OFX_EXCEPTION_SPEC { throw OfxhException( kOfxStatErrBadHandle, "Not a " #Name " parameter (3)." ); }\
	inline virtual void setAtTime( const OfxTime time, const Type& value1, const Type& value2, const Type& value3 ) OFX_EXCEPTION_SPEC { setAtTime( time, value1, value2, value3, eChangeUserEdited ); }\


namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

class OfxhParamDescriptor;
class OfxhParamSet;

/// plugin parameter instance
class OfxhParam :
	public OfxhAttribute,
	virtual public OfxhParamAccessor,
	protected property::OfxhNotifyHook,
	private boost::noncopyable
{
OfxhParam();

protected:
	OfxhParamSet*  _paramSetInstance;
	OfxhParam*     _parentInstance;
	bool           _avoidRecursion;		///< Avoid recursion when updating with paramChangedAction

protected:
	OfxhParam( const OfxhParam& other )
		: OfxhAttribute( other ),
		_paramSetInstance( other._paramSetInstance ),
		_parentInstance( other._parentInstance ),
		_avoidRecursion(false)
	{
		/// @todo tuttle : copy content, not pointer ?
	}

public:
	/// make a parameter, with the given type and name
	explicit OfxhParam( const OfxhParamDescriptor& descriptor, const std::string& name, OfxhParamSet& setInstance );

	virtual ~OfxhParam() = 0;

	/// clone this parameter
	virtual OfxhParam* clone() const = 0;
	/**
	 * @todo check values...
	 */
	bool operator==( const OfxhParam& p ) const { return true; }

	/// grab a handle on the parameter for passing to the C API
	OfxParamHandle getParamHandle() const
	{
		return ( OfxParamHandle ) this;
	}

#ifndef SWIG
	void paramChanged( const EChange change );

	void changedActionBegin() { _avoidRecursion = true; }
	void changedActionEnd() { _avoidRecursion = false; }
	bool changedActionInProgress() const { return _avoidRecursion; }

	// get the param instance
	OfxhParamSet* getParamSetInstance()                         { return _paramSetInstance; }
	void          setParamSetInstance( OfxhParamSet* instance ) { _paramSetInstance = instance; }

	// set/get parent instance
	void       setParentInstance( OfxhParam* instance );
	OfxhParam* getParentInstance();

	// copy one parameter to another
	virtual void copy( const OfxhParam& instance ) OFX_EXCEPTION_SPEC = 0;

	// copy one parameter to another
	virtual void copy( const OfxhParam& instance, OfxTime offset ) OFX_EXCEPTION_SPEC;

	// copy one parameter to another, with a range
	virtual void copy( const OfxhParam& instance, OfxTime offset, OfxRangeD range ) OFX_EXCEPTION_SPEC;

	// callback which should set enabled state as appropriate
	virtual void setEnabled();

	// callback which should set secret state as appropriate
	virtual void setSecret();

	/// callback which should update label
	virtual void setLabel();

	/// callback which should set
	virtual void setDisplayRange();

	// va list calls below turn the var args (oh what a mistake)
	// suite functions into virtual function calls on instances
	// they are not to be overridden by host implementors by
	// by the various typeed param instances so that they can
	// deconstruct the var args lists

#endif
	inline virtual std::size_t getSize() const 
	{
		return 1;
	}

	TUTTLE_DEFINE_OFXHPARAM_ACCESSORS( String, std::string );
	TUTTLE_DEFINE_OFXHPARAM_ACCESSORS( Int, int );
	TUTTLE_DEFINE_OFXHPARAM_ACCESSORS( Double, double );
	TUTTLE_DEFINE_OFXHPARAM_ACCESSORS( Bool, bool );

	inline virtual void set( const char* value, const EChange change ) OFX_EXCEPTION_SPEC { throw OfxhException( kOfxStatErrBadHandle, "Not a string parameter." ); }
	inline virtual void set( const char* value ) OFX_EXCEPTION_SPEC { set( value, eChangeUserEdited ); }
	inline virtual void set( const OfxTime time, const char* value, const attribute::EChange change ) OFX_EXCEPTION_SPEC { throw OfxhException( kOfxStatErrBadHandle, "Not a string parameter." ); }
	inline virtual void set( const OfxTime time, const char* value ) OFX_EXCEPTION_SPEC { set( time, value, eChangeUserEdited ); }

#ifndef SWIG
	/// get a value, implemented by instances to deconstruct var args
	virtual void getV( va_list arg ) const OFX_EXCEPTION_SPEC;

	/// get a value, implemented by instances to deconstruct var args
	virtual void getV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC;

	/// set a value, implemented by instances to deconstruct var args
	virtual void setV( va_list arg, const EChange change ) OFX_EXCEPTION_SPEC;

	/// key a value, implemented by instances to deconstruct var args
	virtual void setV( const OfxTime time, va_list arg, const EChange change ) OFX_EXCEPTION_SPEC;

	/// derive a value, implemented by instances to deconstruct var args
	virtual void deriveV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC;

	/// integrate a value, implemented by instances to deconstruct var args
	virtual void integrateV( const OfxTime time1, const OfxTime time2, va_list arg ) const OFX_EXCEPTION_SPEC;

	/// overridden from Property::NotifyHook
	virtual void notify( const std::string& name, bool single, int num ) OFX_EXCEPTION_SPEC;
#endif
};

#ifndef SWIG
/**
 * @brief to make ParamInstance clonable (for use in boost::ptr_container)
 */
inline OfxhParam* new_clone( const OfxhParam& a )
{
	return a.clone();
}
#endif

}
}
}
}

#ifndef SWIG
// force boost::is_virtual_base_of value (used by boost::serialization)
namespace boost{
template<> struct is_virtual_base_of<tuttle::host::ofx::attribute::OfxhAttribute, tuttle::host::ofx::attribute::OfxhParam>: public mpl::true_ {};
}
#endif


#endif
