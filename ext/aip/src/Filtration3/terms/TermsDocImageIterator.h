/**
 * @file   TermsDocImageIterator.h
 * @author swizard <me@swizard.info>
 * @date   Wed Aug 27 02:13:56 2008
 * 
 * @brief  A DocImageIterator wrapper that skips stop words while moving.
 * 
 * 
 */
#ifndef _TERMSDOCIMAGEITERATOR_H_
#define _TERMSDOCIMAGEITERATOR_H_

#include <_include/cc_compat.h>

#include <LingProc4/LexID.h>
#include <LingProc4/DocImage/DocImageIterator.h>
#include <LingProc4/DocText/DocText.h>

#include "FltTermsStopEx.h"

class TermsDocImageIterator
{
public:
    TermsDocImageIterator() :
        iterator( ),
        docImage( NULL ),
        stopTable( NULL ),
        compound_mode( false )
        { }

    TermsDocImageIterator( const DocImage &_docImage, const StopDictEx *_stopTable, bool _compound_mode ) :
        iterator( _docImage ),
        docImage( &_docImage ),
        stopTable( _stopTable ),
        compound_mode( _compound_mode )
    {
        Sync();
    }

    TermsDocImageIterator( const DocImage &_docImage, const StopDictEx *_stopTable, bool _compound_mode, DocImageIterator _iterator ) :
        iterator( _docImage ),
        docImage( &_docImage ),
        stopTable( _stopTable ),
        compound_mode( _compound_mode )
    {
        *this=_iterator;
    }

    TermsDocImageIterator(const TermsDocImageIterator& it) :
        iterator( it.iterator ),
        docImage( it.docImage ),
        stopTable( it.stopTable ),
        compound_mode( it.compound_mode )
        { }
    
    ~TermsDocImageIterator() { }

protected:
    void Sync(){
        //moves to the first non-stop word, if current position is non-stop word then it does nothig
        //the function is used in constructor and assignment from DocImageIterator
        if( !docImage || !stopTable )
            return;
        if ( IsEnd() || IsHead() )
            return;
        if ( !stopTable->isStop( docImage->GetLex(GetWordPos()) ) )
            return;
        operator++(); //moves to the next non_stop word
    }
public:
    TermsDocImageIterator &operator=(const TermsDocImageIterator &src)
    {
        iterator = src.iterator;
        docImage = src.docImage;
        stopTable = src.stopTable;
        compound_mode = src.compound_mode;
        return *this;
    }
    
    TermsDocImageIterator &operator=(const DocImageIterator &src)
    {
        iterator = src;
        Sync();
        return *this;
    }
    
    TermsDocImageIterator &operator++()
    {
        if( !docImage || !stopTable )
            return *this;

        for (;;)
        {
            size_t previousWordPosition = GetWordPos();
            if(compound_mode)
                ++iterator;
            else
                iterator.ForwardWord();

            if ( IsEnd() )
                break;
            if ( previousWordPosition == GetWordPos() )
                break;
            if ( !stopTable->isStop( docImage->GetLex(GetWordPos()) ) )
                break;
        }
        return *this;
    }
    
    TermsDocImageIterator &operator--()
    {
        if( !docImage || !stopTable )
            return *this;

        for (;;)
        {
            size_t previousWordPosition = GetWordPos();
            if(compound_mode)
                --iterator;
            else
                iterator.BackWord();
            
            if ( IsHead() )
                break;
            if ( previousWordPosition == GetWordPos() )
                break;
            if ( !stopTable->isStop( docImage->GetLex(GetWordPos()) ) )
                break;
        }
        return *this;
    }
    
    bool IsEnd() const
    {
        return iterator.IsEnd();
    }
    
    bool IsHead() const
    {
        return iterator.IsHead();
    }

    bool operator==(const TermsDocImageIterator &it) const
    {
        return iterator == it.iterator;
    }

    bool operator<(const TermsDocImageIterator &it) const
    {
        return iterator < it.iterator;
    }
    bool operator<(const DocImageIterator &it) const
    {
        return iterator < it;
    }

    bool operator<=(const TermsDocImageIterator &it) const
    {
        return iterator <= it.iterator;
    }

    bool operator>(const TermsDocImageIterator &it) const
    {
        return iterator > it.iterator;
    }

    bool operator>=(const TermsDocImageIterator &it) const
    {
        return iterator >= it.iterator;
    }

    bool IsMiddleCompoundPos() const { return iterator.IsMiddleCompoundPos(); }

    size_t GetWordPos()     const { return iterator.GetWordPos();     }
    size_t GetCompoundPos() const { return iterator.GetCompoundPos(); }

    LEXID GetLex()
    {
        if(!docImage || IsHead() || IsEnd())
            return LEXINVALID;
        if(compound_mode)
            return docImage->GetLex(iterator);
        else
            return docImage->GetLex(iterator.GetWordPos());
    }

    const DocTextWord *GetDocTextWord() const
    {
        if ( unlikely(docImage == 0 || IsHead() || IsEnd()) )
            return 0;
        
        const DocText *doc_text = docImage->GetDocText();
        if ( unlikely(doc_text == 0) )
            return 0;
        
        return doc_text->Word( GetWordPos() );
    }
        
    operator const DocImageIterator&() const { return iterator; }

    void SetCompoundMode(bool _compound_mode) { compound_mode = _compound_mode; }

private:
    DocImageIterator      iterator;
    const DocImage       *docImage;
    const StopDictEx     *stopTable;
    bool                  compound_mode;
};

#endif /* _TERMSDOCIMAGEITERATOR_H_ */

