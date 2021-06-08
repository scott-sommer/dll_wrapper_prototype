#pragma once
#include "BaseModel.hpp"

class ThreatSystemB : public BaseModel {
    public:
        std::string GetModelPath();
        bool DoesModelRequireFastForward();

        // std::unique_ptr<std::vector<double>> SetOtherInputs(std::unique_ptr<std::vector<double>>&& inputs);
        // std::unique_ptr<std::vector<double>> GetOtherOutputs();
        void DerivedTick();

        void ModifyInOutMappings();

        std::string GetName();
};