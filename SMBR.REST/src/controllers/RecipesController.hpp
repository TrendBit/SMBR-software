#pragma once

#include "BaseController.hpp"

#include "dto/MessageDto.hpp"
#include "dto/ScriptContentDto.hpp"
#include "dto/ScriptDto.hpp"

#include "SMBR/IRecipes.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

#undef OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS
#define OATPP_MACRO_API_CONTROLLER_ADD_CORS_BODY_DEFAULT_METHODS "GET, POST, OPTIONS, PUT, PATCH, DELETE"

/**
 * @class RecipesController
 * @brief Endpoints managing the stored recipes.
 */
class RecipesController : public SMBRControllerBase {
public:
    RecipesController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                      std::shared_ptr<ISystemModule> systemModule,
                      std::shared_ptr<IRecipes> recipes);

    /**
    * @brief Get list of recipes.
    */
    ENDPOINT_INFO(getRecipeList) {
        info->summary = "Get list of recipes";
        info->addTag("Recipes");
        info->description = "Return list of names of existing recipes.";

        // provide an example array so swagger shows realistic values instead of ["string"]
        auto example = oatpp::Vector<oatpp::String>::createShared();
        example->push_back("example|measurement");
        example->push_back("macros|calibration");
        example->push_back("testing|lights_on");
        info->addResponse<List<String>>(Status::CODE_200, "application/json", "List of recipe names")
            .addExample("application/json", example);
    }
    ADD_CORS(getRecipeList)
    ENDPOINT("GET", "/recipes", getRecipeList);

    /**
    * @brief Get list of recipes.
    */
    ENDPOINT_INFO(reloadRecipeList) {
        info->summary = "Reload recipes from filesystem";
        info->addTag("Recipes");
        info->description = "Reloads recipes from the filesystem.";

        auto example = oatpp::Vector<oatpp::String>::createShared();
        example->push_back("example|measurement");
        example->push_back("macros|calibration");
        info->addResponse<List<String>>(Status::CODE_200, "application/json", "List of recipe names")
            .addExample("application/json", example);
    }
    ADD_CORS(reloadRecipeList)
    ENDPOINT("PATCH", "/recipes", reloadRecipeList);

    /**
     * @brief Get recipe content
     */
    ENDPOINT_INFO(getRecipeContent) {
        info->summary = "Get recipe content";
        info->addTag("Recipes");
        info->description = "Return content of the recipe.";

        auto exampleContent = ScriptDto::createShared();
        exampleContent->name = "testing|lights_on";
        exampleContent->content = "main:\n    print \"start\"\n    illumination 0.5 0.5 0.5 0.5\n    display \"lights on\"\n    print \"done\"\n";
        info->addResponse<Object<ScriptDto>>(Status::CODE_200, "application/json", "Recipe content")
            .addExample("application/json", exampleContent);
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Recipe not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Recipe not found"}}));
    }
    ADD_CORS(getRecipeContent)
    ENDPOINT("GET", "/recipes/{recipeName}", getRecipeContent, PATH(String, recipeName));

    /** 
     * @brief Updates Recipe content
     */
    ENDPOINT_INFO(updateRecipe) {
        info->summary = "Update recipe content";
        info->addTag("Recipes");
        info->description = "Update content of the recipe.";
        auto requestExample = ScriptContentDto::createShared();
        requestExample->content = "main:\n    print \"start\"\n    illumination 1.0 1.0 1.0 1.0\n    display \"lights on\"\n    print \"done\"\n";
        info->addConsumes<Object<ScriptContentDto>>("application/json", "Recipe content to update")
            .addExample("application/json", requestExample);
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Recipe updated successfully")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Recipe updated successfully"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Recipe not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Recipe not found"}}));
    }
    ADD_CORS(updateRecipe)
    ENDPOINT("PUT", "/recipes/{recipeName}", updateRecipe, PATH(String, recipeName), BODY_DTO(Object<ScriptContentDto>, body));
    
    /**
     * @brief Deletes recipe
     */
    ENDPOINT_INFO(deleteRecipe) {
        info->summary = "Delete recipe";
        info->addTag("Recipes");
        info->description = "Delete the recipe.";
        info->addResponse<Object<MessageDto>>(Status::CODE_200, "application/json", "Recipe deleted successfully")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Recipe deleted successfully"}}));
        info->addResponse<Object<MessageDto>>(Status::CODE_404, "application/json", "Recipe not found")
            .addExample("application/json", oatpp::Fields<oatpp::String>({{"message", "Recipe not found"}}));
    }
    ADD_CORS(deleteRecipe)
    ENDPOINT("DELETE", "/recipes/{recipeName}", deleteRecipe, PATH(String, recipeName));

private:
    std::shared_ptr<IRecipes> recipes_;

};

#include OATPP_CODEGEN_END(ApiController)
