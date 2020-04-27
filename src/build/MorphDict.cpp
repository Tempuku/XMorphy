#include "MorphDict.h"
namespace X
{
std::vector<LexemeGroup> MorphDict::getForms(const utils::UniString & form) const
{
    const std::string & rawString = form.getRawString();
    std::vector<LexemeGroup> result;
    if (mainDict->contains(rawString))
    {
        ParaPairArray paraCandidates = mainDict->getValue(rawString);
        for (const ParaPair & elem : paraCandidates.data)
        {
            EncodedParadigm paradigm = paraMap[elem.paraNum];
            EncodedLexemeGroup encodedLg = paradigm[elem.formNum];
            utils::UniString prefix = prefixes.right.at(encodedLg.prefixId);
            MorphTagPair ts = tags.right.at(encodedLg.tagId);
            utils::UniString suffix = suffixes.right.at(encodedLg.suffixId);
            result.emplace_back(LexemeGroup{prefix, ts.sp, ts.tag, suffix});
        }
    }
    return result;
}

std::vector<MorphDictInfo> MorphDict::getClearForms(const utils::UniString & form) const
{
    const std::string & rawString = form.getRawString();
    std::vector<MorphDictInfo> result;
    if (mainDict->contains(rawString))
    {
        const ParaPairArray & paraCandidates = mainDict->getValue(rawString);
        getClearForms(paraCandidates, result);
    }
    return result;
}

void MorphDict::getClearForms(
    const ParaPairArray & paraCandidates, std::vector<MorphDictInfo> & result, const std::vector<size_t> & lengths) const
{
    result.reserve(paraCandidates.data.size());
    for (size_t i = 0; i < paraCandidates.data.size(); ++i)
    {
        const ParaPair & elem = paraCandidates.data[i];
        if (elem.paraNum >= paraMap.size()) throw std::runtime_error(
            "Incorrect paradigm number " + std::to_string(elem.paraNum) + " largest is " + std::to_string(paraMap.size() - 1));

        const EncodedParadigm & p = paraMap[elem.paraNum];
        const EncodedLexemeGroup & current = p[elem.formNum];
        const EncodedLexemeGroup & normal = p[0];
        MorphTagPair tp = tags.right.at(current.tagId);
        if (tp.sp == UniSPTag::X)
            throw std::runtime_error("Incorrect tag pair in binary dict for paradigm number " + std::to_string(elem.paraNum));
        const utils::UniString & suffix = suffixes.right.at(current.suffixId);

        if (!lengths.empty() && suffix.length() > lengths[i])
            continue;

        const utils::UniString & prefix = prefixes.right.at(current.prefixId);
        const utils::UniString & nprefix = prefixes.right.at(normal.prefixId);
        const utils::UniString & nsuffix = suffixes.right.at(normal.suffixId);
        LexemeGroup lg{prefix, tp.sp, tp.tag, suffix};
        AffixPair pair{nprefix, nsuffix};
        MorphDictInfo info{lg, pair, elem.freq};
        result.emplace_back(std::move(info));
    }
}

std::map<Paradigm, std::size_t> MorphDict::getParadigmsForForm(const utils::UniString & form) const
{
    const std::string & rawString = form.getRawString();
    std::map<Paradigm, std::size_t> result;
    if (mainDict->contains(rawString))
    {
        ParaPairArray paraCandidates = mainDict->getValue(rawString);
        getParadigmsForForm(paraCandidates, result);
    }
    return result;
}

void MorphDict::getParadigmsForForm(const ParaPairArray & paraCandidates, std::map<Paradigm, std::size_t> & result) const
{
    for (const ParaPair & elem : paraCandidates.data)
    {
        EncodedParadigm p = paraMap[elem.paraNum];
        Paradigm decoded = decodeParadigm(p);
        if (!result.count(decoded))
        {
            result[decoded] = elem.formNum;
        }
    }
}

Paradigm MorphDict::decodeParadigm(const EncodedParadigm & para) const
{
    Paradigm result(para.size());
    for (std::size_t i = 0; i < para.size(); ++i)
    {
        utils::UniString prefix = prefixes.right.at(para[i].prefixId);
        MorphTagPair tp = tags.right.at(para[i].tagId);
        utils::UniString suffix = suffixes.right.at(para[i].suffixId);
        result[i] = LexemeGroup{prefix, tp.sp, tp.tag, suffix};
    }
    return result;
}

void dropToFiles(const std::unique_ptr<MorphDict> & dict, const std::string & mainDictFilename, const std::string & affixesFileName)
{
    std::ofstream ofsMain(mainDictFilename);
    std::ofstream ofsAffix(affixesFileName);
    dropBimapToFile<utils::UniString>(ofsAffix, dict->prefixes);
    dropBimapToFile<MorphTagPair>(ofsAffix, dict->tags);
    dropBimapToFile<utils::UniString>(ofsAffix, dict->suffixes);
    saveParas(dict->paraMap, ofsMain);
    dict->mainDict->serialize(ofsMain);
}

std::unique_ptr<MorphDict> MorphDict::loadFromFiles(std::istream & mainDictIs, std::istream & affixesIs)
{
    std::string row;
    boost::bimap<utils::UniString, std::size_t> prefixes, suffixes;
    boost::bimap<MorphTagPair, std::size_t> tags;
    readBimapFromFile(affixesIs, prefixes);
    readBimapFromFile(affixesIs, tags);
    readBimapFromFile(affixesIs, suffixes);
    DictPtr mainDict = std::make_shared<dawg::Dictionary<ParaPairArray>>();
    std::vector<EncodedParadigm> paras;
    loadParas(paras, mainDictIs);
    mainDict->deserialize(mainDictIs);
    return utils::make_unique<MorphDict>(paras, mainDict, prefixes, tags, suffixes);
}
}
