#ifndef __FlagsStorage_hpp__
#define __FlagsStorage_hpp__

#include <vector>
#include <cmath>

#include "InfEngine2/_Include/BinaryData.hpp"

namespace NanoLib
{
	class FlagsStorage
	{
	public:
		void pre_allocate( const unsigned int key, unsigned int size )
		{
			unsigned int pos;
			if( !find( key, pos ) )
				add( key, pos );
			pre_allocate_int( pos, size );
		}

	private:
		void pre_allocate_int( unsigned int pos, unsigned int size )
		{
			unsigned int shift = data.size();
			data.resize( data.size() + std::ceil( ((double)size)/8 ) );
			bzero( static_cast<char*>( data.data() ) + shift, data.size() - shift );
			shifts[2*pos] = 8*( data.size() - shift );
		}

		void add( unsigned int key, unsigned int pos )
		{
			shifts.resize( shifts.size() + 2 );
			index.resize( index.size() + 1 );

			memmove( static_cast<uint32_t*>( index.data() ) + pos + 1, static_cast<uint32_t*>( index.data() ) + pos, (index.size() - pos - 1)*sizeof(uint32_t) );
			index[pos] = key;

			memmove( static_cast<uint32_t*>( shifts.data() ) + 2*pos + 2, static_cast<uint32_t*>( shifts.data() ) + 2*pos, (shifts.size() - 2*pos - 2)*sizeof(uint32_t) );
			shifts[2*pos] = 0;
			shifts[2*pos + 1] = data.size();
		}

	public:
		void set( unsigned int key, unsigned int ind, bool flag = true )
		{
			unsigned int pos;
			if( !find( key, pos ) )
				add( key, pos );

			if( shifts[2*pos] <= ind )
				pre_allocate_int( pos, ind + 1 );

			unsigned int shift = std::floor( ind/8 );
			if( flag )
				static_cast<char*>( data.data() )[shifts[2*pos+1] + shift] |= 1 << ( ind - 8*shift );
			else
				static_cast<char*>( data.data() )[shifts[2*pos+1] + shift] &= ~( 1 << ( ind - 8*shift ) );
		}

		void unset( unsigned int key, unsigned int ind )
			{ set( key, ind, false ); }

		bool get( unsigned int key, unsigned int ind ) const
		{
			unsigned int pos;
			if( !find( key, pos ) )
				return false;

			unsigned int shift = std::floor( ind/8 );
			return static_cast<const char*>( data.data() )[shifts[2*pos+1] + shift] & ( 1 << ( ind - 8*shift ) );
		}

	public:
		unsigned int size() const
			{ return ( index.size() + 1)*sizeof(uint32_t) + 2*index.size()*sizeof(uint32_t) + sizeof(uint32_t) + data.size(); }

		unsigned int save( void * buffer ) const
		{
			char * ptr = static_cast<char*>( buffer );
			binary_data_save<uint32_t>( buffer, index.size() );
			binary_data_save<uint32_t>( buffer, data.size() );
			binary_data_save( buffer, static_cast<const uint32_t*>( index.data() ), index.size() );
			binary_data_save( buffer, static_cast<const uint32_t*>( shifts.data() ), 2*index.size() );
			binary_data_save( buffer, static_cast<const char*>( data.data() ), data.size() );
            return static_cast<char*>( buffer ) - ptr;
		}

		void load( const void * buffer )
		{
			reset();
			uint32_t size = *binary_data_get<uint32_t>( buffer );
			uint32_t data_size = *binary_data_get<uint32_t>( buffer, sizeof(uint32_t) );
			data.resize( data_size );
			shifts.resize( 2*size );
			index.resize( size );
			memcpy( index.data(), binary_data_get<uint32_t>( buffer, 2*sizeof(uint32_t) ), size*sizeof(uint32_t) );
			memcpy( shifts.data(), binary_data_get<uint32_t>( buffer, (2 + size)*sizeof(uint32_t) ), 2*size*sizeof(uint32_t) );
			memcpy( data.data(), binary_data_get<uint32_t>( buffer, (2 + 3*size)*sizeof(uint32_t) ), data_size );
		}

		void reset()
		{
			index.clear();
			shifts.clear();
			data.clear();
		}

	private:
		bool find( unsigned int key, unsigned int & pos ) const
		{
			auto ptr = std::lower_bound( index.begin(), index.end(), key );
			if( ptr == index.end() )
			{
				pos = index.size();
				return false;
			}
			else if( *ptr == key )
			{
				pos = ptr - index.begin();
				return true;
			}
			else
			{
				pos = ptr - index.begin();
				return false;
			}
		}

	private:
		std::vector<uint32_t> index;
		std::vector<uint32_t> shifts;
		std::vector<char> data;
	};
}

#endif /** __FlagsStorage_hpp__ */
