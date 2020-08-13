#ifndef _LIBBUCKWALTER_COMPATIBILITY_CONSUMER_IFACE_H_
#define _LIBBUCKWALTER_COMPATIBILITY_CONSUMER_IFACE_H_
#include "utils.h"

class ICompatibilityConsumer {
public:
    virtual bool Consume(TCharBuffer first, TCharBuffer second) = 0;
    virtual ~ICompatibilityConsumer() {};
};

#endif /* _LIBBUCKWALTER_COMPATIBILITY_CONSUMER_IFACE_H_ */
