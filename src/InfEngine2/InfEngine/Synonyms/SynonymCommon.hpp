#ifndef __SynonymCommon_hpp__
#define __SynonymCommon_hpp__

struct SynonymAttrsWR {

    inline void SetBuffer(cf_term_attrs * aAttrs) {
        Attrs = aAttrs;
    }

    inline void SetValues(unsigned int aMainID, unsigned short aGroupID, unsigned short aDictId) {
        *reinterpret_cast<unsigned int*> (*Attrs) = aMainID;
        *reinterpret_cast<unsigned short*> (reinterpret_cast<char*> (*Attrs) + sizeof (unsigned int)) = aGroupID;
        *reinterpret_cast<unsigned short*> (reinterpret_cast<char*> (*Attrs) + sizeof (unsigned int) + sizeof (unsigned short)) = aDictId;
    }

    inline const cf_term_attrs * GetAttrs() const {
        return Attrs;
    }

private:
    cf_term_attrs * Attrs;
};

struct SynonymAttrsRO {

    inline void SetBuffer(const cf_term_attrs * aAttrs) {
        Attrs = aAttrs;
    }

    inline const cf_term_attrs * GetAttrs() const {
        return Attrs;
    }

    /**
     *  Возвращает индекс главного синонима.
     */
    inline unsigned int GetMainID() const {
        return *reinterpret_cast<const unsigned int*> (*Attrs);
    }

    /**
     *  Возвращает номер группы синонимов, к которой принадлежит данный синоним.
     */
    inline unsigned short GetGroupID() const {
        return *reinterpret_cast<const unsigned short*> (reinterpret_cast<const char *> (*Attrs) + sizeof (unsigned int));
    }

    /**
     *  Возвращает индекс словаря синонимов, к которому принадлежит данный синоним.
     */
    inline unsigned short GetDictID() const {
        return *reinterpret_cast<const unsigned short*> (reinterpret_cast<const char *> (*Attrs) + sizeof (unsigned int) + sizeof (unsigned short));
    }

private:
    const cf_term_attrs * Attrs;
};

#endif // __SynonymCommon_hpp__
