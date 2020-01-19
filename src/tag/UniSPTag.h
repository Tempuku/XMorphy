#ifndef _UNI_SP_TAG_H
#define _UNI_SP_TAG_H
#include "ITag.h"
namespace base {
struct UniSPTag : public ITag {
private:
    UniSPTag(uint64_t val);
    static const std::vector<UniSPTag> inner_runner;

public:
    static const UniSPTag X;
    static const UniSPTag ADJ;
    static const UniSPTag ADV;
    static const UniSPTag INTJ;
    static const UniSPTag NOUN;
    static const UniSPTag PROPN;
    static const UniSPTag VERB;
    static const UniSPTag ADP;
    static const UniSPTag AUX;
    static const UniSPTag CONJ;
    static const UniSPTag SCONJ;
    static const UniSPTag DET;
    static const UniSPTag NUM;
    static const UniSPTag PART;
    static const UniSPTag PRON;
    static const UniSPTag PUNCT;
    static const UniSPTag H;
    static const UniSPTag R;
    static const UniSPTag Q;
    static const UniSPTag SYM;

    static std::vector<UniSPTag>::const_iterator begin() {
        return inner_runner.begin();
    }

    static std::vector<UniSPTag>::const_iterator end() {
        return inner_runner.end();
    }

    static constexpr std::size_t size() {
        return 20;
    }

    static UniSPTag get(std::size_t index) {
        return inner_runner[index];
    }

    static std::size_t get(UniSPTag t) {
        return std::find(inner_runner.begin(), inner_runner.end(), t) - inner_runner.begin();
    }

    UniSPTag(const std::string& val);
    UniSPTag();

    bool operator==(const UniSPTag& other) const {
        return value == other.value && name_map == other.name_map;
    }
    bool operator!=(const UniSPTag& other) const {
        return !this->operator==(other);
    }
    bool operator<(const UniSPTag& other) const {
        return value < other.value;
    }
    bool operator>(const UniSPTag& other) const {
        return value > other.value;
    }

    virtual ~UniSPTag() {}
};

static std::set<UniSPTag> FIXED_UNISPS = {UniSPTag::ADV, UniSPTag::INTJ, UniSPTag::ADP};
}

#endif
