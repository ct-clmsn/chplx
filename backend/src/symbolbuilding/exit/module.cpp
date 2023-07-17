#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_EXIT_H_F(Module)
    {
        if (0 < symstack.size())
        {
            sym.reset();
            sym = symstack.back();
        }

        if (sym)
        {
            assert(0 < sym->get().identifier.size());
            auto lusym =
                symbolTable.find(sym->get().scopeId, sym->get().identifier);
            if (!lusym)
            {
                symbolTable.parentModuleId -= 1;
                std::shared_ptr<module_kind>& fk =
                    std::get<std::shared_ptr<module_kind>>(sym->get().kind);

                if (fk->retKind.index() < 1)
                {
                    fk->retKind = nil_kind{};
                }

                symbolTable.modules.push_back(&(sym->get()));
                symbolTable.addEntry(
                    sym->get().scopeId, fk->symbolTableSignature, *sym);
                sym.reset();
                symnode = nullptr;

                if (1 < symstack.size())
                {
                    symstack.pop_back();
                }
                if (2 < symstack.size())
                {
                    sym = symstack.back();
                }

                symbolTable.popScope();
            }
            else if (0 < sym->get().kind.index() &&
                std::holds_alternative<std::shared_ptr<module_kind>>(
                    sym->get().kind))
            {
                std::shared_ptr<module_kind>& fk =
                    std::get<std::shared_ptr<module_kind>>(sym->get().kind);

                std::cerr << "chplx : " << fk->symbolTableSignature
                          << " identifier already defined in current scope"
                          << std::endl;
                return;
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
