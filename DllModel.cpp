#include <iostream>
#include <ostream>

#include "./include/DllModel.hpp"

DllModel::DllModel() :
    model_id(0),
    last_run_rate(0.0),
    initial_tick(true),
    step_size(0.005) {
    std::cout << "DllModel()" << std::endl;
}

DllModel::~DllModel() {
    std::cout << "~DllModel()" << std::endl;
}

void DllModel::BindDll() {
    std::string model_path = GetModelPath();

    hDll = LoadLibraryA(model_path.c_str());
    if(hDll) {
        std::cout << "DLL loaded successfully from path: " << model_path << std::endl;
        
		pProcCreateModel = (CreateModel_fn)GetProcAddress(hDll, "CreateModel");
		pProcFreeModel = (FreeModel_fn)GetProcAddress(hDll, "FreeModel");
		pProcInitializeModel = (InitializeModel_fn)GetProcAddress(hDll, "InitializeModel");
		pProcSetInput = (SetInput_fn)GetProcAddress(hDll, "SetInput");
		pProcSetParameter = (SetParameter_fn)GetProcAddress(hDll, "SetParameter");
		pProcSetParameterString = (SetParameterString_fn)GetProcAddress(hDll, "SetParameterString");
		pProcUpdateModel = (UpdateModel_fn)GetProcAddress(hDll, "UpdateModel");
		pProcGetOutput = (GetOutput_fn)GetProcAddress(hDll, "GetOutput");
		pProcWasLastUpdateFastEnough = (WasLastUpdateFastEnough_fn)GetProcAddress(hDll, "WasLastUpdateFastEnough");
		pProcGetMaxElapsedTimeInSeconds = (GetMaxElapsedTimeInSeconds_fn)GetProcAddress(hDll, "GetMaxElapsedTimeInSeconds");
		pProcGetLastElapsedTimeInSeconds = (GetLastElapsedTimeInSeconds_fn)GetProcAddress(hDll, "GetLastElapsedTimeInSeconds");
		pProcGetLastRuntimeRate = (GetLastRuntimeRate_fn)GetProcAddress(hDll, "GetLastRuntimeRate");
		pProcSetInputArray = (SetInputArray_fn)GetProcAddress(hDll, "SetInputArray");
		pProcSetParameterArray = (SetParameterArray_fn)GetProcAddress(hDll, "SetParameterArray");
		pProcGetOutputArray = (GetOutputArray_fn)GetProcAddress(hDll, "GetOutputArray");

        model_id = pProcCreateModel();

        if(model_id < 0) {
            std::cout << "Failed to create the DLL model" << std::endl;
        } else {
            std::cout << "Created DLL model with id: " << model_id << std::endl;
        }
        
        if (!(hDll && pProcCreateModel                 &&
            pProcFreeModel                   &&
            pProcInitializeModel             &&
            pProcSetInput                    &&
            pProcSetParameter                &&
            pProcSetParameterString          &&
            pProcUpdateModel                 &&
            pProcGetOutput                   &&
            pProcWasLastUpdateFastEnough     &&
            pProcGetMaxElapsedTimeInSeconds  &&
            pProcGetLastElapsedTimeInSeconds &&
            pProcGetLastRuntimeRate			&&
            pProcSetInputArray        &&
            pProcSetParameterArray    &&
            pProcGetOutputArray
            ))
        {
            std::cout << "Failed to load model DLL and all Functions" << std::endl;
        }
        else {
            std::cout << "Verified successful load model DLL and all Functions" << std::endl;

            SetupInOutMappings();
        }
    } else {
        std::cout << "Could not load DLL from path: " << model_path << std::endl;
    }
}

struct SLModelData {

	double time_of_flight;
	double launcher_pos[3]; // x,y,z in metres
	double threat_pos[3]; // x,y,z in metres
	double lat_lon_alt[3];
	double mis_pos_ned[3]; // x,y,z in metres
	double mis_pos_ecef[3]; // x,y,z
	double launcher_qtrn[4];
	double mis_ang_vel[3]; // radians/sec
	double launcher_euler[3]; // radians
	double mis_euler[3]; // radians
	double mis_vel[3]; //m/s
	double mis_acc[3]; // m/s^2
	double launcher_trigger;
	double laser_signal;
	double mis_distance; // metres
	double scan_freq[2]; //vertical,horizontal
	double field_of_view; // degrees
	double laser_field;
	double laser_error[2]; // Y,Z
	double laser_diverg[2]; //Az,El
	double ycmd;
	double zcmd;

};

void DllModel::Test2() {
    BindDll();
    
    SLModelData slData;
    int tickRate{ 10 };
    double target_n{ 500.0 };
    double target_e{ 0.0 };
    double target_d{ 0.0 };

    ModelData::PlatformTransform launcher_transform;
    launcher_transform.location.x = 0.0;
    launcher_transform.location.y = 0.0;
    launcher_transform.location.z = -1.0;
    SetLauncherTransform(launcher_transform);
    

	// // Set relevant inputs
	// pProcSetInput(model_id, in_launcher_pos_n, 0);
	// pProcSetInput(model_id, in_launcher_pos_e, 0);

	// // Unreal Engine sends launcher origin at base of tripod. Offset 1.35 metres from origin to fire at the tube.
	// // Simulink uses NED, UE uses NEU.
	// pProcSetInput(model_id, in_launcher_pos_d,  -1.0);

	// pProcSetInput(model_id, in_launcher_az, 0.0);
	// pProcSetInput(model_id, in_launcher_el, 0.0);
    std::cout << "GetInputindex(in_launcher_launch_cmd): " << GetInputIndex(in_launcher_launch_cmd) << std::endl;
	pProcSetInput(model_id, GetInputIndex(in_launcher_launch_cmd), 1);

	// if (slData.threat_pos[0] != 0 && slData.threat_pos[1] != 0) {
	// 	pProcSetInput(model_id, in_target_pos_n, target_n);
	// 	pProcSetInput(model_id, in_target_pos_e, target_e);
	// 	pProcSetInput(model_id, in_target_pos_d, target_d);

	// }
	double step;
	pProcGetMaxElapsedTimeInSeconds(model_id, &step);
	step *= 1000; // Step size from seconds to ms
	std::cout << "Interval set to " << tickRate  << " ms. Model step size is " <<  step << "ms" << std::endl; 
	std::cout << "Starting run in 2 seconds..." << std::endl;
	Sleep(2000);
	int count = 0;

	while (1) {
		pProcUpdateModel(model_id);


		pProcGetOutput(model_id, out_tof, &slData.time_of_flight);
		

		pProcGetOutput(model_id, out_horiz_scan_freq, &slData.scan_freq[0]);
		pProcGetOutput(model_id, out_vert_scan_freq, &slData.scan_freq[1]);
		pProcGetOutputArray(model_id, out_euler, slData.mis_euler);
		pProcGetOutput(model_id, out_miss_distance, &slData.mis_distance);
		pProcGetOutput(model_id, out_launch_command, &slData.launcher_trigger);
		pProcGetOutputArray(model_id, out_mis_pos_n, slData.mis_pos_ned);
		pProcGetOutputArray(model_id, out_mis_pos_ecef_roll, slData.mis_pos_ecef);
		pProcGetOutputArray(model_id, out_mis_vel_n, slData.mis_vel);
		pProcGetOutputArray(model_id, out_mis_acc, slData.mis_acc);
		//pProcGetOutputArray(model_id, outKORNET_Launcher_Pos_NED, slData.launcher_pos);
		pProcGetOutputArray(model_id, out_mis_ang_vel, slData.mis_ang_vel);

		double launcher_az = 0.0;
		double launcher_ele = 0.0;
		pProcGetOutput(model_id, out_launcher_az, &launcher_az);
		pProcGetOutput(model_id, out_launcher_el, &launcher_ele);

		// Specific to Kornet
		pProcGetOutput(model_id, out_y_cmd, &slData.ycmd);
		pProcGetOutput(model_id, out_z_cmd, &slData.zcmd);
		// Not currently in use. simply collecting output from Simulink model.
		pProcGetOutput(model_id, out_track_lim_flg, &slData.laser_error[0]);
		pProcGetOutput(model_id, out_track_rate_lim_flg, &slData.laser_error[1]);
		pProcGetOutput(model_id, out_laser_diverg_az, &slData.laser_diverg[0]);
		pProcGetOutput(model_id, out_laser_diverg_el, &slData.laser_diverg[1]);

		pProcGetOutput(model_id, out_laser_field, &slData.laser_field);
		//pProcGetOutput(model_id, outKORNET_Laser_signal, &slData.laser_signal);
		//pProcGetOutput(model_id, outKORNET_Mis_Los_Err, &slData.laser_signal); // overwrite
	
		// double fan_mode = 0.0;
		// pProcGetOutput(model_id, 41, &fan_mode);

		// double wire_cond = 0.0;
		// pProcGetOutput(model_id, 42, &wire_cond);

		// double guidance_reset = 0.0;
		// pProcGetOutput(model_id, 43, &guidance_reset);

		//if (count == 100) {
			// std::stringstream sstream;

			std::cout << "TOF: " << slData.time_of_flight
				<< "\tMis Pos: X - " << slData.mis_pos_ned[0]
				<< "\tY - " << slData.mis_pos_ned[1]
				<< "\tZ - " << slData.mis_pos_ned[2]
				<< "\tMis Eul: " << slData.mis_euler[0]
				<< "\t, " << slData.mis_euler[1]
				<< "\t, " << slData.mis_euler[2]
				<< "\tPlat NED, " << slData.launcher_pos[0]
				<< "\t, " << slData.launcher_pos[1]
				<< "\t, " << slData.launcher_pos[2]
				<< "\t, Plat az" << launcher_az
				<< "\t, Plat ele" << launcher_ele
				<< "\tRange: " << slData.mis_distance
				<< "\tTrack Rate Lim: " << slData.laser_error[1]
				<< "\tGuidance Y: " << slData.ycmd
				<< "\tZ: " << slData.zcmd
				<< "\tMiss Distance: " << slData.mis_distance
				// << "\tFan mode: " << fan_mode
				// << "\tWire cond: " << wire_cond
				// << "\tGuidance reset: " << guidance_reset
				<< "\n";

			// std::cout << sstream.str();
			count = 0;
		//}
		count++;
		Sleep(tickRate);

	}
	
	pProcFreeModel(model_id);
}

void DllModel::Test() {


	int modelId;
    std::string model_path = GetModelPath();

    HINSTANCE hDll = LoadLibraryA(model_path.c_str());
	// HINSTANCE hDll = LoadLibrary(vm["model_name"].as<std::string>().c_str());
	std::cout << model_path.c_str() << std::endl;
	if (hDll) {
		std::cout << model_path.c_str() << " -- Model DLL Loaded Successfully\n";

	}
	else {

		std::cout << "MCS_Lib -- Failed to load Model DLL\n";
		DWORD err = GetLastError();
		char err_str[70];
		sprintf_s(err_str, "MCS_Lib -- Failed to load Model DLL: %d", err);
		std::cout << err_str << std::endl;
	}

	if (hDll)
	{
		pProcCreateModel = (CreateModel_fn)GetProcAddress(hDll, "CreateModel");
		pProcFreeModel = (FreeModel_fn)GetProcAddress(hDll, "FreeModel");
		pProcInitializeModel = (InitializeModel_fn)GetProcAddress(hDll, "InitializeModel");
		pProcSetInput = (SetInput_fn)GetProcAddress(hDll, "SetInput");
		pProcSetParameter = (SetParameter_fn)GetProcAddress(hDll, "SetParameter");
		pProcSetParameterString = (SetParameterString_fn)GetProcAddress(hDll, "SetParameterString");
		pProcUpdateModel = (UpdateModel_fn)GetProcAddress(hDll, "UpdateModel");
		pProcGetOutput = (GetOutput_fn)GetProcAddress(hDll, "GetOutput");
		pProcWasLastUpdateFastEnough = (WasLastUpdateFastEnough_fn)GetProcAddress(hDll, "WasLastUpdateFastEnough");
		pProcGetMaxElapsedTimeInSeconds = (GetMaxElapsedTimeInSeconds_fn)GetProcAddress(hDll, "GetMaxElapsedTimeInSeconds");
		pProcGetLastElapsedTimeInSeconds = (GetLastElapsedTimeInSeconds_fn)GetProcAddress(hDll, "GetLastElapsedTimeInSeconds");
		pProcGetLastRuntimeRate = (GetLastRuntimeRate_fn)GetProcAddress(hDll, "GetLastRuntimeRate");
		pProcSetInputArray = (SetInputArray_fn)GetProcAddress(hDll, "SetInputArray");
		pProcSetParameterArray = (SetParameterArray_fn)GetProcAddress(hDll, "SetParameterArray");
		pProcGetOutputArray = (GetOutputArray_fn)GetProcAddress(hDll, "GetOutputArray");

		modelId = pProcCreateModel();
		if (modelId < 0)
		{
			std::cerr << "Failed to create model" << std::endl;
			return;
		}
		int result = pProcInitializeModel(modelId);
		if (result < 0) 
		{
			std::cerr << "Failed to initialise model "<< modelId << std::endl;
			return;
		}

		
	}

    SLModelData slData;
    int tickRate{ 10 };
    double target_n{ 500.0 };
    double target_e{ 0.0 };
    double target_d{ 0.0 };

	// Set relevant inputs
	pProcSetInput(modelId, in_launcher_pos_n, 0);
	pProcSetInput(modelId, in_launcher_pos_e, 0);

	// Unreal Engine sends launcher origin at base of tripod. Offset 1.35 metres from origin to fire at the tube.
	// Simulink uses NED, UE uses NEU.
	pProcSetInput(modelId, in_launcher_pos_d,  -1.0);

	pProcSetInput(modelId, in_launcher_az, 0.0);
	pProcSetInput(modelId, in_launcher_el, 0.0);
	pProcSetInput(modelId, in_launcher_launch_cmd, 1);

	// if (slData.threat_pos[0] != 0 && slData.threat_pos[1] != 0) {
	// 	pProcSetInput(modelId, in_target_pos_n, target_n);
	// 	pProcSetInput(modelId, in_target_pos_e, target_e);
	// 	pProcSetInput(modelId, in_target_pos_d, target_d);

	// }
	double step;
	pProcGetMaxElapsedTimeInSeconds(modelId, &step);
	step *= 1000; // Step size from seconds to ms
	std::cout << "Interval set to " << tickRate  << " ms. Model step size is " <<  step << "ms" << std::endl; 
	std::cout << "Starting run in 2 seconds..." << std::endl;
	Sleep(2000);
	int count = 0;

	while (1) {
		pProcUpdateModel(modelId);


		pProcGetOutput(modelId, out_tof, &slData.time_of_flight);
		

		pProcGetOutput(modelId, out_horiz_scan_freq, &slData.scan_freq[0]);
		pProcGetOutput(modelId, out_vert_scan_freq, &slData.scan_freq[1]);
		pProcGetOutputArray(modelId, out_euler, slData.mis_euler);
		pProcGetOutput(modelId, out_miss_distance, &slData.mis_distance);
		pProcGetOutput(modelId, out_launch_command, &slData.launcher_trigger);
		pProcGetOutputArray(modelId, out_mis_pos_n, slData.mis_pos_ned);
		pProcGetOutputArray(modelId, out_mis_pos_ecef_roll, slData.mis_pos_ecef);
		pProcGetOutputArray(modelId, out_mis_vel_n, slData.mis_vel);
		pProcGetOutputArray(modelId, out_mis_acc, slData.mis_acc);
		//pProcGetOutputArray(modelId, outKORNET_Launcher_Pos_NED, slData.launcher_pos);
		pProcGetOutputArray(modelId, out_mis_ang_vel, slData.mis_ang_vel);

		double launcher_az = 0.0;
		double launcher_ele = 0.0;
		pProcGetOutput(modelId, out_launcher_az, &launcher_az);
		pProcGetOutput(modelId, out_launcher_el, &launcher_ele);

		// Specific to Kornet
		pProcGetOutput(modelId, out_y_cmd, &slData.ycmd);
		pProcGetOutput(modelId, out_z_cmd, &slData.zcmd);
		// Not currently in use. simply collecting output from Simulink model.
		pProcGetOutput(modelId, out_track_lim_flg, &slData.laser_error[0]);
		pProcGetOutput(modelId, out_track_rate_lim_flg, &slData.laser_error[1]);
		pProcGetOutput(modelId, out_laser_diverg_az, &slData.laser_diverg[0]);
		pProcGetOutput(modelId, out_laser_diverg_el, &slData.laser_diverg[1]);

		pProcGetOutput(modelId, out_laser_field, &slData.laser_field);
		//pProcGetOutput(modelId, outKORNET_Laser_signal, &slData.laser_signal);
		//pProcGetOutput(modelId, outKORNET_Mis_Los_Err, &slData.laser_signal); // overwrite
	
		// double fan_mode = 0.0;
		// pProcGetOutput(modelId, 41, &fan_mode);

		// double wire_cond = 0.0;
		// pProcGetOutput(modelId, 42, &wire_cond);

		// double guidance_reset = 0.0;
		// pProcGetOutput(modelId, 43, &guidance_reset);

		//if (count == 100) {
			// std::stringstream sstream;

			std::cout << "TOF: " << slData.time_of_flight
				<< "\tMis Pos: X - " << slData.mis_pos_ned[0]
				<< "\tY - " << slData.mis_pos_ned[1]
				<< "\tZ - " << slData.mis_pos_ned[2]
				<< "\tMis Eul: " << slData.mis_euler[0]
				<< "\t, " << slData.mis_euler[1]
				<< "\t, " << slData.mis_euler[2]
				<< "\tPlat NED, " << slData.launcher_pos[0]
				<< "\t, " << slData.launcher_pos[1]
				<< "\t, " << slData.launcher_pos[2]
				<< "\t, Plat az" << launcher_az
				<< "\t, Plat ele" << launcher_ele
				<< "\tRange: " << slData.mis_distance
				<< "\tTrack Rate Lim: " << slData.laser_error[1]
				<< "\tGuidance Y: " << slData.ycmd
				<< "\tZ: " << slData.zcmd
				<< "\tMiss Distance: " << slData.mis_distance
				// << "\tFan mode: " << fan_mode
				// << "\tWire cond: " << wire_cond
				// << "\tGuidance reset: " << guidance_reset
				<< "\n";

			// std::cout << sstream.str();
			count = 0;
		//}
		count++;
		Sleep(tickRate);

	}
	
	pProcFreeModel(modelId);

	// return 0;

    /*
    BindDll();

    ModelData::PlatformTransform target;
    target.location.x = 5500.0;
    target.location.y = 0.0;
    target.location.z = 0.0;
    target.orientation.yaw = 0.0;
    target.orientation.pitch = 0.0;
    target.orientation.roll = 0.0;

    ModelData::MissileTransform threat;
    threat.location.x = 0.0;
    threat.location.y = 0.0;
    threat.location.z = 0.0;
    threat.orientation.yaw = 0.0;
    threat.orientation.pitch = 0.0;
    threat.orientation.roll = 0.0;

    ModelData::InitialiseParams init_params;
    init_params.transform = target;
    
	double tof = 0.0;

    std::cout << "Missile Position: " << threat.location.x << ", " << threat.location.y << ", " << threat.location.z << std::endl;
    InitDll(init_params);
    Tick();
	pProcGetOutput(model_id, GetOutputIndex(out_tof), &tof);
    std::cout << "TOF: " << tof << std::endl;
    GetMissileTransform(threat);
    std::cout << "Missile Position: " << threat.location.x << ", " << threat.location.y << ", " << threat.location.z << std::endl;
    pProcGetMaxElapsedTimeInSeconds(model_id, &step_size);
    std::cout << "Step Size: " << step_size << std::endl;

    UpdatePosition(threat.location);
    UpdateOrientation(threat.orientation);
    Tick();
	pProcGetOutput(model_id, GetOutputIndex(out_tof), &tof);
    std::cout << "TOF: " << tof << std::endl;
    GetMissileTransform(threat);
    std::cout << "Missile Position: " << threat.location.x << ", " << threat.location.y << ", " << threat.location.z << std::endl;
    pProcGetMaxElapsedTimeInSeconds(model_id, &step_size);
    std::cout << "Step Size: " << step_size << std::endl;

    UpdatePosition(threat.location);
    UpdateOrientation(threat.orientation);
    Tick();
	pProcGetOutput(model_id, GetOutputIndex(out_tof), &tof);
    std::cout << "TOF: " << tof << std::endl;
    GetMissileTransform(threat);
    std::cout << "Missile Position: " << threat.location.x << ", " << threat.location.y << ", " << threat.location.z << std::endl;
    pProcGetMaxElapsedTimeInSeconds(model_id, &step_size);
    std::cout << "Step Size: " << step_size << std::endl;
    //*/
}

void DllModel::GetTOF(double& tof) {
	pProcGetOutput(model_id, GetOutputIndex(out_tof), &tof);
}

void DllModel::InitDll(const ModelData::InitialiseParams& params) {

    int result = pProcInitializeModel(model_id);
    std::cout << "InitDLL Result: " << result << std::endl;
    UpdatePosition(params.transform.location);
    UpdateOrientation(params.transform.orientation);
    pProcGetMaxElapsedTimeInSeconds(model_id, &step_size);
    
    std::cout << "Step Size: " << step_size << std::endl;
}

void DllModel::SetupInOutMappings() {
    for(DefaultInputMapping input_enum = FIRST_INPUT_MAPPING; input_enum <= LAST_INPUT_MAPPING; input_enum = static_cast<DefaultInputMapping>(static_cast<int>(input_enum)+1)) {
        input_mappings[static_cast<int>(input_enum)] = static_cast<int>(input_enum);
        
        if(input_enum == LAST_INPUT_MAPPING) break;
    }

    for(DefaultOutputMapping output_enum = FIRST_OUTPUT_MAPPING; output_enum <= LAST_OUTPUT_MAPPING; output_enum = static_cast<DefaultOutputMapping>(static_cast<int>(output_enum)+1)) {
        output_mappings[output_enum] = output_enum;
        
        if(output_enum == LAST_OUTPUT_MAPPING) break;
    }

    ModifyInOutMappings();
}

void DllModel::SetInputs(std::unique_ptr<std::vector<double>>&& inputs) {
    // TODO: check inputs is valid, hDll not null.
    std::cout << "DllModel::SetInputs()" << std::endl;

    this->inputs.clear();
    this->inputs.insert(
        this->inputs.end(), 
        std::make_move_iterator(inputs->begin()), 
        std::make_move_iterator(inputs->end())
    );

    // inputs gets deleted when it goes out of scope.
}

std::unique_ptr<std::vector<double>> DllModel::GetOutputs() {
    // TODO: check hDll not null.
    std::cout << "DllModel::GetOutputs()" << std::endl;

    std::unique_ptr<std::vector<double>> return_vector(new std::vector<double>);
    return_vector->insert(
        return_vector->end(),
        inputs.begin(),
        inputs.end()
    );
    return std::move(return_vector);
}

void DllModel::Tick() {
    // TODO: check hDll not null.
    // std::cout << "DllModel::Tick()" << std::endl;

    UpdateModel();

    // outputs = inputs;
    // outputs.push_back(4.0);
}

void DllModel::GetLauncherTransform(ModelData::PlatformTransform& launcher_transform) {
    pProcGetOutput(model_id, GetOutputIndex(out_launcher_pos_n), &launcher_transform.location.x);
    pProcGetOutput(model_id, GetOutputIndex(out_launcher_pos_e), &launcher_transform.location.y);
    pProcGetOutput(model_id, GetOutputIndex(out_launcher_pos_d), &launcher_transform.location.z);
    
    pProcGetOutput(model_id, GetOutputIndex(out_launcher_az), &launcher_transform.orientation.yaw);
    pProcGetOutput(model_id, GetOutputIndex(out_launcher_el), &launcher_transform.orientation.pitch);
    launcher_transform.orientation.roll = 0.0;
}

void DllModel::GetMissileTransform(ModelData::MissileTransform& missile_transform) {
    pProcGetOutput(model_id, GetOutputIndex(out_mis_pos_n), &missile_transform.location.x);
    pProcGetOutput(model_id, GetOutputIndex(out_mis_pos_e), &missile_transform.location.y);
    pProcGetOutput(model_id, GetOutputIndex(out_mis_pos_d), &missile_transform.location.z); // Check if we need to correct the value with  * SIMULINK_NED_CONVERSION
    
    pProcGetOutput(model_id, GetOutputIndex(out_mis_vel_n), &missile_transform.velocity.x);
    pProcGetOutput(model_id, GetOutputIndex(out_mis_vel_e), &missile_transform.velocity.y);
    pProcGetOutput(model_id, GetOutputIndex(out_mis_vel_d), &missile_transform.velocity.z); // Check if we need to correct the value with  * SIMULINK_NED_CONVERSION
    
    pProcGetOutput(model_id, GetOutputIndex(out_mis_pos_ecef_roll), &missile_transform.orientation.roll);
    pProcGetOutput(model_id, GetOutputIndex(out_mis_pos_ecef_pitch), &missile_transform.orientation.pitch);
    pProcGetOutput(model_id, GetOutputIndex(out_mis_pos_ecef_yaw), &missile_transform.orientation.yaw);
}

void DllModel::GetMissDistance(double& miss_distance) {
    pProcGetOutput(model_id, GetOutputIndex(out_miss_distance), &miss_distance);
}

void DllModel::GetStepSize(double& step_size) {
    pProcGetLastElapsedTimeInSeconds(model_id, &step_size);
    // pProcGetOutput(model_id, );
}

void DllModel::UpdatePosition(const ModelData::Vec3D& location) {
    pProcSetInput(model_id, GetInputIndex(in_launcher_pos_n), location.x);
    pProcSetInput(model_id, GetInputIndex(in_launcher_pos_e), location.y);
    pProcSetInput(model_id, GetInputIndex(in_launcher_pos_d), location.z * SIMULINK_NED_CONVERSION);   // UE Coordinate system is in NEU, the dll models are in NED.
}

void DllModel::UpdateOrientation(const ModelData::Rot3D& orientation) {
    pProcSetInput(model_id, GetInputIndex(in_launcher_az), orientation.yaw);
    pProcSetInput(model_id, GetInputIndex(in_launcher_el), orientation.pitch);
}

void DllModel::UpdateModel() {
	pProcGetLastRuntimeRate(model_id, &last_run_rate);

	pProcSetInput(model_id, GetInputIndex(in_launcher_launch_cmd), 1);

	pProcUpdateModel(model_id);

	// Check if model needs to be fast forwarded through initial stage
	double tof = 0.0;
	pProcGetOutput(model_id, GetOutputIndex(out_tof), &tof);
	while (tof <= 0 && DoesModelRequireFastForward() && !has_model_fast_forwarded) {
		pProcUpdateModel(model_id);
		pProcGetOutput(model_id, GetOutputIndex(out_tof), &tof);
	}

	has_model_fast_forwarded = true;
}

// void DllModel::SetTargetTransform(ModelData::PlatformTransform& platform_transform) {
//     pProcSetInput(model_id, GetInputIndex(in_target_pos_n), platform_transform.location.x);
//     pProcSetInput(model_id, GetInputIndex(in_target_pos_e), platform_transform.location.y);
//     pProcSetInput(model_id, GetInputIndex(in_target_pos_d), platform_transform.location.z); // * SIMULINK_NED_CONVERSION
// }

void DllModel::SetLauncherTransform(ModelData::PlatformTransform& launcher_transform) {
    pProcSetInput(model_id, GetInputIndex(in_launcher_pos_n), launcher_transform.location.x);
    pProcSetInput(model_id, GetInputIndex(in_launcher_pos_e), launcher_transform.location.y);
    pProcSetInput(model_id, GetInputIndex(in_launcher_pos_d), launcher_transform.location.z * SIMULINK_NED_CONVERSION);

    pProcSetInput(model_id, GetInputIndex(in_launcher_az), launcher_transform.orientation.yaw);
    pProcSetInput(model_id, GetInputIndex(in_launcher_el), launcher_transform.orientation.pitch);
    // No roll required, assumed zero.
}

void DllModel::SetMissileTransform(ModelData::MissileTransform& missile_transform) {
    pProcSetInput(model_id, GetInputIndex(in_launcher_pos_n), missile_transform.location.x);
    pProcSetInput(model_id, GetInputIndex(in_launcher_pos_e), missile_transform.location.y);
    pProcSetInput(model_id, GetInputIndex(in_launcher_pos_d), missile_transform.location.z);
}