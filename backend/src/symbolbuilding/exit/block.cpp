#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_EXIT_H_F(Block)
    {
        if (sym)
        {
            assert(0 < sym->get().identifier.size());
            auto lusym =
                symbolTable.find(sym->get().scopeId, sym->get().identifier);
            const bool is_func =
                std::holds_alternative<std::shared_ptr<func_kind>>(
                    sym->get().kind);
            if (!lusym && is_func)
            {
                std::shared_ptr<func_kind>& fk =
                    std::get<std::shared_ptr<func_kind>>(sym->get().kind);

                if (fk->retKind.index() < 1)
                {
                    fk->retKind = nil_kind{};
                }

                symbolTable.addEntry(
                    sym->get().scopeId, fk->symbolTableSignature, *sym);

                sym.reset();
                symnode = nullptr;
                if (2 < symstack.size())
                {
                    symstack.pop_back();
                    sym = symstack.back();
                }
                else if (1 < symstack.size())
                {
                    symstack.pop_back();
                }

                symbolTable.popScope();
            }
            else if (is_func && 0 < sym->get().kind.index())
            {
                std::shared_ptr<func_kind>& fk =
                    std::get<std::shared_ptr<func_kind>>(sym->get().kind);

                std::cerr << "chplx : " << fk->symbolTableSignature
                          << " identifier already defined in current scope"
                          << std::endl;
                return;
            }
        }
    }

}}}}    // namespace chpl::ast::visitors::hpx
