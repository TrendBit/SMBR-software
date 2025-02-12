#include <SMBR/Interpreter.hpp>
#include <SMBR/Script.hpp>
#include <future>

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
                input.intensity = l.argAsFloat(0, -1.0, 1.0);
                return input;
            }
            std::future <bool> run(HeaterInput input, ISystemModule::Ptr m){
                return m->controlModule()->setHeaterIntensity(input.intensity);
            }
    };

    
    

    struct MixerInput {
        double rpm = 0.0;
        double length = 0.0;
    };

    class Mixer {
        public:
            MixerInput parse(ScriptLine l){
                MixerInput input;
                input.rpm = l.argAsFloat(0, 0.0, 1000.0);
                input.length = l.argAsFloat(1, 0.0, 100.0);
                return input;
            }
            std::future <bool> run(MixerInput input, ISystemModule::Ptr m){
                return m->controlModule()->stirMixer(input.rpm, input.length);
            }
    };

    struct PumpInput {
        double volume = 0.0;
    };

    class Pump {
        public:
            PumpInput parse(ScriptLine l){
                PumpInput input;
                input.volume = l.argAsFloat(0, 0.0, 100.0);
                return input;
            }
            std::future <bool> run(PumpInput input, ISystemModule::Ptr m){
                return m->controlModule()->moveCuvettePump(input.volume, 0.0);
            }
    };

    struct AeratorInput {
        double volume = 0.0;
    };

    class Aerator {
        public:
            AeratorInput parse(ScriptLine l){
                AeratorInput input;
                input.volume = l.argAsFloat(0, 0.0, 100.0);
                return input;
            }
            std::future <bool> run(AeratorInput input, ISystemModule::Ptr m){
                return m->controlModule()->moveAerator(input.volume, 0.0);
            }
    };



    typedef ModuleCommand <Heater, HeaterInput> HeaterCommand;
    typedef ModuleCommand <Illumination, IlluminationInput> IlluminationCommand;
    typedef ModuleCommand <Mixer, MixerInput> MixerCommand;
    typedef ModuleCommand <Pump, PumpInput> PumpCommand;
    typedef ModuleCommand <Aerator, AeratorInput> AeratorCommand;

}


void registerModuleBlocks(CommandFactory & f);


