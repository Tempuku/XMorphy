#ifndef _BUILD_DEFS_H
#define _BUILD_DEFS_H
#include <tag/UniSPTag.h>
#include <tag/UniMorphTag.h>
#include <tag/PhemTag.h>
#include <DAWG/Dictionary.h>
#include <utils/UniString.h>
#include <boost/bimap.hpp>
#include <vector>
#include <memory>
#include <functional>

namespace build {
struct LexemeGroup
{
    utils::UniString prefix;
    base::UniSPTag sp;
    base::UniMorphTag tag;
    utils::UniString suffix;

    bool operator<(const LexemeGroup & o) const {
        return std::tie(prefix, sp, tag, suffix)
            < std::tie(o.prefix, o.sp, o.tag, o.suffix);
    }
};

struct EncodedLexemeGroup
{
    size_t prefixId;
    size_t tagId;
    size_t suffixId;

    bool operator<(const EncodedLexemeGroup& o) const {
        return std::tie(prefixId, tagId, suffixId) < std::tie(o.prefixId, o.tagId, o.suffixId);
    }
};

struct AffixPair {
    utils::UniString prefix;
    utils::UniString suffix;
};

using Paradigm = std::vector<LexemeGroup>;
using EncodedParadigm = std::vector<EncodedLexemeGroup>;

struct MorphTagPair
{
    base::UniSPTag sp;
    base::UniMorphTag tag;

    bool operator<(const MorphTagPair & o) const {
        return std::tie(sp, tag) < std::tie(o.sp, o.tag);
    }

};

struct ParaPair {
    std::size_t paraNum;
    std::size_t formNum;
    std::size_t freq;
    bool operator==(const ParaPair& o) const {
        return paraNum == o.paraNum && formNum == o.formNum && freq == o.freq;
    }
};

struct ParaPairArray : public dawg::ISerializable {
    std::vector<ParaPair> data;
    bool operator==(const ParaPairArray& o) const {
        return data == o.data;
    }
    bool serialize(std::ostream& os) const override {
        std::size_t sz = data.size();
        if (!os.write((const char*)(&sz), sizeof(std::size_t)))
            return false;
        if (!os.write((const char*)(&data[0]), sz * sizeof(ParaPair)))
            return false;
        return true;
    }
    bool deserialize(std::istream& is) override {
        std::size_t sz;
        if (!is.read((char*)(&sz), sizeof(std::size_t)))
            return false;
        data.resize(sz);
        if (!is.read((char*)(&data[0]), sizeof(ParaPair) * sz))
            return false;
        return true;
    }
};

struct PhemMarkup : public dawg::ISerializable {
    std::vector<uint8_t> data;

    bool operator==(const PhemMarkup& o) const {
        return data == o.data;
    }

    base::PhemTag getTag(std::size_t index) const {
        return base::PhemTag::get(index);
    }
    void append(const base::PhemTag& tag) {
        data.push_back(base::PhemTag::get(tag));
    }

    bool serialize(std::ostream& os) const override {
        std::size_t sz = data.size();
        if (!os.write((const char*)(&sz), sizeof(std::size_t)))
            return false;
        if (!os.write((const char*)(&data[0]), sz * sizeof(uint8_t)))
            return false;
        return true;
    }
    bool deserialize(std::istream& is) override {
        std::size_t sz;
        if (!is.read((char*)(&sz), sizeof(std::size_t)))
            return false;
        data.resize(sz);
        if (!is.read((char*)(&data[0]), sizeof(uint8_t) * sz))
            return false;
        return true;
    }
};

const char DISAMBIG_SEPARATOR = '\xFF';
using DictPtr = std::shared_ptr<dawg::Dictionary<ParaPairArray>>;
using DisambDictPtr = std::shared_ptr<dawg::Dictionary<std::size_t>>;
using InnerPhemDictPtr = std::shared_ptr<dawg::Dictionary<PhemMarkup>>;
using InnerCounterPhemDictPtr = std::shared_ptr<dawg::Dictionary<std::size_t>>;
using InnerCounterPhemDictPtr = std::shared_ptr<dawg::Dictionary<std::size_t>>;

using LoadFunc = std::function<void(
    std::map<std::string, ParaPairArray>&,
    const std::vector<utils::UniString>&,
    const std::vector<MorphTagPair>&)>;

using FilterFunc = std::function<void(std::map<std::string, ParaPairArray>&)>;

void saveParas(const std::vector<EncodedParadigm>& paraMap, std::ostream& os);
void loadParas(std::vector<EncodedParadigm>& paraMap, std::istream& is);
}
namespace std {
template <>
struct hash<build::ParaPair> {
public:
    size_t operator()(const build::ParaPair& s) const {
        size_t h1 = std::hash<std::size_t>()(s.paraNum);
        size_t h2 = std::hash<std::size_t>()(s.formNum);
        size_t h3 = std::hash<std::size_t>()(s.freq);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};
template <>
struct hash<build::ParaPairArray> {
public:
    size_t operator()(const build::ParaPairArray& s) const {
        size_t hashSum = 0;
        for (size_t i = 0; i < s.data.size(); ++i) {
            hashSum += std::hash<build::ParaPair>()(s.data[i]);
        }
        return hashSum;
    }
};
template <>
struct hash<build::PhemMarkup> {
public:
    size_t operator()(const build::PhemMarkup& s) const {
        size_t hashSum = 0;
        for (size_t i = 0; i < s.data.size(); ++i) {
            hashSum += std::hash<uint8_t>()(s.data[i]);
        }
        return hashSum;
    }
};
}
#endif
