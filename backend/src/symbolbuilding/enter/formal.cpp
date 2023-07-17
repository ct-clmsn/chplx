#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_ENTER_H_F(Formal)
    {
        if (sym.has_value() && 0 < sym->get().kind.index() &&
            std::holds_alternative<std::shared_ptr<func_kind>>(sym->get().kind))
        {
            std::shared_ptr<func_kind>& fk =
                std::get<std::shared_ptr<func_kind>>(sym->get().kind);

            int kindqual = -1;

            Formal::Intent k = dynamic_cast<Formal const*>(ast)->intent();
            if (k == Formal::Intent::DEFAULT_INTENT)
            {
                kindqual = 0;
            }
            else if (k == Formal::Intent::CONST)
            {
                kindqual = 1;
            }
            else if (k == Formal::Intent::CONST_REF)
            {
                kindqual = 2;
            }
            else if (k == Formal::Intent::REF)
            {
                kindqual = 3;
            }
            else if (k == Formal::Intent::PARAM)
            {
                kindqual = 4;
            }
            else if (k == Formal::Intent::TYPE)
            {
                kindqual = 5;
            }
            else if (k == Formal::Intent::IN)
            {
                kindqual = 6;
            }
            else if (k == Formal::Intent::CONST_IN)
            {
                kindqual = 7;
            }
            else if (k == Formal::Intent::OUT)
            {
                kindqual = 8;
            }
            else if (k == Formal::Intent::INOUT)
            {
                kindqual = 9;
            }

            std::string ident{dynamic_cast<Formal const*>(ast)->name().c_str()};
            fk->args.emplace_back(Symbol{{kind_types{}, ident, {}, kindqual,
                false, symbolTable.symbolTableRef->id}});

            symbolTable.addEntry(fk->lutId, ident, fk->args.back());
        }
    }

}}}}    // namespace chpl::ast::visitors::hpx
