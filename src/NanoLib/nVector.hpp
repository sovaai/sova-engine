#ifndef NVECTOR_HPP
#define NVECTOR_HPP

#include <vector>
#include <cstddef>

namespace NanoLib
{
	template<typename T>
	class nVector
	{
	public:
		nVector() {}

	public:
		void clear() noexcept { vVector.clear(); }

	public:
		bool push_back( const T & aItem ) noexcept;
		bool push_back( T && aItem ) noexcept;

		bool push_back( const nVector<T> & aVector ) noexcept;
		bool push_back( nVector<T> && aVector ) noexcept;

	public:
		bool resize( size_t aSize ) noexcept;

	public:
		T & operator[] ( size_t num ) { return vVector[num]; }
		const T & operator[] ( size_t num ) const { return vVector[num]; }

	public:
		size_t size() const noexcept { return vVector.size(); }

	public:
		using iterator = typename std::vector<T>::iterator;
		using const_iterator = typename std::vector<T>::const_iterator;

	public:
		iterator begin() noexcept { return vVector.begin(); }
		const_iterator begin() const noexcept { return vVector.begin(); }
		iterator end() noexcept { return vVector.end(); }
		const_iterator end() const noexcept { return vVector.end(); }

	public:
		T & back() { return vVector.back(); }
		const T & back() const { return vVector.back(); }

	private:
		std::vector<T> vVector;
	};

	template<typename T>
	bool nVector<T>::push_back( const T & aItem ) noexcept
	{
		try
		{
			vVector.push_back( aItem );
		}
		catch( ... )
		{
			return false;
		}
		return true;
	}

	template<typename T>
	bool nVector<T>::push_back( T && aItem ) noexcept
	{
		try
		{
			vVector.push_back( std::move( aItem ) );
		}
		catch( ... )
		{
			return false;
		}
		return true;
	}

	template<typename T>
	bool nVector<T>::push_back( const nVector<T> & aVector ) noexcept
	{
		try
		{
			vVector.reserve( vVector.size() + aVector.size() );
			for( auto & item : aVector )
				vVector.push_back( item );
		}
		catch( ... )
		{
			return false;
		}

		return true;
	}

	template<typename T>
	bool nVector<T>::push_back( nVector<T> && aVector ) noexcept
	{
		try
		{
			vVector.reserve( vVector.size() + aVector.size() );
			for( auto & item : aVector )
				vVector.push_back( std::move( item ) );
			vVector.clear();
		}
		catch( ... )
		{
			return false;
		}

		return true;
	}

	template<typename T>
	bool nVector<T>::resize( size_t aSize ) noexcept
	{
		try
		{
			vVector.resize( aSize );
		}
		catch( ... )
		{
			return false;
		}
		return true;
	}
}

#endif // NVECTOR_HPP

