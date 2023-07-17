#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_ENTER_H_F(Domain)
    {
        if (sym.has_value())
        {
            if (std::holds_alternative<std::shared_ptr<array_kind>>(
                    sym->get().kind))
            {
                std::get<std::shared_ptr<array_kind>>(sym->get().kind)
                    ->args.emplace_back(Symbol{{
                        std::make_shared<domain_kind>(
                            domain_kind{{symbolTable.symbolTableRef->id,
                                std::string{"domain_" + emitChapelLine(ast)},
                                {},
                                std::make_shared<kind_node_type>(
                                    kind_node_type{})}}),
                        std::string{"domain_" + emitChapelLine(ast)},
                        {},
                        -1,
                        false,
                        symbolTable.symbolTableRef->id,
                    }});
            }
        }
    }
}}}}    // namespace chpl::ast::visitors::hpx