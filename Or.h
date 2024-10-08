#pragma once

#include "Constants.h"
#include "Symbol.h"

#include <numeric>
#include <set>

class Or : public Symbol
{
public:
    std::set<const Sentence*> sentences;

    Or(std::set<const Sentence*>&& sentences) :
        Symbol(
            [](std::set<const Sentence*>* set) -> std::string
            {
                std::string result = std::accumulate(
                    set->begin(),
                    set->end(),
                    std::string(),
                    [](std::string&& result, const Sentence* sentence)
                    {
                        if (!sentence) throw std::domain_error("Symbol contains NULL");
                        if (sentence->isSymbol()) result += "(";
                        result += sentence->getDescription();
                        if (sentence->isSymbol()) result += ")";
                        result += "&";
                        return std::move(result);
                    });
                return result.substr(0, result.size() - 1);
            }(&sentences)),
        sentences(std::move(sentences))
    {
        if (this->sentences.size() < 2)
        {
            throw std::invalid_argument("Or clause requires at least 2 sentences");
        }
    }

    ~Or()
    {
#ifdef TEST
        std::cout << "Or deleting: " << getDescription() << "\n";
#endif
    }

    bool evaluate() const override
    {
        for (const Sentence* sentence : sentences)
        {
            if (sentence->evaluate())
            {
                return true;
            }
        }

        return false;
    }

    size_t atomicCount() const override
    {
        size_t result = 0;
        for (const Sentence* sentence : sentences)
        {
            result += sentence->atomicCount();
        }

        return result;
    }

    bool operator==(const Sentence& other) const override
    {
        const Or* orSymbol = dynamic_cast<const Or*>(&other);
        return orSymbol && operator==(*orSymbol);
    }

    bool operator==(const Or& other) const { return sentences == other.sentences; }
};
