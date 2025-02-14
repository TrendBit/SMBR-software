#include "SMBR/APIClient.hpp"
#include "SMBR/IScheduler.hpp"

class APIClientImpl;

class APIScheduler : public IScheduler {
    public: 
        APIScheduler(APIClientParams params);
        virtual ~APIScheduler();

        unsigned long long start() override;
        void stop() override;

        void setScriptFromString(const ScriptInfo & ) override;
        void setScriptFromFile(const std::string & filename) override;

        ScriptInfo getScript() const override;
        RuntimeInfo getRuntimeInfo() const override;
        
    private:
        std::shared_ptr<APIClientImpl> impl;
};


