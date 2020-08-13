#ifndef __terms_h__
#define __terms_h__
/********************************************************************************/
/* TERMS BASE EXTERNAL C-Style API                                              */
/********************************************************************************/
#include "lib/fstorage/fstorage.h"
#include "lib/aptl/avector.h"
#include "../common/DocMessage.h"
#include "terms_types.h"

class LingProc;
class DocImage;
class FltTermsMatcher;

/*------------------------------------------------------------------------------*/
/* FUNCTION PROTOTYPES                                                          */
/*------------------------------------------------------------------------------*/
extern "C" {

/* create + save */
enum FltTermsErrors cf_terms_base_create(HTERMSBASE* phtb, LingProc* lp);
enum FltTermsErrors cf_terms_base_create_f(HTERMSBASE* phtb, LingProc* lp, uint32_t flags);
enum FltTermsErrors cf_terms_base_create_dfn(HTERMSBASE* phtb, LingProc* lp, const char* defaults_path);
enum FltTermsErrors cf_terms_base_create_dfs(HTERMSBASE* phtb, LingProc* lp, fstorage* defaults);
enum FltTermsErrors cf_terms_base_save_fn(HTERMSBASE* phtb, struct cf_terms_save_params* params, const char* path);
enum FltTermsErrors cf_terms_base_save_fs(HTERMSBASE* phtb, struct cf_terms_save_params* params, fstorage* fs);

/* open (mode = TERM_BASE_OPEN_CREATE also allowed) + save */
enum FltTermsErrors cf_terms_base_open(HTERMSBASE* phtb, LingProc* lp, const char* path, enum FltTermsBaseOpenModes mode);
enum FltTermsErrors cf_terms_base_open_fs(HTERMSBASE* phtb, LingProc* lp, fstorage* fs, enum FltTermsBaseOpenModes mode);
enum FltTermsErrors cf_terms_base_save(HTERMSBASE* phtb, struct cf_terms_save_params* params);

enum FltTermsErrors cf_terms_base_close(HTERMSBASE* phtb);

/* terms enumeration */
enum FltTermsErrors cf_terms_enum(HTERMSBASE htb,
                                cf_terms_enum_callback callback, void *data, bool for_export);
// breaks if callback() returns non-zero value

/* add new term */
enum FltTermsErrors cf_terms_add(HTERMSBASE htb, const cf_term_attrs* attrs, const char* text); 
// returns TERMS_OK (=0) on succses, negative value on errors, positive value on warnings
// possible warnings: TERMS_WARN_EMPTY, TERMS_WARN_EXIST
        
/* fill stop dictionary */
enum FltTermsErrors cf_terms_add_stop_words(HTERMSBASE htb, const char **stop_words, size_t stop_words_size);
/* remove words from stop dictionary */
enum FltTermsErrors cf_terms_clr_stop_words(HTERMSBASE htb, const char **stop_words, size_t stop_words_size);
/* remove all words from stop dictionary */
enum FltTermsErrors cf_terms_clr_all_words(HTERMSBASE htb);
        
/* terms check */
int cf_terms_check_res(HTERMSBASE htb, struct cf_terms_check_result* res, size_t res_size,
                       const DocImage* di, const CDocMessage::CoordMapping* mapping);
// returns non-negative amount of terms_check_result's or negative value (enum TermsErrors) on errors
// check for enough space for results; res or res_size can be zero for space checking


enum FltTermsErrors cf_terms_check(HTERMSBASE htb, const DocImage* di, const CDocMessage::CoordMapping* mapping);

size_t cf_terms_last_res_size(HTERMSBASE htb);
int cf_terms_last_res(HTERMSBASE htb, struct cf_terms_check_result* res, size_t res_size);
// returns non-negative amount of last terms_check_result's or negative value (enum TermsErrors) on errors
// check for enough space for results; res or res_size can be zero for space checking

size_t cf_terms_last_found_terms_size(HTERMSBASE htb);
int cf_terms_last_found_terms(HTERMSBASE htb, struct cf_terms_check_info* info,
     struct cf_terms_check_found_term* terms, size_t max_size);
int cf_terms_last_found_terms_cp(HTERMSBASE htb, struct cf_terms_check_info* info,
     struct cf_terms_check_found_term* terms, size_t max_size);
// returns non-negative amount of last terms_check_found_term's or negative value (enum TermsErrors) on errors
// check for enough space for results; res or res_size can be zero for space checking


// multi-thread
enum FltTermsErrors cf_terms_check_ex(HTERMSBASE htb,
                                  FltTermsMatcher* matcher, const DocImage* di);

} /* extern "C" */

/*------------------------------------------------------------------------------*/
#endif
