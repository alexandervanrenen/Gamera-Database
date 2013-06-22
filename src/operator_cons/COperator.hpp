#pragma once

#include "harriet/ScriptLanguage.hpp"
#include <vector>

namespace dbi {

    class COperator {
    public:

        COperator(std::vector<harriet::Value>& registerz) : registerz(registerz) {
        }

        virtual void open() = 0;

        virtual void close() = 0;

        virtual bool next() = 0;

    protected:
        std::vector<harriet::Value>& registerz;

    };

}

