#include "transformation.h"
#include "grammar_file_parser.h"
#include "grammar_consumer_iface.h"
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>

const size_t        TGrammarFileDispatcher::KNoError    =   std::numeric_limits<size_t>::max();

static const char   kLexemeIdStart[]                    =   ";; ";
static const char   kCommentStart[]                     =   ";";

TGrammarFileDispatcher::EStringType TGrammarFileDispatcher::WhatType(TConstString str)
{
    if (str.IsEmpty()) {
        return EEmpty;
    }

    if ( (str.Size() >= strlen(kLexemeIdStart))
      && (0 == strncmp(kLexemeIdStart, str.Begin, strlen(kLexemeIdStart)))) {
        return ELexemeId;
    }

    if ( (str.Size() >= strlen(kCommentStart))
      && (0 == strncmp(kCommentStart, str.Begin, strlen(kCommentStart)))) {
        return EComment;
    }
    return EGrammar;
}

size_t TGrammarFileDispatcher::Parse(const char* filePath)
{
    size_t          strNumber   =   0;
    std::ifstream   file(filePath);
    if (!file.is_open()) {
        return 0;
    }
    std::string     str;
    bool            isError     =   false;
    TConstString    buffer;
    while ((!isError) && (std::getline(file, str))) {
        strNumber           +=  1;
        buffer              =   Strip(TConstString(str.c_str(), str.c_str() + str.size()));

        EStringType type    =   WhatType(buffer);

        switch (type) {
            case EEmpty:
                isError |=  !Dispatchable.EmptyString(buffer);
                break;
            case EComment:
                isError |=  !Dispatchable.CommentString(buffer);
                break;
            case ELexemeId:
                isError |=  !Dispatchable.LexemeIdString(buffer);
                break;
            case EGrammar:
                isError |=  !Dispatchable.GrammarString(buffer);
                break;
            case EUnknown:
                isError =   true;
                break;
        }
    }
    if (!isError) {
        strNumber   =   KNoError;
    }
    return strNumber;
}

bool TGrammarFileParser::EmptyString(TConstString /*str*/)
{
    //ignore empty lines
    return true;
}

bool TGrammarFileParser::CommentString(TConstString /*str*/)
{
    //ignore comment lines
    return true;
}

bool TGrammarFileParser::LexemeIdString(TConstString str)
{
    TConstString    lexemeId(str.Begin + strlen(kLexemeIdStart), str.End);
    lexemeId    =   Strip(lexemeId);
    CurrentGrammar.Clear();
    CurrentGrammar.Fields[LexemeIdNum].assign( lexemeId.Begin
                                             , lexemeId.Size() );
    return true;
}

bool TGrammarFileParser::GrammarString(TConstString str)
{
    bool    isOk    =   true;
    isOk    &=  ParseGrammarString(str, CurrentGrammar);

    avector<char>   transformBuffer;

    const std::string&  woVowels(CurrentGrammar.Fields[EntryWoVowelsNum]);
    buckwalterFromLatinToArabic(woVowels.c_str(), woVowels.size(), transformBuffer);
    CurrentGrammar.Fields[EntryWoVowelsNum].assign(transformBuffer.get_buffer(), transformBuffer.size());

    const std::string&  wthVowels(CurrentGrammar.Fields[EntryWthVowelsNum]);
    buckwalterFromLatinToArabic(wthVowels.c_str(), wthVowels.size(), transformBuffer);
    CurrentGrammar.Fields[EntryWthVowelsNum].assign(transformBuffer.get_buffer(), transformBuffer.size());

    std::string&    interpretation(CurrentGrammar.Fields[InterpretationNum]);
    TConstString    stripped(Strip(TConstString(interpretation.c_str(), interpretation.size())));
    interpretation.assign(stripped.Begin, stripped.Size());

    isOk    &=  DeducePartOfSpeech(CurrentGrammar);
    isOk    &=  Consumer.Consume(CurrentGrammar);

    return isOk;
}

static const char       kPosStartTag[]  =   "<pos>";
static const char       kPosStopTag[]   =   "</pos>";
bool ParseGrammarString(TConstString str, TPartGrammar& grammar)
{
    TSplits                 fields;
    Split(str, '\t', fields);
    if (fields.size() < 4) {
        return false;
    }

    grammar.Fields[EntryWoVowelsNum].assign(fields[0].Begin, fields[0].Size());
    grammar.Fields[EntryWthVowelsNum].assign(fields[1].Begin, fields[1].Size());
    grammar.Fields[MorphoCategoryNum].assign(fields[2].Begin, fields[2].Size());

    TConstString    interpretationNPos(fields[3]);
    //sometimes info about POS is in the same column as interpretation
    const char* startPos    =   std::search( interpretationNPos.Begin
                                           , interpretationNPos.End
                                           , kPosStartTag
                                           , kPosStartTag + strlen(kPosStartTag) );

    const char* endPos      =   std::search( interpretationNPos.Begin
                                           , interpretationNPos.End
                                           , kPosStopTag
                                           , kPosStopTag + strlen(kPosStopTag) );

    if ((endPos == interpretationNPos.End) && (startPos == interpretationNPos.End)) {
        grammar.Fields[InterpretationNum].assign( interpretationNPos.Begin
                                                , interpretationNPos.Size() );
        grammar.Fields[PartOfSpeechNum].assign("");
    }
    else if ((endPos == interpretationNPos.End) && (startPos != interpretationNPos.End)) {
        //have start wo end
        return false;
    }
    else if ((endPos != interpretationNPos.End) && (startPos == interpretationNPos.End)) {
        //have end wo start
        return false;
    }
    else if ((endPos != interpretationNPos.End) && (startPos != interpretationNPos.End)) {
        TConstString    pos(startPos + strlen(kPosStartTag), endPos);
        grammar.Fields[PartOfSpeechNum].assign(pos.Begin, pos.Size());
        TConstString    interpretation(interpretationNPos.Begin, startPos);
        grammar.Fields[InterpretationNum].assign(interpretation.Begin, interpretation.Size());
    }
    return true;
}

void MaterializeGrammarString(const TPartGrammar& grammar, std::string& str)
{
    std::string         interpretationNPos(grammar.Fields[InterpretationNum]);
    const std::string&  pos(grammar.Fields[PartOfSpeechNum]);
    if (!pos.empty()) {
        interpretationNPos  +=  kPosStartTag;
        interpretationNPos  +=  pos;
        interpretationNPos  +=  kPosStopTag;
    }

    std::vector<const std::string*>   parts(4);
    parts[0]    =   &grammar.Fields[EntryWoVowelsNum];
    parts[1]    =   &grammar.Fields[EntryWthVowelsNum];
    parts[2]    =   &grammar.Fields[MorphoCategoryNum];
    parts[3]    =   &interpretationNPos;

    Join(parts.begin(), parts.end(), '\t', str);
}

bool TGrammarFileParser::DeducePartOfSpeech(TPartGrammar& grammar)
{
    std::string&        partOfSpeech(grammar.Fields[PartOfSpeechNum]);
    if (!partOfSpeech.empty()) {
        return true;
    }
    //this function copied from perl code, so comments copied too
    const std::string&  morphology(grammar.Fields[MorphoCategoryNum]);
    const std::string&  interpretation(grammar.Fields[InterpretationNum]);
    //const std::string&  vocalized(grammar.Fields[EntryWthVowelsNum]);

    static const char   kPrefixMorphology[]         =   "Pref-0";
    static const char   kSuffixMorphology[]         =   "Suff-0";
    static const char   kVerbImperfectMorphology[]  =   "IV";
    static const char   kVerbPerfectMorphology[]    =   "PV";
    static const char   kVerbImperativeMorphology[] =   "CV";

    if ( ( (morphology.size() == strlen(kPrefixMorphology))
        && (0 == strncmp(kPrefixMorphology, morphology.c_str(), strlen(kPrefixMorphology))) )
      || ( (morphology.size() == strlen(kSuffixMorphology))
        && (0 == strncmp(kSuffixMorphology, morphology.c_str(), strlen(kSuffixMorphology))) ) ) {
        //# null prefix or suffix
        partOfSpeech.assign("");
    }
    else if ((morphology.size() >= 1) && (morphology[0] == 'F')) {
        partOfSpeech.assign("FUNC_WORD");
    }
    else if ( (morphology.size() >= strlen(kVerbImperfectMorphology))
           && (0 == strncmp(kVerbImperfectMorphology, morphology.c_str(), strlen(kVerbImperfectMorphology))) ) {
        partOfSpeech.assign("VERB_IMPERFECT");
    }
    else if ( (morphology.size() >= strlen(kVerbPerfectMorphology))
           && (0 == strncmp(kVerbPerfectMorphology, morphology.c_str(), strlen(kVerbPerfectMorphology))) ) {
        partOfSpeech.assign("VERB_PERFECT");
    }
    else if ( (morphology.size() >= strlen(kVerbImperativeMorphology))
           && (0 == strncmp(kVerbImperativeMorphology, morphology.c_str(), strlen(kVerbImperativeMorphology))) ) {
        partOfSpeech.assign("VERB_IMPERATIVE");
    }
    else if ( ((morphology.size() >= 1) && (morphology[0] == 'N'))
           && ((interpretation.size() >= 1) && ('Z' >= interpretation[0]) && (interpretation[0] >= 'A')) ) {
        //# educated guess (99% correct)
        partOfSpeech.assign("NOUN_PROP");
    }
    else if ((morphology.size() >= 1) && (morphology[0] == 'N')) {
        partOfSpeech.assign("NOUN");
    }
    else {
        return false;
    }
    //end of copy paste
    return true;
}
