#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_EXIT_H_F(Variable)
    {
        if (sym)
        {
            assert(0 < sym->get().identifier.size());
            const std::size_t lutId = sym->get().scopeId;

            if (sym->get().isConfig)
            {
                configVars.emplace_back(*sym);
            }

            auto lusym = symbolTable.find(lutId, sym->get().identifier);
            if (!lusym)
            {
                symbolTable.addEntry(lutId, sym->get().identifier, *sym);

                sym.reset();
                symnode = nullptr;
                assert(0 < symstack.size());
                symstack.pop_back();
                if (!symstack.empty())
                {
                    sym = symstack.back();
                }
            }
            else
            {
                std::cerr << "chplx : " << sym->get().identifier
                          << " identifier already defined in current scope"
                          << std::endl;
                return;
            }
        }
    }

}}}}    // namespace chpl::ast::visitors::hpx
