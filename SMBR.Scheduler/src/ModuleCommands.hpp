#include <future>
#include <iostream>
#include <Poco/String.h>
#include <SMBR/Interpreter.hpp>
#include <SMBR/ISensorModule.hpp>
#include <SMBR/Script.hpp>
#include <SMBR/Log.hpp>

namespace Scripting {

    template <class Impl, class Input>
    class ModuleCommand : public ICommand {
        public:
            ModuleCommand(Block::Ptr block, ParseContext::Ptr pctx) : lineNumber(block->line.lineNumber()){
                input = Impl().parse(block->line);
            }
            void run(RunContext::Ptr rctx) override {
                LTRACE("Scheduler") << "Sch run " << name() << LE;
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
            std::string name() const override {
                //return name of Impl type
                std::string name = typeid(Impl).name();
                Poco::replaceInPlace(name, "class ", "");
                Poco::replaceInPlace(name, "Impl", "");
                Poco::replaceInPlace(name, "ModuleCommand<", "");
                Poco::replaceInPlace(name, ">", "");
                Poco::replaceInPlace(name, "std::", "");
                Poco::replaceInPlace(name, "::", "-");
                return name + "-line-" + std::to_string(lineNumber);

            }
        private:
            int lineNumber;
            Input input;
    };

    struct EmptyInput {
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

    class HeaterOff {
        public:
            EmptyInput parse(ScriptLine l){
                EmptyInput input;
                return input;
            }
            std::future <bool> run(EmptyInput input, ISystemModule::Ptr m){
                return m->controlModule()->turnOffHeater();
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
                    input.message = "";
                    for (int i = 0; i < l.args(); i++) {
                        if (i > 0) input.message += " ";
                        input.message += l.arg(i);
                    }
                    input.message = Poco::replace(input.message, "\"", "");
                }
                return input;
            }
            std::future <bool> run(DisplayInput input, ISystemModule::Ptr m){
                return m->sensorModule()->printCustomText(input.message);
            }
    };

    class Fluorometer {
        public:
            ISensorModule::FluorometerInput parse(ScriptLine l){
                ISensorModule::FluorometerInput input;
                input.sample_timing = Fluorometer_config::Timing::Logarithmic;
                input.emitor_intensity = 1.0f;

                input.length_ms = l.argAsInt(0, 500, 2000);
                input.sample_count = l.argAsInt(1, 500, 1000);
                int gain = l.argAsInt(2, 1, 50);

                if (gain == 1){
                    input.detector_gain = Fluorometer_config::Gain::x1;
                } else if (gain == 10){
                    input.detector_gain = Fluorometer_config::Gain::x10;
                } else if (gain == 50){
                    input.detector_gain = Fluorometer_config::Gain::x50;
                } else{
                    throw std::runtime_error("Invalid gain value: " + std::to_string(gain));
                }

                return input;
            }

            std::future <bool> run(ISensorModule::FluorometerInput input, ISystemModule::Ptr m){
                LINFO("Thread") << "Fluorometer new thread started" << LE;

                return std::async(std::launch::async, [input, m]() {
                    auto ojip_data = m->sensorModule()->captureFluorometerOjip(input).get();
                    std::cout << "OJIP measurement is missing: " << ojip_data.missing_samples <<" samples" << std::endl;
                    return ojip_data.captured_samples != 0;
                });
            }
    };

    class FluorometerCalibrate {
        public:
            EmptyInput parse(ScriptLine l){
                EmptyInput input;
                return input;
            }
            std::future <bool> run(EmptyInput input, ISystemModule::Ptr m){
                return m->sensorModule()->calibrateFluorometer();
            }
    };

    typedef ModuleCommand <Heater, HeaterInput> HeaterCommand;
    typedef ModuleCommand <Illumination, IlluminationInput> IlluminationCommand;
    typedef ModuleCommand <Mixer, MixerInput> MixerCommand;
    typedef ModuleCommand <Pump, PumpInput> PumpCommand;
    typedef ModuleCommand <Aerator, AeratorInput> AeratorCommand;
    typedef ModuleCommand <Display, DisplayInput> DisplayCommand;
    typedef ModuleCommand <Fluorometer, ISensorModule::FluorometerInput> FluorometerCommand;
    typedef ModuleCommand <HeaterOff, EmptyInput> HeaterOffCommand;
    typedef ModuleCommand <FluorometerCalibrate, EmptyInput> FluorometerCalibrateCommand;
}


void registerModuleBlocks(CommandFactory & f);


