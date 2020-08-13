#ifndef __DocMessage_H
#define __DocMessage_H
//============================================================================//
#include "LingProc4/LingProc.h"
#include "LingProc4/DocText/DocText.h"
//#include "Filtration3/API/message.h"
//----------------------------------------------------------------------------//
#ifndef MAX_FILTRATION_SIZE
#define MAX_FILTRATION_SIZE 16*1024
#endif
//============================================================================//
class CDocMessage : public DocText
{
public:
    struct CoordMappingInterval{
        int32_t orig_begin, orig_end;
        CoordMappingInterval():
            orig_begin(0), orig_end(0){}
    };
    typedef avector<CoordMappingInterval> CoordMapping;
private:
  CoordMapping mapping;
  // copy prevention
  // CDocMessage(const CDocMessage&) { assert(false); abort(); }
  CDocMessage(const DocText&) { assert(false); abort(); }
  CDocMessage& operator = (const CDocMessage&) { assert(false); abort(); return *this; }
  CDocMessage& operator = (const DocText&) { assert(false); abort(); return *this; }

public:
  CDocMessage(size_t MaxTextSize = 0) : DocText()
    { m_MaxTextSize = MaxTextSize > 0 ? MaxTextSize : MAX_FILTRATION_SIZE; }
  
  int SetMessageSaveCoords(const char* subj, size_t s_size, const char* body, size_t b_size, const LingProc *lp); //also perform word breaking + saves word border mapping to original string
  int SetMessage(const char* subj, size_t s_size, const char* body, size_t b_size, bool force_utf_canonize);

  const CoordMapping& GetMapping() const{
    return mapping;
  }

  void SetMaxTextSize(size_t new_max_size)
    { m_MaxTextSize = new_max_size; }

protected:
  size_t m_MaxTextSize;
};
//============================================================================//
#endif /*__DocMessage_H*/
