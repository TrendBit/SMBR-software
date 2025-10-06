#pragma once

#include "oatpp/Types.hpp"
#include "oatpp/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 * @brief Data Transfer Object representing a single module issue.
 *
 */
class ModuleIssueDto : public oatpp::DTO {
    DTO_INIT(ModuleIssueDto, DTO)

    /**
     * @brief Unique identifier of the issue.
     */
    DTO_FIELD(UInt16, id);         

    /**
     * @brief Human-readable name or description of the issue.
     */       
    DTO_FIELD(String, name);  

    /**
     * @brief Severity level of the issue (e.g., "Warning", "Critical").
     */           
    DTO_FIELD(Int16, index);      

    /**
     * @brief Timestamp indicating when the problem last occurred.
     */  
    DTO_FIELD(String, timestamp);  

    /**
     * @brief Associated value or measurement related to the issue.
     */    
    DTO_FIELD(Float32, value);       

    /**
     * @brief Type of the module reporting the issue.
     */
    DTO_FIELD(String, module);       

    /**
     * @brief Instance identifier of the module.
     */
    DTO_FIELD(String, instance);       
};

#include OATPP_CODEGEN_END(DTO)