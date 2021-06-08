#include <iostream>
#include <ostream>

#include "./include/ThreatSystemB.hpp"

std::string ThreatSystemB::GetModelPath() {
    return "../../lib/b.dll";
}

void ThreatSystemB::DerivedTick() {
    // std::cout << "ThreatSystemB::OtherTick()" << std::endl;
}

bool ThreatSystemB::DoesModelRequireFastForward() {
    return true;
}

void ThreatSystemB::ModifyInOutMappings() {
    // Defaults are fine.
}

std::string ThreatSystemB::GetName() {
    return "B";
}