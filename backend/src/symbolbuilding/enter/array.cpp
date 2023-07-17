#include "hpx/symbolbuildingvisitor.hpp"

using namespace chpl::uast;

// global options
extern bool suppressLineDirectives;
extern bool fullFilePath;

namespace chpl { namespace ast { namespace visitors { namespace hpx {

    SYMBOL_VISITOR_DEF_ENTER_H_F(Array)
    {
        if (sym.has_value())
        {
            if (sym->get().kind.index() == 0)
            {
                sym->get().kind = std::make_shared<array_kind>(
                    array_kind{{symbolTable.symbolTableRef->id,
                        std::string{"array_" + emitChapelLine(ast)},
                        {Symbol{{
                            std::make_shared<domain_kind>(
                                domain_kind{{symbolTable.symbolTableRef->id,
                                    std::string{
                                        "domain_" + emitChapelLine(ast)},
                                    {},
                                    std::make_shared<kind_node_type>(
                                        kind_node_type{})}}),
                            std::string{"domain_" + emitChapelLine(ast)},
                            {},
                            -1,
                            false,
                            symbolTable.symbolTableRef->id,
                        }}},
                        std::make_shared<kind_node_type>(kind_node_type{
                            {std::make_shared<array_kind>(array_kind{{}})}})}});
            }
            else
            {
                std::get<std::shared_ptr<kind_node_type>>(
                    std::get<std::shared_ptr<array_kind>>(sym->get().kind)
                        ->retKind)
                    ->children.emplace_back(std::make_shared<array_kind>(
                        array_kind{{symbolTable.symbolTableRef->id,
                            std::string{"array_" + emitChapelLine(ast)},
                            {Symbol{{
                                std::make_shared<domain_kind>(
                                    domain_kind{{symbolTable.symbolTableRef->id,
                                        std::string{
                                            "domain_" + emitChapelLine(ast)},
                                        {},
                                        std::make_shared<kind_node_type>(
                                            kind_node_type{})}}),
                                std::string{"domain_" + emitChapelLine(ast)},
                                {},
                                -1,
                                false,
                                symbolTable.symbolTableRef->id,
                            }}},
                            std::make_shared<kind_node_type>(
                                kind_node_type{{std::make_shared<array_kind>(
                                    array_kind{{}})}})}}));
            }
        }
    }
}}}}    // namespace chpl::ast::visitors::hpx