#include "SMBR/APIClient.hpp"
#include <memory>

class APIClientImpl;

class APITester  {
    public: 
        APITester(APIClientParams params);
        virtual ~APITester();

        void capture();
        bool captureDone();
        void retrieveLast();

    private:
        std::shared_ptr<APIClientImpl> impl;
};


