#include <iostream>
#include <ostream>

#include "./include/ThreatSystemA.hpp"

std::string ThreatSystemA::GetModelPath() {
    return "../../lib/a.dll";
}

void ThreatSystemA::DerivedTick() {
    // std::cout << "ThreatSystemA::DerivedTick()" << std::endl;
}

bool ThreatSystemA::DoesModelRequireFastForward() {
    return false;
}

void ThreatSystemA::ModifyInOutMappings() {
	// This threat system does not support the Lat/Lon inputs, and the following inputs in the array are at different indexes.
    input_mappings.erase(in_latitude);
    input_mappings.erase(in_longitude);
    input_mappings[in_altitude] = 9,
    input_mappings[in_target_pos_n] = 10,
    input_mappings[in_target_pos_e] = 11,
    input_mappings[in_target_pos_d] = 12

	// This threat system does not support the Missile LOS Error output.
    output_mappings.erase(out_mis_los_err);

	// It is also possible to add extra enum/index pairs to the map for new inputs/outputs that future models may have. This is just an example, and I have not created the additional enums for ThreatSystemA.
	// input_mappings[ThreatSystemAInputs.in_seeker_mode] = 13;
}

std::string ThreatSystemA::GetName() {
    return "A";
}