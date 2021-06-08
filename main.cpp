#include <iostream>
#include <ostream>

#include "./include/ThreatSystemA.hpp"
#include "./include/ThreatSystemB.hpp"
#include "./include/ThreatSystemMetis.hpp"
#include "./include/ThreatSystemMetisM.hpp"
#include "./include/ThreatSystemFagot.hpp"

struct TestConditions {
    TestConditions(
        double delta_time,
        double max_sim_time,
        double target_location_x,
        double target_location_y,
        double target_location_z,
        double target_velocity_x,
        double target_velocity_y,
        double target_velocity_z,
        double launcher_location_x,
        double launcher_location_y,
        double launcher_location_z
    ) {
        this->delta_time = delta_time;
        this->max_sim_time = max_sim_time;
        initial_launcher.transform.location.x = launcher_location_x;
        initial_launcher.transform.location.y = launcher_location_y;
        initial_launcher.transform.location.z = launcher_location_z;
        initial_target.location.x = target_location_x;
        initial_target.location.y = target_location_y;
        initial_target.location.z = target_location_z;
        initial_target.velocity.x = target_velocity_x;
        initial_target.velocity.y = target_velocity_y;
        initial_target.velocity.z = target_velocity_z;
    }

    ModelData::InitialiseParams initial_launcher;
    ModelData::PlatformTransform initial_target;
    double delta_time;
    double max_sim_time;
};

int main(int argc, char** argv) {
    std::cout << "Application Start..." << std::endl;

    // ThreatSystemA a;
    // a.Test2();

/*
    ThreatSystemA a;
    a.BindDll();

    ModelData::PlatformTransform initial_target_transform;
    initial_target_transform.location.x = 5500.0;
    initial_target_transform.location.y = 0.0;
    initial_target_transform.location.z = 0.0;
    a.SetPlatformTransform(initial_target_transform);
    
    // a.SetPlatformTransform();
    a.InitDll();
//*/

    std::vector<std::pair<TestConditions*, BaseModel*>*> model_tests;
    model_tests.push_back(new std::pair<TestConditions*, BaseModel*>(
        new TestConditions(
            0.001,
            40.0,
            5500.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            -1.0
        ), 
        new ThreatSystemA()));
    model_tests.push_back(new std::pair<TestConditions*, BaseModel*>(
        new TestConditions(
            0.0005,
            15.0,
            2000.0,
            2000.0,
            4000.0,
            0.0,
            -100.0,
            0.0,
            0.0,
            0.0,
            -1.0
        ), 
        new ThreatSystemB()));
    model_tests.push_back(new std::pair<TestConditions*, BaseModel*>(
        new TestConditions(
            0.001,
            15.0,
            1000.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            -1.0
        ), 
        new ThreatSystemMetis()));
    model_tests.push_back(new std::pair<TestConditions*, BaseModel*>(
        new TestConditions(
            0.001,
            15.0,
            1500.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            -1.0
        ), 
        new ThreatSystemMetisM()));
    model_tests.push_back(new std::pair<TestConditions*, BaseModel*>(
        new TestConditions(
            0.001,
            20.0,
            2000.0,
            0.0,
            -3.0,
            0.0,
            0.0,
            0.0,
            0.0,
            0.0,
            -1.0
        ), 
        new ThreatSystemFagot()));
    
    // std::vector<BaseModel*> models;
    // models.push_back(new ThreatSystemA());
    // models.push_back(new ThreatSystemB());
    // models.push_back(new ThreatSystemA());

    double max_sim_time;// = 40.0;

    double tof = 0.0;

    for(auto model_test : model_tests) {
        // TODO: When you implement the ThreatSystemFactory, make it responsible for calling their BindDll()!!!
        auto test_conditions = model_test->first;
        auto model = model_test->second;
        model->BindDll();

        double target_location_x = test_conditions->initial_target.location.x;
        double target_location_y = test_conditions->initial_target.location.y;
        double target_location_z = test_conditions->initial_target.location.z;
        double target_velocity_x = test_conditions->initial_target.velocity.x;
        double target_velocity_y = test_conditions->initial_target.velocity.y;
        double target_velocity_z = test_conditions->initial_target.velocity.z;

        ModelData::InitialiseParams initial_params;
        initial_params.transform.location.x = test_conditions->initial_launcher.transform.location.x;
        initial_params.transform.location.y = test_conditions->initial_launcher.transform.location.y;
        initial_params.transform.location.z = test_conditions->initial_launcher.transform.location.z;
        initial_params.transform.orientation.yaw = std::atan2(target_location_z, target_location_x);
        initial_params.transform.orientation.pitch = std::atan2(target_location_y, target_location_x);
        initial_params.transform.orientation.roll = 0.0;
        double delta_time = test_conditions->delta_time;
        max_sim_time = test_conditions->max_sim_time;
        model->InitDll(initial_params);
        ModelData::PlatformTransform initial_target_transform;
        initial_target_transform.location.x = target_location_x;
        initial_target_transform.location.y = target_location_y;
        initial_target_transform.location.z = target_location_z;
        model->SetTargetTransform(initial_target_transform);
        // model->SetLauncherTransform(initial_params.transform);
        // model->Tick();

        double total_time = 0.0;
        double target_distance = std::sqrt(
            std::pow(target_location_x, 2) +
            std::pow(target_location_y, 2) +
            std::pow(target_location_z, 2)
        );
        double threat_distance = 0.0;
        double calculated_miss_distance = target_distance;
        double last_calculated_miss_distance = target_distance;
        ModelData::UpdateParams update_params;
        update_params.delta_seconds = delta_time;

        ModelData::MissileTransform missile_transform;

        std::cout << "=============================" << std::endl
                  << "Testing threat system " << model->GetName() << std::endl
                  << "Target Location: " << target_location_x << ", " << target_location_y << ", " << target_location_z << std::endl
                  << "Target Velocity: " << target_velocity_x << ", " << target_velocity_y << ", " << target_velocity_z << std::endl
                  << "Launcher Location: " << initial_params.transform.location.x << ", " << initial_params.transform.location.y << ", " << initial_params.transform.location.z << std::endl
                  << "Starting in 2 Seconds..." << std::endl;
        Sleep(2000);

        while(calculated_miss_distance <= last_calculated_miss_distance) {
            target_location_x += target_velocity_x * delta_time;
            target_location_y += target_velocity_y * delta_time;
            target_location_z += target_velocity_z * delta_time;

            update_params.launcher_transform.location.x = 0;
            update_params.launcher_transform.location.y = 0;
            update_params.launcher_transform.location.z = 0;
            update_params.launcher_transform.orientation.yaw = std::atan2(target_location_z, target_location_x);
            update_params.launcher_transform.orientation.pitch = std::atan2(target_location_y, target_location_x);
            update_params.launcher_transform.orientation.roll = 0.0;

            update_params.target_transform.location.x = target_location_x;
            update_params.target_transform.location.y = target_location_y;
            update_params.target_transform.location.z = target_location_z;

            // std::cout << "Target Position: " << target_location_x << ", " << target_location_y << ", " << target_location_z << std::endl;

            model->SetLauncherTransform(update_params.launcher_transform);
            model->SetTargetTransform(update_params.target_transform);

            // Update the model.
            model->Tick();

            total_time += update_params.delta_seconds;

            model->GetMissileTransform(missile_transform);
            model->GetMissDistance(threat_distance);

            // model->GetTOF(tof);
            // std::cout << "--------------------" << std::endl
            //           << "TOF: " << tof << std::endl
            //           << "Target Position: " << target_location_x << ", " << target_location_y << ", " << target_location_z << std::endl
            //           << "Missile Position: " << missile_transform.location.x << ", " << missile_transform.location.y << ", " << missile_transform.location.z << std::endl
            //           << "Miss Distance: " << last_calculated_miss_distance << std::endl;
            // std::cout << "Missile Position: " << missile_transform.location.x << ", " << missile_transform.location.y << ", " << missile_transform.location.z << std::endl;

            last_calculated_miss_distance = calculated_miss_distance;
            calculated_miss_distance = std::sqrt(
                std::pow(target_location_x - missile_transform.location.x, 2) + 
                std::pow(target_location_y - missile_transform.location.y, 2) +
                std::pow(target_location_z - missile_transform.location.z, 2)
            );

            // Get the relevant outputs to calculate and update target and threat distance.

            // Check if the maximum time has been exceeded and break early if it has.
            if(total_time > max_sim_time) break;
        }


        model->GetTOF(tof);
        std::cout << "---------- Results ----------" << std::endl
                  << "TOF: " << tof << std::endl
                  << "Target Position: " << target_location_x << ", " << target_location_y << ", " << target_location_z << std::endl
                  << "Missile Position: " << missile_transform.location.x << ", " << missile_transform.location.y << ", " << missile_transform.location.z << std::endl
                  << "Miss Distance: " << last_calculated_miss_distance << std::endl
                  << "-----------------------------" << std::endl;

    }//*/

    // Test Stuff
    /*
    std::unique_ptr<std::vector<double>> inputs(new std::vector<double>);
    inputs->push_back(1.0);

    // BaseModel model = *model_iter;
    std::cout   << std::endl
                << "Construct and run Model:" << std::endl
                << "=========================" << std::endl;

    model->SetInputs(std::move(inputs));
    model->Tick();
    std::unique_ptr<std::vector<double>> model_outputs = model->GetOutputs();

    std::cout   << "Model Outputs:" << std::endl
                << "===============" << std::endl;
    for(auto i = model_outputs->begin(); i != model_outputs->end(); ++i) {
        std::cout << *i << ' ';
    }
    std::cout << std::endl;
    //*/

    std::cout << "All Done!" << std::endl;
}