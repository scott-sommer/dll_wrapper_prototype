#include <iostream>
#include <ostream>

#include "./include/BaseModel.hpp"

// void BaseModel::SetInputs(std::unique_ptr<std::vector<double>>&& inputs) {
//     std::cout << "BaseModel::SetInputs()" << std::endl;

//     std::unique_ptr<std::vector<double>> returned_inputs = SetOtherInputs(std::move(inputs));

//     DllModel::SetInputs(std::move(returned_inputs));
// }

// std::unique_ptr<std::vector<double>> BaseModel::GetOutputs() {
//     std::cout << "BaseModel::GetOutputs()" << std::endl;

//     std::unique_ptr<std::vector<double>> outputs = DllModel::GetOutputs();
//     std::unique_ptr<std::vector<double>> other_outputs = GetOtherOutputs();
    
//     outputs->insert(
//         outputs->end(), 
//         other_outputs->begin(), 
//         other_outputs->end()
//     );
    
//     return std::move(outputs);
// }

void BaseModel::Tick() {
    DllModel::Tick();
    DerivedTick();
}

void BaseModel::SetTargetTransform(ModelData::PlatformTransform& platform_transform) {
    if(!SupportsTargetTransform()) return;

    pProcSetInput(model_id, GetInputIndex(in_target_pos_n), platform_transform.location.x);
    pProcSetInput(model_id, GetInputIndex(in_target_pos_e), platform_transform.location.y);
    pProcSetInput(model_id, GetInputIndex(in_target_pos_d), platform_transform.location.z); // * SIMULINK_NED_CONVERSION
}