#ifndef _LIBBUCKWALTER_GRAMMAR_CONSUMER_IFACE_H_
#define _LIBBUCKWALTER_GRAMMAR_CONSUMER_IFACE_H_
#include "grammar.h"

class IGrammarConsumer {
public:
    virtual bool Consume(const TPartGrammar& grammar) = 0;
    virtual ~IGrammarConsumer() {};
};

#endif /* _LIBBUCKWALTER_GRAMMAR_CONSUMER_IFACE_H_ */
