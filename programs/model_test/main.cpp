#include <fdeep/fdeep.hpp>
#include <ml/Disambiguator.h>
#include <fasttext.h>
#include <iostream>
#include <vector>
#include <string>
#include <morph/Processor.h>
#include <graphem/Tokenizer.h>
#include <utils/UniString.h>
using namespace tokenize;
using namespace analyze;
using namespace std;
using namespace utils;
using namespace base;

int main()
{
    boost::locale::generator gen;
    std::locale loc = gen("ru_RU.UTF8");
    std::locale::global(loc);

    Tokenizer tok;
    Processor analyzer;
    std::string words =
        "привет"
        " прекрасный"
        " мир"
        " привет"
        " прекрасный"
        " дивный"
        " новый"
        " интерсный"
        " настоящий"
        " текущий"
        " мир"
        " мама"
        " мыла"
        " раму"
        " раму"
        " мыла"
        " мама"
        " сегодня"
        " завтра"
        " вчера";
    //fasttext::FastText fstxt;
    //fstxt.loadModel("/home/alesap/code/cpp/XMorpheWork/scripts/morphorueval-2019.model.bin");
    //std::cerr << "FS:" << fstxt.getDimension() << std::endl;
    //std::vector<float> data;
    //size_t counter = 0;
    //for (const auto word : words)
    //{
    //    std::cerr << "counter:" << counter << " word:" << word << std::endl;
    //    ++counter;
    //    fasttext::Vector v(fstxt.getDimension());
    //    fstxt.getWordVector(v, word);
    //    //std::cerr << word << " " << v << std::endl;
    //    std::vector<float> word_data(v.data(), v.data() + v.size());
    //    word_data.resize(120);
    //    std::vector<TokenPtr> tokens = tok.analyze(UniString(word));
    //    std::vector<WordFormPtr> forms = analyzer.analyze(tokens);

    //    for (const auto & info : forms[0]->getMorphInfo()) {
    //        size_t index = UniSPTag::get(info.sp);
    //        word_data[100 + index] = 1;
    //    }
    //    data.insert(data.end(), word_data.begin(), word_data.end());
    //    std::cerr <<  "Data size:" << data.size() << std::endl;
    //}

    std::ifstream embedding_stream("/home/alesap/code/cpp/XMorpheWork/scripts/morphorueval-2019.model.bin");
    std::ifstream model_stream("/home/alesap/code/cpp/XMorpheWork/data/models/sp_model.json");
    ml::Disambiguator disamb(embedding_stream, model_stream, 20);
    std::vector<TokenPtr> tokens = tok.analyze(UniString(words));


    std::vector<WordFormPtr> forms = analyzer.analyze(tokens);
    std::vector<WordFormPtr> test_forms;
    for (size_t i = 0; i < forms.size(); ++i) {
        if (!(forms[i]->getTag() & base::GraphemTag::SPACE))
            test_forms.push_back(forms[i]);
    }

    auto result = disamb.disambiguate(test_forms);
    for (size_t i = 0; i < test_forms.size(); ++i) {
        std::cerr << "Form:" << test_forms[i]->getWordForm() << std::endl;
        std::cerr << "SP:" << result[i].sp << std::endl;
    }
    //const auto model = fdeep::load_model("/home/alesap/code/cpp/XMorpheWork/data/models/sp_model.json");
    //ml::KerasModel model(model_stream);
    //// std::cerr << "Inputshape:" << model.generate_dummy_inputs().size() << std::endl;
    //// std::cerr << "Inside input: " << fdeep::show_tensor_shape(model.generate_dummy_inputs()[0].shape()) << std::endl;
    ////auto result = model.predict({fdeep::tensor(fdeep::tensor_shape(static_cast<std::size_t>(20), static_cast<size_t>(120)), std::move(data))});
    //auto result = model.predictSingle(std::move(data));
    //std::cerr << "Result\n";
    ////std::cerr <<  fdeep::show_tensors(result) << std::endl;
    ////std::cerr <<  fdeep::show_tensor_shape(result[0].shape())<< std::endl;
    ////auto vector_begin = result[0].as_vector()->begin();
    ////auto vector_end = result[0].as_vector()->end();
    //size_t i = 0;
    //for (auto it = result.begin(); it != result.end(); it += 21)
    //{
    //    auto max_pos = std::max_element(it, it + 21);
    //    auto max_index = std::distance(result.begin(), max_pos) - 21 * i;
    //    std::cerr << "MaxIndex for word:" << words[i] << " is " << max_index <<std::endl;
    //    std::cerr << "SpeechPart:" << UniSPTag::get(max_index) << std::endl;
    //    ++i;
    //}
    return 0;
}
