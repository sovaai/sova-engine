#ifndef _LIBBUCKWALTER_GRAMMAR_FILE_PARSER_IFACE_H_
#define _LIBBUCKWALTER_GRAMMAR_FILE_PARSER_IFACE_H_
#include "grammar.h"
#include "utils.h"
#include <lib/aptl/avector.h>
#include <limits>

class IGrammarConsumer;

class IGrammarFileDispatchable {
public:
    ~IGrammarFileDispatchable() {};
    //return false on error
    virtual bool EmptyString(TConstString str)    = 0;
    virtual bool CommentString(TConstString str)  = 0;
    virtual bool LexemeIdString(TConstString str) = 0;
    virtual bool GrammarString(TConstString str)  = 0;
};

class TGrammarFileDispatcher {
public:
    TGrammarFileDispatcher(IGrammarFileDispatchable& dispatchable)
    : Dispatchable(dispatchable)
    {}
    //return string number in which there is an error
    //0 if file not exists
    //KNoError is all Ok
    static const size_t KNoError;
    size_t Parse(const char* filePath);
private:
    enum EStringType {
        EEmpty = 0,
        EComment,
        ELexemeId,
        EGrammar,
        EUnknown
    };

    static EStringType WhatType(TConstString str);
    IGrammarFileDispatchable&   Dispatchable;
};

bool ParseGrammarString(TConstString str, TPartGrammar& grammar);
void MaterializeGrammarString(const TPartGrammar& grammar, std::string& str);
//parse buckwalter grammar file and feed output to Consumer
class TGrammarFileParser : public IGrammarFileDispatchable {
public:
    TGrammarFileParser(IGrammarConsumer& consumer)
    : Consumer(consumer)
    {}

    bool EmptyString(TConstString str);
    bool CommentString(TConstString str);
    bool LexemeIdString(TConstString str);
    bool GrammarString(TConstString str);

private:
    static bool DeducePartOfSpeech(TPartGrammar& grammar);

    IGrammarConsumer&   Consumer;
    TPartGrammar        CurrentGrammar;
};

#endif /* _LIBBUCKWALTER_GRAMMAR_FILE_PARSER_IFACE_H_ */
