#include "FltTermsBase.h"
#include "LingProc4/LingProc.h"

#ifdef _MSC_VER
    #pragma warning(disable:4097) // typedef-name '_Base' used as synonym for class-name
    #pragma warning(disable:4512) // assignment operator could not be generated
#endif

inline FltTermsErrors mapLingProcErrors( LingProcErrors status )
{
    switch ( status )
    {
    case LP_OK:                      return TERMS_OK;
    case LP_ERROR_ENOTIMPL:          return TERMS_ERROR_LP_ENOTIMPL;
    case LP_ERROR_ENOMEM:            return TERMS_ERROR_LP_ENOMEM;
    case LP_ERROR_EFAULT:            return TERMS_ERROR_LP_EFAULT;
    case LP_ERROR_EINVAL:            return TERMS_ERROR_LP_EINVAL;
    case LP_ERROR_ERANGE:            return TERMS_ERROR_LP_ERANGE;
    case LP_ERROR_INVALID_MODE:      return TERMS_ERROR_LP_INVALID_MODE;
    case LP_ERROR_INVALID_FORMAT:    return TERMS_ERROR_LP_INVALID_FORMAT;
    case LP_ERROR_FS_FAILED:         return TERMS_ERROR_LP_FS_FAILED;
    case LP_ERROR_STRICT_DATA:       return TERMS_ERROR_LP_STRICT_DATA;
    case LP_ERROR_STRICT:            return TERMS_ERROR_LP_STRICT;
    case LP_ERROR_STRICT_DATA_FS:    return TERMS_ERROR_LP_STRICT_DATA_FS;
    case LP_ERROR_TOO_MANY_LANGS:    return TERMS_ERROR_LP_TOO_MANY_LANGS;
    case LP_ERROR_UNSUPPORTED_LANG:  return TERMS_ERROR_LP_UNSUPPORTED_LANG;
    case LP_ERROR_INVALID_LEXID:     return TERMS_ERROR_LP_INVALID_LEXID;
    case LP_ERROR_UNSUPPORTED_MORPH: return TERMS_ERROR_LP_UNSUPPORTED_MORPH;
    case LP_ERROR_MORPH_FLAGS:       return TERMS_ERROR_LP_MORPH_FLAGS;
    case LP_ERROR_NOT_ENOUGH_DATA:   return TERMS_ERROR_LP_NOT_ENOUGH_DATA;
    case LP_ERROR_MORPHS_COLLISION:  return TERMS_ERROR_LP_MORPHS_COLLISION;
    case LP_ERROR_DETECTOR:          return TERMS_ERROR_LP_DETECTOR;
    default:
        return TERMS_ERROR_EFAULT;
    };
};

//============================================================================//
// Functions that deals with LingProc
//============================================================================//
FltTermsErrors FltTermsBaseWR::imageRecurseProcess( FltTermsBaseLemmatizeContext  &context,
                                                    const FltTermsBaseLinkedLexid *prevLexid,
                                                    const TermsDocImageIterator   &imageIterator,
                                                    bool                           followHierChain,
                                                    bool                           recursionRootFlag )
{
    /* Check recursion stop condition */
    if ( imageIterator.IsEnd() )
    {
        FltTermsErrors  currentWarning = TERMS_OK;
        bool            forceIndexOnly = context.forceIndexOnly;
        bool            processingMain = false;
                
        /* Examine the current path endpoint */
        if ( followHierChain )
        {
            /* Processing the hieroglyphs-only chain */
            set_term_extra( context.termHeader );
            
            /* Always add hier term to the storage */
            forceIndexOnly = false;

            /* Check if no main terms were added: skip adding if true */
            if ( !context.mainWasAdded )
                return TERMS_OK;
        }
        else
        {
            /* Processing the fuzzy chain */
            processingMain = true;    
            if ( !context.isFuzzy )
            {
                /* This is the only hieroglyph term */
                set_term_hier( context.termHeader );
                forceIndexOnly = false;
            }
        }

        FltTermsErrors result = Add( prevLexid, context.termHeader, currentWarning, forceIndexOnly );
        if ( result != TERMS_OK && result < 0 )
            return result;
        
        if ( context.warning == TERMS_OK )
                context.warning = currentWarning;
        if ( result == TERMS_OK )
        {
                if ( processingMain )
                        context.mainWasAdded = true;
        
                /* Turn on index only adding mode */
                context.forceIndexOnly = true;
        }
                
        return TERMS_OK;
    }
    
    /* Get word image and number of homos */
    size_t homoNumber = doc_image.GetHomoNumber( imageIterator );
    if (homoNumber < 1)
        return TERMS_ERROR_LP_MORPH;

    /* Verify the words image */
    if (homoNumber > 1)
    {
        /* Check first homonym */
        MorphTypes mType = m_lp->LexMorphType( doc_image.GetLex(imageIterator, 0) );
        if (mType != MORPH_TYPE_FUZZY && mType != MORPH_TYPE_STRICT && mType != MORPH_TYPE_TILDE && mType != MORPH_TYPE_TILDE_REV)
            return TERMS_ERROR_LP_UNSUPPORTED_MORPH;

        /* Check last homonym */
        if(!imageIterator.IsMiddleCompoundPos())
        {
            mType = m_lp->LexMorphType( doc_image.GetLex(imageIterator, homoNumber - 1) );
            if (mType != MORPH_TYPE_HIER)
                return TERMS_ERROR_LP_UNSUPPORTED_MORPH;
        }
    }
    
    unsigned short currentMorph = LexNoMrph( doc_image.GetLex(imageIterator, 0) );
    size_t         i;
    
    for (i = 0; i < homoNumber; i++)
    {
        LEXID cur_lex = doc_image.GetLex(imageIterator, i);
        if ( cur_lex == LEXNONE )
            return TERMS_ERROR_LP_MORPH;
        else if ( cur_lex == LEXINVALID )
            return TERMS_ERROR_LP_FAILED;

        if ( i > 0 && (i != homoNumber - 1  || imageIterator.IsMiddleCompoundPos()))
        {
            // not first and not last homonym
            if ( LexNoMrph( cur_lex ) != currentMorph )
                return TERMS_ERROR_LP_UNSUPPORTED_MORPH;
        }
    }
    
    FltTermsBaseLinkedLexid lexNode;
    lexNode.prev = prevLexid;
    
    /* Check the hieroglyphs-only chain */
    TermsDocImageIterator new_it( imageIterator );
    ++new_it;
    if ( followHierChain )
    {
        if ( imageIterator.IsMiddleCompoundPos() )
            return imageRecurseProcess( context, prevLexid, new_it, true, false );
        
        lexNode.lex = doc_image.GetLex(imageIterator, homoNumber - 1);
        return imageRecurseProcess( context, &lexNode, new_it, true, false );
    }
    
    /* Scan lexids homonyms on the current position */
    FltTermsErrors result = TERMS_OK;
    for ( i = 0; i < homoNumber; i++ )
    {
        lexNode.lex = doc_image.GetLex(imageIterator, i);

        if ( i == homoNumber - 1 && !imageIterator.IsMiddleCompoundPos() )
        {
            /* Current processing hieroglyph homonym */
            if ( homoNumber == 1 )
            {
                /* Start normal permutation with only one hieroglyph as homonym */
                result = imageRecurseProcess( context, &lexNode, new_it, false, false );
            }
            
            if ( recursionRootFlag && context.isFuzzy )
            {
                /* Start additional hieroglyphs chain */
                result = imageRecurseProcess( context, &lexNode, new_it, true, false );
            }
        }
        else
        {
            /* Start normal permutation */
            context.isFuzzy = true;
            result = imageRecurseProcess( context, &lexNode, new_it, false, false );
        }
        if ( result != TERMS_OK )
            return result;
    }
    
    return TERMS_OK;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseWR::LemmatizeTerm( TERM_HDR *thdr, const char *txt )
{
    // dict morph attrib processing
    // not carried from old code
    // see patdata.cpp line 208 and 216

    const cf_term_attrs* attrs = term_attrs(thdr);
    
    // add text to doc_text
    LingProcErrors lp_res = doc_text.AssignString( txt, static_cast<size_t>(-1), CPG_UTF8, true );
    if ( lp_res != LP_OK )
        return mapLingProcErrors( lp_res );
        
    // setting langs
    if ( !term_is_morph_none(attrs) )
    {
        switch ( term_lang(attrs) )
        {
        case LNG_VIETNAMESE:
            doc_text.PresetLangs(LNG_VIETNAMESE, LNG_ENGLISH, false);
            break;
                
        case LNG_RUSSIAN:
            doc_text.PresetLangs(LNG_RUSSIAN, LNG_ENGLISH, false);
            break;

        case LNG_ARABIC:
            doc_text.PresetLangs(LNG_ARABIC, LNG_ENGLISH, false);
            break;

        case LNG_ENGLISH:
        case LNG_GERMAN:
        case LNG_FRENCH:
        case LNG_SPANISH:
            doc_text.PresetLangs(term_lang(attrs), LNG_UNKNOWN, false);
            break;
            
        default:
            doc_text.PresetLangs(LNG_UNKNOWN, LNG_UNKNOWN, false);
        }
    }
    else
    {
        doc_text.PresetLangs(LNG_UNKNOWN, LNG_UNKNOWN, false);
    }
    
    // set lp flags
    uint64_t set_lp_flags = 0;
    set_lp_flags |= LingProc::fUseDetectedLangs;
    set_lp_flags |= LingProc::fTillFirst;
    set_lp_flags |= LingProc::fForceHierHomo;
    uint64_t old_lp_flags = m_lp->AddFlags(set_lp_flags);

    uint64_t clear_lp_flags = 0;
    clear_lp_flags |= LingProc::fDetectLang;
    m_lp->ClearFlags(clear_lp_flags);

    // lemmatize
    lp_res = m_lp->LemmatizeDoc(doc_text, doc_image, MORPH_FLAG_PROCESS_TILDE);
    m_lp->SetFlags(old_lp_flags);
    if ( lp_res != LP_OK )
        return mapLingProcErrors( lp_res );
    /* Check if lemmatization returns zero size image */
    if ( doc_image.Size() == 0 )
        return TERMS_WARN_EMPTY;
    
    /* Process */
    FltTermsBaseLemmatizeContext context;
    context.termHeader       = thdr;
    context.isFuzzy          = false;
    context.forceIndexOnly   = false;
    context.mainWasAdded     = false;
    context.warning          = TERMS_OK;
    
    TermsDocImageIterator it(doc_image, &m_table_stop, true);
    
    /* Check if term is constructed from stop words only */
    if ( it.IsEnd() )
        return TERMS_WARN_EMPTY;
    
    FltTermsErrors trm_res = imageRecurseProcess( context, 0, it, false, true );
    if ( trm_res != TERMS_OK )
        return trm_res;

    if ( context.warning != TERMS_OK )
        return context.warning;
    
    return TERMS_OK;
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseRO::GetTermText(char** buf, size_t* bufsize, const TERM* term) const
{
    return GetTermText(buf, bufsize, term, GT_AUTO);
}
//----------------------------------------------------------------------------//
enum FltTermsErrors FltTermsBaseRO::GetTermText( char             **buf,
                                                 size_t            *bufsize,
                                                 const TERM        *term,
                                                 enum GetTextMode   mode ) const
{
    (*buf)[0] = '\0';
    
    if ( (mode == GT_USE_TEXTS) ||
         (mode == GT_AUTO && (m_hdr.data.base_flags & TERMS_BASE_FLAGS_WITH_TEXT)) )
    {
        /* Restore the term text from the database */
        
        if ( !(m_hdr.data.base_flags & TERMS_BASE_FLAGS_WITH_TEXT) )
            return TERMS_ERROR_EINVAL;

        size_t idx = term_id(term) - 1;
        if ( idx >= (m_texts_index_size / sizeof(uint32_t)) - 1 )
            return TERMS_ERROR_EFAULT;
        
        uint32_t offset = *( reinterpret_cast<const uint32_t *>(m_texts_index) + idx );
        uint32_t length = *( reinterpret_cast<const uint32_t *>(m_texts_index) + idx + 1 ) - offset;

        if ( static_cast<size_t>(offset + length) > m_texts_data_size )
            return TERMS_ERROR_EFAULT;

        if ( length + 1 > *bufsize )
        {
            void* tmp = realloc(*buf, length + 1);
            if (!tmp)
                return TERMS_ERROR_ENOMEM;
            *buf = (char*)tmp;
            *bufsize = length + 1;
        }
        
        memcpy( *buf, m_texts_data + offset, length );
        (*buf)[length] = '\0';

        return TERMS_OK;
    }
    
    size_t length = GetTermLength(term);
    const LEXID* lex = GetTermLex(term);

    BufferReceiver< 4096 > textReceiver;
    const char *lex_text_pt;
    size_t      lex_text_size;
    
    bool lastPartWasCompoundOne = false;
    for (size_t i = 0; i < length; i++)
    {
        LingProcErrors status = m_lp->LexText( lex[i], textReceiver );
        if ( unlikely(status != LP_OK) )
            return mapLingProcErrors( status );

        lex_text_pt   = textReceiver.getBuffer();
        lex_text_size = textReceiver.size();
        
        /* Let's check if it is a compound part */        
        bool     isCompoundPart = false;
        size_t   offset         = lex_text_size;
        uint32_t ch;

        U8_PREV_UNSAFE( lex_text_pt, offset, ch );
        if ( ch == static_cast<uint32_t>( 0x2021 ) ) // #\DOUBLE_DAGGER
        {
            isCompoundPart = true;
            
            /* Cut off the dagger on the end */
            lex_text_size = offset;
        }
        
        size_t len_needed = strlen(*buf) + lex_text_size + 2;
        if( len_needed > *bufsize )
        {
            void* tmp = realloc(*buf, len_needed);
            if(!tmp)
                return TERMS_ERROR_ENOMEM;
            *buf = (char*)tmp;
            *bufsize = len_needed;
        }

        if (i > 0 && !lastPartWasCompoundOne)
            strcat(*buf, " ");
        strncat(*buf, lex_text_pt, lex_text_size);
        
        lastPartWasCompoundOne = isCompoundPart;
    }
    return TERMS_OK;
}
//============================================================================//
static enum FltTermsErrors fillMarkupInfo(const LPMarkup &markup, struct terms_di_info* info)
{
  // getting markup
  size_t subj_markup_idx = static_cast<size_t>(-1);
  size_t body_markup_idx = static_cast<size_t>(-1);

  size_t m;

  for ( m = 0; m < markup.size(); m++ )
  {
    if ( markup[(unsigned int)m].m_Type == MARKUP_SUBJECT )
    {
      if ( subj_markup_idx != static_cast<size_t>(-1) )
        return TERMS_ERROR_EFAULT;
      subj_markup_idx = m;
    }
    else if ( markup[(unsigned int)m].m_Type == MARKUP_BODY )
    {
      if ( body_markup_idx != static_cast<size_t>(-1) )
        return TERMS_ERROR_EFAULT;
      body_markup_idx = m;
    }
  }

  // verifying markup
  if ( subj_markup_idx != static_cast<size_t>(-1) && markup[ subj_markup_idx ].m_nBeg )
    return TERMS_ERROR_EFAULT;
  if ( subj_markup_idx != static_cast<size_t>(-1) &&
       body_markup_idx != static_cast<size_t>(-1) &&
       markup[ subj_markup_idx ].m_nBeg + markup[ subj_markup_idx ].m_nLen != markup[ body_markup_idx ].m_nBeg )
    return TERMS_ERROR_EFAULT;

  if ( subj_markup_idx != static_cast<size_t>(-1) )
  {
    info->subj_from = markup[ subj_markup_idx ].m_nBeg;
    info->subj_to = markup[ subj_markup_idx ].m_nBeg + markup[ subj_markup_idx ].m_nLen;
    if ( info->subj_to > info->size ) // additional check - markup can't be trusted
        info->subj_to = info->size;
  }
  else
  {
    info->subj_from = 0;
    info->subj_to   = 0;
  }

  if ( body_markup_idx != static_cast<size_t>(-1) )
  {
    info->body_from = markup[ body_markup_idx ].m_nBeg;
    info->body_to = markup[ body_markup_idx ].m_nBeg + markup[ body_markup_idx ].m_nLen;
    if ( info->body_to > info->size ) // additional check - markup can't be trusted
        info->body_to = info->size;
  }
  else
  {
    info->body_from = 0;
    info->body_to   = 0;
  }

  return TERMS_OK;
}

//============================================================================//
enum FltTermsErrors FltTermsBaseRO::FillDocImageInfo(const LingProc* lp,
                      const DocImage* di, const CDocMessage::CoordMapping* mapping, struct terms_di_info* info, bool bNoMarkup) const
{
  // memset(info, 0, sizeof(struct terms_di_info));

  info->di = di;
  info->size = di->Size();
  info->length = 0; // will be culculated during lookup 
  
  // setting LangPrim and LangSupp
  // (!) must be coordinated with LingProc::LemmatizeDoc
  info->lang_prim = di->LangPrim();
  info->lang_supp = di->LangSupp();

  /* Set the compound support flags */
  info->prim_compound_support = lp->IsCompoundModeOn( info->lang_prim );
  info->supp_compound_support = lp->IsCompoundModeOn( info->lang_supp );
  //setting coordinates mapping
  assert( mapping==NULL || mapping->size()==di->Size() );
  info->mapping=mapping;

  if((info->lang_prim == LNG_RUSSIAN) && (info->lang_supp == LNG_ENGLISH))
  {
    info->prim_by_supp = true;
    info->supp_by_prim = false;
  }
  else if((info->lang_prim == LNG_ENGLISH) && (info->lang_supp == LNG_RUSSIAN))
  {
    info->supp_by_prim = true;
    info->prim_by_supp = false;
  }
  else
  {
    info->prim_by_supp = false;
    info->supp_by_prim = false;
  }

  // setting morph numbers (shifted)
  info->morph_prim = info->morph_supp = 0;
  info->morph_prim_strict = info->morph_supp_strict = 0;
  if(info->lang_prim)
  {
      unsigned int morph_no = 0;
      LingProcErrors status = lp->MorphNo(info->lang_prim, MORPH_TYPE_FUZZY, morph_no);
      if ( unlikely(status != LP_OK) )
          return mapLingProcErrors( status );
      if(morph_no == MORPHNO_CTRL)
      {
          status = lp->MorphNo(info->lang_prim, MORPH_TYPE_TILDE, morph_no);
          if ( unlikely(status != LP_OK) )
              return mapLingProcErrors( status );
      }
      info->morph_prim = static_cast<LEXID>(morph_no) << BitsMaskPower< ~LEX_MORPH >::n;
      
      status = lp->MorphNo(info->lang_prim, MORPH_TYPE_STRICT, morph_no);
      if ( unlikely(status != LP_OK) )
          return mapLingProcErrors( status );
      if(morph_no == MORPHNO_CTRL)
      {
          status = lp->MorphNo(info->lang_prim, MORPH_TYPE_TILDE_REV, morph_no);
          if ( unlikely(status != LP_OK) )
              return mapLingProcErrors( status );
      }
      info->morph_prim_strict = static_cast<LEXID>(morph_no) << BitsMaskPower< ~LEX_MORPH >::n;
  }
  if(info->lang_supp)
  {
      unsigned int morph_no = 0;
      LingProcErrors status = lp->MorphNo(info->lang_supp, MORPH_TYPE_FUZZY, morph_no);
      if ( unlikely(status != LP_OK) )
          return mapLingProcErrors( status );
      if(morph_no == MORPHNO_CTRL)
      {
          status = lp->MorphNo(info->lang_supp, MORPH_TYPE_TILDE, morph_no);
          if ( unlikely(status != LP_OK) )
              return mapLingProcErrors( status );
      }
      info->morph_supp = static_cast<LEXID>(morph_no) << BitsMaskPower< ~LEX_MORPH >::n;
      
      status = lp->MorphNo(info->lang_supp, MORPH_TYPE_STRICT, morph_no);
      if ( unlikely(status != LP_OK) )
          return mapLingProcErrors( status );
      if(morph_no == MORPHNO_CTRL)
      {
          status = lp->MorphNo(info->lang_supp, MORPH_TYPE_TILDE_REV, morph_no);
          if ( unlikely(status != LP_OK) )
              return mapLingProcErrors( status );
      }
      info->morph_supp_strict = static_cast<LEXID>(morph_no) << BitsMaskPower< ~LEX_MORPH >::n;
  }

  if(!bNoMarkup)
    return fillMarkupInfo(di->GetTextMarkup(), info);

  info->subj_from = 0;
  info->subj_to   = 0;
  info->body_from = 0;
  info->body_to   = info->size;

  return TERMS_OK;
}
//----------------------------------------------------------------------------//
int FltTermsBaseRO::Lookup_GetHomo( const struct terms_di_info      *di_info,
                                    const        DocImageIterator   &it,
                                    const        LEXID             *&primArray,
                                    size_t                          &primSize,
                                    const LEXID                    *&suppArray,
                                    size_t                          &suppSize,
                                    LEXID                           &hier) const
{
    primArray = suppArray = 0;
    primSize  = suppSize  = 0;
    hier      = LEXNONE;
    
    size_t       homo_num;
    const LEXID *word_image;
    bool         hold_prim = false;
    bool         hold_supp = false;
    
    /* Process the compound part of a word image */
    homo_num   = di_info->di->GetCompoundHomoNumber(it);
    word_image = ( homo_num > 0 ? di_info->di->GetLexPtr(it, 0) : 0 );

    size_t homo;
    for (homo = 0; homo < homo_num; homo++)
    {
            LEXID lex   = word_image[homo];
            LEXID morph = lex & LEX_MORPH;

            if ( di_info->prim_compound_support &&
                 morph != 0                     &&
                 morph == di_info->morph_prim_strict )
            {
                    if ( primSize == 0 )
                            primArray = word_image + homo;
                    primSize++;
                    hold_prim = true;
            }
            else if ( di_info->supp_compound_support &&
                      morph != 0                     &&
                      morph == di_info->morph_supp_strict )
            {
                    if ( suppSize == 0 )
                            suppArray = word_image + homo;
                    suppSize++;
                    hold_supp = true;
            }
    }
    
    /* A position in a middle of a compound -- can stop collecting */
    if ( it.IsMiddleCompoundPos() )
            return 0;
    
    /* Process the standard part of a word image */
    homo_num   = di_info->di->GetHomoNumber(it.GetWordPos());
    word_image = ( homo_num > 0 ? di_info->di->GetWordImage(it.GetWordPos()) : 0 );
    
    for (homo = 0; homo < homo_num; homo++)
    {
        LEXID lex   = word_image[homo];
        LEXID morph = lex & LEX_MORPH;
        
        if ( !hold_prim && morph != 0 && (morph == di_info->morph_prim || morph == di_info->morph_prim_strict))
        {
            if ( primSize == 0 )
                primArray = word_image + homo;
            primSize++;
        }
        else if ( !hold_supp && morph != 0 && (morph == di_info->morph_supp || morph == di_info->morph_supp_strict))
        {
            if ( suppSize == 0 )
                suppArray = word_image + homo;
            suppSize++;
        }
        else if ( LexIsHier(lex) || LexIsPunct(lex) || LexIsNum(lex) )
        {
            if (hier == LEXNONE)
                hier = LexNormalize(lex);
        }
    }

    // adjusting Russian
    if ( primSize == 0 && di_info->prim_by_supp )
    {
        primArray = suppArray;
        primSize  = suppSize;
    }
    if ( suppSize == 0 && di_info->supp_by_prim )
    {
        suppArray = primArray;
        suppSize  = primSize;
    }

    return 0;
}
//============================================================================//
