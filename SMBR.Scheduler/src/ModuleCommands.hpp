#include <future>
#include <Poco/String.h>
#include <SMBR/Interpreter.hpp>
#include <SMBR/ISensorModule.hpp>
#include <SMBR/Script.hpp>

namespace Scripting {

    template <class Impl, class Input>
    class ModuleCommand : public ICommand {
        public:
            ModuleCommand(Block::Ptr block, ParseContext::Ptr pctx) : lineNumber(block->line.lineNumber()){
                input = Impl().parse(block->line);
            }
            void run(RunContext::Ptr rctx) override {
                rctx->stack->updateTop(lineNumber);
                if (rctx->module == nullptr){
                    throw std::runtime_error("Module is null");
                }
                auto ftr = Impl().run(input, rctx->module);
                ftr.wait();
                if (!ftr.get()) {
                    throw std::runtime_error("Command failed");
                }
            }
        private:
            int lineNumber;
            Input input;
    };


    struct IlluminationInput {
        double i1 = 0.0;
        double i2 = 0.0;
        double i3 = 0.0;
        double i4 = 0.0;
    };

    class Illumination {
        public:

            IlluminationInput parse(ScriptLine l){
                IlluminationInput input;
                input.i1 = l.argAsFloat(0, 0.0, 1.0);
                input.i2 = l.argAsFloat(1, 0.0, 1.0);
                input.i3 = l.argAsFloat(2, 0.0, 1.0);
                input.i4 = l.argAsFloat(3, 0.0, 1.0);
                return input;
            }
            std::future <bool> run(IlluminationInput input, ISystemModule::Ptr m){
                return m->controlModule()->setIntensities(input.i1, input.i2, input.i3, input.i4);
            }
    };

    struct HeaterInput {
        double intensity = 0.0;
    };

    class Heater {
        public:
            HeaterInput parse(ScriptLine l){
                HeaterInput input;
                input.intensity = l.argAsFloat(0, -10.0, 60.0);
                return input;
            }
            std::future <bool> run(HeaterInput input, ISystemModule::Ptr m){
                return m->controlModule()->setHeaterTargetTemperature(input.intensity);
            }
    };


    struct MixerInput {
        double length = 0.0;
        double rpm = 0.0;
    };

    class Mixer {
        public:
            MixerInput parse(ScriptLine l){
                MixerInput input;
                input.length = l.argAsFloat(0, 0.0, 24*3600.0);
                input.rpm = l.argAsFloat(1, 0.0, 10000.0);
                return input;
            }
            std::future <bool> run(MixerInput input, ISystemModule::Ptr m){
                return m->controlModule()->stirMixer(input.rpm, input.length);
            }
    };

    struct PumpInput {
        double volume = 0.0;
        double flowrate = 0.0;
    };

    class Pump {
        public:
            PumpInput parse(ScriptLine l){
                PumpInput input;
                input.volume = l.argAsFloat(0, -1000.0, 1000.0);
                input.flowrate = l.argAsFloat(1, 0.0, 1000.0);
                return input;
            }
            std::future <bool> run(PumpInput input, ISystemModule::Ptr m){
                return m->controlModule()->moveCuvettePump(input.volume, input.flowrate);
            }
    };

    struct AeratorInput {
        double volume = 0.0;
        double flowrate = 0.0;
    };

    class Aerator {
        public:
            AeratorInput parse(ScriptLine l){
                AeratorInput input;
                input.volume = l.argAsFloat(0, 0.0, 5000.0);
                input.flowrate = l.argAsFloat(1, 0.0, 5000.0);
                return input;
            }
            std::future <bool> run(AeratorInput input, ISystemModule::Ptr m){
                return m->controlModule()->moveAerator(input.volume, input.flowrate);
            }
    };

    struct DisplayInput {
        std::string message;
    };

    class Display {
        public:
            DisplayInput parse(ScriptLine l){
                DisplayInput input;
                if (l.noArgs()){
                    input.message = "";
                } else {
                    input.message = l.arg(0);
                    input.message = Poco::replace(input.message, "\"", "");
                }
                return input;
            }
            std::future <bool> run(DisplayInput input, ISystemModule::Ptr m){
                if (input.message.empty()){
                    return m->sensorModule()->clearCustomText();
                } else {
                    // Clear display before printing otherwise is text appended
                    return std::async(std::launch::async, [m, input] {
                        m->sensorModule()->clearCustomText();
                        return m->sensorModule()->printCustomText(input.message).get();
                    });
                }
            }
    };

    class Fluorometer {
        public:
            ISensorModule::FluorometerInput parse(ScriptLine l){
                ISensorModule::FluorometerInput input;
                input.detector_gain = Fluorometer_config::Gain::x1;
                input.sample_timing = Fluorometer_config::Timing::Logarithmic;
                input.emitor_intensity = 1.0f;
                input.length_ms = l.argAsInt(0, 500, 2000);
                input.sample_count = l.argAsInt(1, 500, 1000);
                return input;
            }

            std::future <bool> run(ISensorModule::FluorometerInput input, ISystemModule::Ptr m){
                return std::async(std::launch::async, [input, m]() {
                    auto ojip_data = m->sensorModule()->captureFluorometerOjip(input).get();
                    std::cout << "OJIP measurement is missing: " << ojip_data.missing_samples <<" samples" << std::endl;
                    return ojip_data.captured_samples != 0;
                });
            }
    };

    typedef ModuleCommand <Heater, HeaterInput> HeaterCommand;
    typedef ModuleCommand <Illumination, IlluminationInput> IlluminationCommand;
    typedef ModuleCommand <Mixer, MixerInput> MixerCommand;
    typedef ModuleCommand <Pump, PumpInput> PumpCommand;
    typedef ModuleCommand <Aerator, AeratorInput> AeratorCommand;
    typedef ModuleCommand <Display, DisplayInput> DisplayCommand;
    typedef ModuleCommand <Fluorometer, ISensorModule::FluorometerInput> FluorometerCommand;
}


void registerModuleBlocks(CommandFactory & f);


