#pragma once
#include "BaseModel.hpp"

class ThreatSystemA : public BaseModel {
    public:
        std::string GetModelPath();
        bool DoesModelRequireFastForward();

        // std::unique_ptr<std::vector<double>> SetOtherInputs(std::unique_ptr<std::vector<double>>&& inputs);
        // std::unique_ptr<std::vector<double>> GetOtherOutputs();
        void DerivedTick();

        void ModifyInOutMappings();

        bool SupportsTargetTransform() { return false; }

        std::string GetName();
};