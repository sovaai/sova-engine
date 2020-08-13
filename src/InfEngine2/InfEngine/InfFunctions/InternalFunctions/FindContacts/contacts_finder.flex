%option noyywrap
%option prefix="contacts_finder_"
%option fast 8bit

%TOP{
	#define YY_DECL int nanolex( std::string& result )

	#include <contacts_finder.hpp>
}

NAME	[^()<>@,;:\\"\[\]\0-\40\200-\377]+
QNAME	\"([^"\0-\40\200-\377]|\\\")+\"
FNAME	{NAME}|({QNAME}|{NAME}\.)*{QNAME}{NAME}?
DOMAIN	[^\.]([^()<>@,;:\\"\[\]\0-\40\200-\377])+
EMAIL	{FNAME}@{DOMAIN}

DIGIT	[0-9]
SPACE	[ \t\v\f]*

INTSTART	(\+7({SPACE}*8)?|8|7){SPACE}*(10{SPACE}*7{SPACE}*)?
INTCODE		(-?{DIGIT}{2,3}-?|\({DIGIT}{2,3}\)){SPACE}*
INTNUMBER	({DIGIT}{SPACE}*-?{SPACE}){7}

EXTSTART	(\+|00)?{SPACE}
EXTCODEC	(1|1242|1246|1264|1268|1284|1340|1345|1441|1473|1664|1670|1671|1758|1767|1784|1787|1808|1808|1829|1868|1869|1876|262|297|298|299|30|31|32|33|3395|34|34928|350|351|352|353|354|355|356|357|358|359|36|370|371|372|373|374|375|376|377|378|380|381|382|385|386|387|389|39|40|41|420|421|423|43|44|45|46|47|48|49|501|502|503|504|505|506|507|509|51|52|53|54|55|56|57|58|590|591|592|593|594|595|596|597|598|599|60|61|62|63|64|65|66|672|673|675|676|678|679|685|686|687|689|690|691|7|81|82|84|840|850|852|853|855|856|86|880|886|90|91|92|93|94|95|960|961|962|963|964|965|966|967|968|971|972|973|974|975|976|977|98|992|993|994|995|996|998)
EXTCODEI	{DIGIT}{1,4}
EXTCODER	{SPACE}({EXTCODEI}|\({SPACE}{EXTCODEI}{SPACE}\)|-{EXTCODEI}-){SPACE}
EXTNUMBER	({DIGIT}{SPACE}*-?{SPACE}){5,7}
PHONE	(({INTSTART}?{INTCODE})?{INTNUMBER}|{EXTSTART}{EXTCODEC}{EXTCODER}{EXTNUMBER})

%x PHONESTART PHONEEND PHONEERROR

%%
{EMAIL}	result = contacts_finder_text; return contacts_finder::email;
{DIGIT}|\+|\(	BEGIN( PHONESTART ); yyless( 0 );
<PHONESTART>{PHONE}	BEGIN( PHONEEND ); result = contacts_finder_text;
<PHONESTART>.|\n	|
<PHONESTART><<EOF>>	BEGIN( INITIAL ); result.clear();

<PHONEEND>{SPACE}
<PHONEEND>-?{SPACE}*{DIGIT}	BEGIN( PHONEERROR ); result.clear();
<PHONEEND>.|\n	|
<PHONEEND><<EOF>>	BEGIN( INITIAL ); return contacts_finder::phone;

<PHONEERROR>{DIGIT}|{SPACE}|-
<PHONEERROR>.|\n	|
<PHONEERROR><<EOF>>	BEGIN( INITIAL );
.|\n
<<EOF>>	result.clear(); return contacts_finder::eof;
%%

contacts_finder::contacts_finder( const std::string& request ): v_request( request )
{
	YY_BUFFER_STATE current = YY_CURRENT_BUFFER;
	v_state = yy_scan_string( v_request.c_str() );
	if( current )
		yy_switch_to_buffer( current );
	v_last_type = none;
}

contacts_finder::~contacts_finder()
{
	yy_delete_buffer( v_state );
}

contacts_finder::type contacts_finder::search( std::string &result )
{
	YY_BUFFER_STATE current = YY_CURRENT_BUFFER;
	yy_switch_to_buffer( v_state );
	switch( nanolex( result ) )
	{
		case eof:
			v_last_type = eof;
			break;
		case phone:
			v_last_type = phone;
			break;
		case email:
			v_last_type = email;
			break;
		default:
			v_last_type = error;
	}
	if( current )
		yy_switch_to_buffer( current );
	return v_last_type;
}

