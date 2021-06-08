#pragma once
#include "DllModel.hpp"

class BaseModel : public DllModel {
    public:
        virtual std::string GetModelPath() = 0;
        virtual bool DoesModelRequireFastForward() = 0;

        // void SetInputs(std::unique_ptr<std::vector<double>>&& inputs);
        // std::unique_ptr<std::vector<double>> GetOutputs();
        void Tick();

        virtual std::string GetName() = 0;

        virtual bool SupportsTargetTransform() { return true; }
        void SetTargetTransform(ModelData::PlatformTransform& platform_transform);
    protected:
        // virtual std::unique_ptr<std::vector<double>> SetOtherInputs(std::unique_ptr<std::vector<double>>&& inputs) = 0;
        // virtual std::unique_ptr<std::vector<double>> GetOtherOutputs() = 0;
        virtual void DerivedTick() = 0;

        virtual void ModifyInOutMappings() = 0;
};