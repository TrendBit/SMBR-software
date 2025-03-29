#include "VirtualSensorModule.hpp"
#include <iostream>       // std::cout
#include <future>         // std::async, std::future
#include "Random.hpp"


std::future <float> VirtualSensorModule::getBottleTemperature() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(20.0, 30.0);
    });
}

std::future <float> VirtualSensorModule::getTopMeasuredTemperature() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(20.0, 30.0);
    });
}

std::future <float> VirtualSensorModule::getBottomMeasuredTemperature() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(20.0, 30.0);
    });
}

std::future <float> VirtualSensorModule::getTopSensorTemperature() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(20.0, 30.0);
    });
}

std::future <float> VirtualSensorModule::getBottomSensorTemperature() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(20.0, 30.0);
    });
}

std::future <bool> VirtualSensorModule::clearCustomText(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return true;
    });
}

std::future <bool> VirtualSensorModule::printCustomText(std::string text){
    return std::async(std::launch::async, [text]() {
        Random::randomDelay();
        return true;
    });
}

std::future <ISensorModule::FluorometerOjipData> VirtualSensorModule::startFluorometerOjipCapture(Fluorometer_config::Gain detector_gain, Fluorometer_config::Timing sample_timing, float emitor_intensity, uint16_t length_ms, uint16_t samples) {
    return std::async(std::launch::async, [&]() {
        Random::randomDelay();
        return ISensorModule::FluorometerOjipData {
            .measurement_id = 1235,
            .detector_gain = Fluorometer_config::Gain::x1,
            .timebase = Fluorometer_config::Timing::Logarithmic,
            .emitor_intensity = 0.5f,
            .samples = {},  
            .read = false,
            .length_ms = 1000,
            .required_samples = 1000,
            .captured_samples = 998,
            .missing_samples = 2
        };
    });
}

std::future <bool> VirtualSensorModule::isFluorometerOjipCaptureComplete(){
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return true;
    });
}

std::future <ISensorModule::FluorometerOjipData> VirtualSensorModule::retrieveFluorometerOjipData() {
    return std::async(std::launch::async, [&]() {
        Random::randomDelay();
        return ISensorModule::FluorometerOjipData {
            .measurement_id = 1235,
            .detector_gain = Fluorometer_config::Gain::x1,
            .timebase = Fluorometer_config::Timing::Logarithmic,
            .emitor_intensity = 0.5f,
            .samples = {},  
            .read = false,
            .length_ms = 1000,
            .required_samples = 1000,
            .captured_samples = 998,
            .missing_samples = 2
        };
    });
}

std::future <ISensorModule::FluorometerDetectorInfo> VirtualSensorModule::getFluorometerDetectorInfo() {
    return std::async(std::launch::async, [&]() {
        Random::randomDelay();
        return ISensorModule::FluorometerDetectorInfo {
            .peak_wavelength = 750,
            .sensitivity = 100,
            .sampling_rate = 500         
        };
    });
}

std::future <float> VirtualSensorModule::getFluorometerDetectorTemperature() {
    return std::async(std::launch::async, []() {
        Random::randomDelay();
        return Random::nextFloat(20.0, 30.0);
    });
}

std::future <ISensorModule::FluorometerEmitorInfo> VirtualSensorModule::getFluorometerEmitorInfo() {
    return std::async(std::launch::async, [&]() {
        Random::randomDelay();
        return ISensorModule::FluorometerEmitorInfo {
            .peak_wavelength = 525,
            .power_output = 10000
        };
    });
}







