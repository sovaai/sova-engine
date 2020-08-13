#ifndef __contacts_finder_hpp__
#define __contacts_finder_hpp__

#include <string>

class yy_buffer_state;

class contacts_finder
{
public:
	contacts_finder( const std::string& request );
	~contacts_finder();

public:
	enum type
	{
		eof = 0,
		phone = 1,
		email = 2,
		error = 3,
		none = 4
	};

	type search( std::string &result );

	type last_type() const { return v_last_type; }

private:
	yy_buffer_state* v_state;
	const std::string& v_request;
	type v_last_type;
};

#endif /** __contacts_finder_hpp__ */
