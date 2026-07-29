#include "RecipesController.hpp"

#include "SMBR/Exceptions.hpp"
#include "SMBR/Log.hpp"
#include "ControllerUtils.hpp"

#include <chrono>

using namespace std::chrono_literals;

RecipesController::RecipesController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers,
                                     std::shared_ptr<ISystemModule> systemModule,
                                     std::shared_ptr<IRecipes> recipes)
    : SMBRControllerBase(apiContentMappers, systemModule)
    , recipes_(recipes)
{}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> RecipesController::getRecipeList() {
    try {
        LDEBUG("API") << "Api getRecipeList begin" << LE;
        auto sc = recipes_->getRecipeNames();
        auto dtoList = oatpp::List<oatpp::String>::createShared();
        for (auto s : sc){
            dtoList->push_back(s);
        }
        LDEBUG("API") << "Api getRecipeList end (success)" << LE;
        return createDtoResponse(Status::CODE_200, dtoList);
    } catch (std::exception & e){
        LWARNING("API") << "Api getRecipeList end (failure: " << e.what() << ")" << LE;
        auto dto = MessageDto::createShared();
        dto->message = "Failed to retrieve recipe list: " + std::string(e.what());
        return createDtoResponse(Status::CODE_500, dto);
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> RecipesController::reloadRecipeList() {
    try {
        recipes_->reload();
        return getRecipeList();
    } catch (std::exception & e){
        auto dto = MessageDto::createShared();
        dto->message = "Failed to reload recipe list: " + std::string(e.what());
        return createDtoResponse(Status::CODE_500, dto);
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> RecipesController::getRecipeContent(const oatpp::String& name) {
    try {
        std::string nn = decodeRecipeName(name);
        LDEBUG("API") << "Api getRecipeContent " << nn << " begin" << LE;
        
        auto s = recipes_->getRecipeContent(nn);
        auto scriptResponseDto = ScriptDto::createShared();
        scriptResponseDto->name = s.name;
        scriptResponseDto->content = s.content;
        LDEBUG("API") << "Api getRecipeContent end (success)" << LE;
        return createDtoResponse(Status::CODE_200, scriptResponseDto);
    } catch (std::exception & e){
        LWARNING("API") << "Api getRecipeContent end (failure: " << e.what() << ")" << LE;
        auto dto = MessageDto::createShared();
        dto->message = "Failed to retrieve recipe: " + std::string(e.what());
        return createDtoResponse(Status::CODE_404, dto);
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> RecipesController::updateRecipe(const oatpp::String& name, const oatpp::Object<ScriptContentDto>& body) {
    try {

        LDEBUG("API") << "Api updateRecipe begin" << LE;
        if (!body || !body->content) {
            throw ArgumentException("Invalid script. Must contain content.");
        }
        ScriptInfo s;
        s.name = decodeRecipeName(name);
        s.content = *body->content;
        recipes_->replaceRecipe(s);
        LDEBUG("API") << "Api updateRecipe end (success)" << LE;
        auto dto = MessageDto::createShared();
        dto->message = "Recipe updated successfully.";
        return createDtoResponse(Status::CODE_200, dto);
    } catch (std::exception & e){
        LWARNING("API") << "Api updateRecipe end (failure: " << e.what() << ")" << LE;
        auto dto = MessageDto::createShared();
        dto->message = "Failed to update recipe: " + std::string(e.what());
        return createDtoResponse(Status::CODE_500, dto);
    }
}

std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> RecipesController::deleteRecipe(const oatpp::String& name) {
    try {
        std::string nn = decodeRecipeName(name);
        LDEBUG("API") << "Api deleteRecipe " << nn << " begin" << LE;
        
        recipes_->deleteRecipe(nn);
        LDEBUG("API") << "Api deleteRecipe end (success)" << LE;
        auto dto = MessageDto::createShared();
        dto->message = "Recipe deleted successfully.";
        return createDtoResponse(Status::CODE_200, dto);
    } catch (std::exception & e){
        LWARNING("API") << "Api deleteRecipe end (failure: " << e.what() << ")" << LE;
        auto dto = MessageDto::createShared();
        dto->message = "Failed to delete recipe: " + std::string(e.what());
        return createDtoResponse(Status::CODE_404, dto);
    }
}
