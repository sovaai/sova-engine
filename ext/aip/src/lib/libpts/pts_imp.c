#include <string.h>
#include <sys/types.h>
# if defined( WIN32 )
#   include "_include/ntoh.h"   /* for ntohl */
# else
#   include <netinet/in.h> /* for ntohl */
# endif
#include "pts_imp.h"

#ifdef _MSC_VER
#pragma warning(disable:4127) // conditional expression is constant
#pragma warning(disable:4702) // unreachable code
#endif

int pts_header_parse_fixed(struct pts_header *ptsh, void *data)
{
    int result = -1;

#define RETURN do { goto finish; } while(0)
#define SHIFT(x) do { result += (x); data = ((char*)data) + (x); } while(0)

    memset(ptsh, 0, sizeof(struct pts_header));

    if(memcmp(data, PTS_SIGNATURE, 4))
        RETURN;
    result = 0;
    
    SHIFT(4);

    ptsh->format = *((unsigned char *)data);
    SHIFT(1);

    if(ptsh->format != PTS_SAVE_DATA_FORMAT && ptsh->format != PTS_SAVE_TRANSPORT_FORMAT)
        RETURN;

    ptsh->format_version = *((unsigned char *)data);
    SHIFT(1);
    
    SHIFT(2);

    if(ptsh->format == PTS_SAVE_DATA_FORMAT)
        ptsh->header_rest = *((uint32_t *)data);
    else
        ptsh->header_rest = ntohl(*((uint32_t *)data));
    SHIFT(sizeof(uint32_t));

#undef SHIFT
#undef RETURN
  finish:

    return result;
}

int pts_header_parse_rest(struct pts_header *ptsh, void *data)
{
    if(ptsh->format == PTS_SAVE_DATA_FORMAT)
        {
            ptsh->pat_tree_offset = *((uint32_t *)data);
              data = ((char*)data) + sizeof(uint32_t);

            ptsh->pat_tree_length = *((uint32_t *)data);
              data = ((char*)data) + sizeof(uint32_t);

        }
    else
        {
            /* transport format */

            ptsh->pat_tree_offset = ntohl(*((uint32_t *)data));
              data = ((char*)data) + sizeof(uint32_t);

            ptsh->pat_tree_length = ntohl(*((uint32_t *)data));
              data = ((char*)data) + sizeof(uint32_t);
        }

    return 0;
}

