/**
 * @file   Functor.h
 * @author swizard <me@swizard.info>
 * @date   Mon May 26 21:49:32 2008
 * 
 * @brief  A simple one-value functor class carcass for callbacks and iterations.
 * 
 * 
 */
#ifndef _FUNCTOR_H_
#define _FUNCTOR_H_

/* Simple functor object */
template<typename ValueType, typename ReturnType = int>
class Functor
{
public:
	Functor() { }
	virtual ~Functor() { }

public:
	virtual ReturnType apply(ValueType) = 0;
};

/* A functor adapter with invocation flag mark */
template<typename ValueType, typename ReturnType = int>
class FunctorMarkAdapter : public Functor<ValueType, ReturnType>
{
public:
        FunctorMarkAdapter( Functor<ValueType, ReturnType> &_target ) :
                Functor<ValueType, ReturnType>(),
                target( _target ),
                invokeFlag( false ) { }
        virtual ~FunctorMarkAdapter() { }
        
public:
        FunctorMarkAdapter &operator=( const FunctorMarkAdapter &src )
        {
                target     = src.target;
                invokeFlag = src.invokeFlag;
                return *this;
        }
        
public:
	virtual ReturnType apply(ValueType value)
        {
                invokeFlag = true;
                return target.apply(value);
        }
        
        bool hasInvoked() const { return invokeFlag;  }
        void clearInvokeFlag()  { invokeFlag = false; }
        
protected:
        Functor<ValueType, ReturnType> &target;
        bool invokeFlag;
};

#endif /* _FUNCTOR_H_ */

