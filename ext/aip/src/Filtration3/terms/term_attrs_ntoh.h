#ifndef __terms_attrs_ntoh_h__
#define __terms_attrs_ntoh_h__

#include "_include/ntoh.h"

/* byte order conversion */
#define __term_attrs_ntoh(dest, src) \
  do { \
    ((short*)(dest))[TDATA_RID_OFS] = ntohs(((short*)(src))[TDATA_RID_OFS]); \
    ((short*)(dest))[TDATA_FLG_OFS] = ntohs(((short*)(src))[TDATA_FLG_OFS]); \
    ((short*)(dest))[TDATA_LNG_OFS] = ntohs(((short*)(src))[TDATA_LNG_OFS]); \
    ((char*)(dest))[TDATA_BYTES_CMPL_OFS] = ((char*)(src))[TDATA_BYTES_CMPL_OFS]; \
    ((char*)(dest))[TDATA_BYTES_WGT_OFS] = ((char*)(src))[TDATA_BYTES_WGT_OFS]; \
  } while(0)

#define __term_attrs_hton(dest, src) \
do { \
    ((short*)(dest))[TDATA_RID_OFS] = htons(((short*)(src))[TDATA_RID_OFS]); \
    ((short*)(dest))[TDATA_FLG_OFS] = htons(((short*)(src))[TDATA_FLG_OFS]); \
    ((short*)(dest))[TDATA_LNG_OFS] = htons(((short*)(src))[TDATA_LNG_OFS]); \
    ((char*)(dest))[TDATA_BYTES_CMPL_OFS] = ((char*)(src))[TDATA_BYTES_CMPL_OFS]; \
    ((char*)(dest))[TDATA_BYTES_WGT_OFS] = ((char*)(src))[TDATA_BYTES_WGT_OFS]; \
  } while(0)


/* CPP-style usage */
#ifdef __cplusplus
  inline void terms_attr_ntoh(cf_term_attrs* dest, const cf_term_attrs* src)
    { __term_attrs_ntoh(dest, src); }
  inline void terms_attr_hton(cf_term_attrs* dest, const cf_term_attrs* src)
    { __term_attrs_hton(dest, src); }
#endif


#endif /* __terms_attrs_ntoh_h__ */
