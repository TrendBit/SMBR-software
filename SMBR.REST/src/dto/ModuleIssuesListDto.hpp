#pragma once

#include "oatpp/Types.hpp"
#include "oatpp/macro/codegen.hpp"
#include "dto/ModuleIssueDto.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing a list of module issues.
 *
 */
class ModuleIssuesListDto : public oatpp::DTO {
    DTO_INIT(ModuleIssuesListDto, DTO)

    /**
     * @brief Informational message accompanying the issue list.
     *
     */
    DTO_FIELD(String, message);

    /**
     * @brief List of reported module issues.
     *
     */
    DTO_FIELD(List<Object<ModuleIssueDto>>, issues);
};

#include OATPP_CODEGEN_END(DTO)