#pragma once
#include <map>
#include <vector>
#include <Windows.h>

#include "DataTypes.hpp"

//typedefs for dll function pointers
typedef int(*CreateModel_fn)                (void);
typedef int(*FreeModel_fn)                  (int);
typedef int(*InitializeModel_fn)            (int);
typedef int(*SetInput_fn)                   (int, int, double);
typedef int(*SetParameter_fn)               (int, int, double);
typedef int(*SetParameterString_fn)         (int, int, const char*);
typedef int(*UpdateModel_fn)                (int);
typedef int(*GetOutput_fn)                  (int, int, double*);
typedef int(*WasLastUpdateFastEnough_fn)    (int, int*);
typedef int(*GetMaxElapsedTimeInSeconds_fn) (int, double*);
typedef int(*GetLastElapsedTimeInSeconds_fn)(int, double*);
typedef int(*GetLastRuntimeRate_fn)         (int, double*);
typedef int(*SetInputArray_fn)              (int, int, double*);
typedef int(*SetParameterArray_fn)          (int, int, double*);
typedef int(*GetOutputArray_fn)             (int, int, double*);

const static int SIMULINK_NED_CONVERSION = -1;

class DllModel {
    public:
        // We want the derived class to be able to adjust the DLL's input array ordering as DLL interfaces can vary.
        enum DefaultInputMapping {
            in_launcher_pos_n = 0,
            in_launcher_vel_n = 1,
            in_launcher_vel_e = 2,
            in_launcher_vel_d = 3,
            in_launcher_launch_cmd = 4,
            in_launcher_az = 5,
            in_launcher_el = 6,
            in_launcher_pos_e = 7,
            in_launcher_pos_d = 8,
            in_latitude = 9,
            in_longitude = 10,
            in_altitude = 11,
            in_target_pos_n = 12,
            in_target_pos_e = 13,
            in_target_pos_d = 14
        };
        static const DefaultInputMapping FIRST_INPUT_MAPPING = in_launcher_pos_n;  // Need this to set up input_mappings with the defaults.
        static const DefaultInputMapping LAST_INPUT_MAPPING = in_target_pos_d;  // Need this to set up input_mappings with the defaults.
        int GetInputIndex(int input_enum_value) { return input_mappings[input_enum_value]; };
        
        // We want the derived class to be able to adjust the DLL's output array ordering as DLL interfaces can vary.
        enum DefaultOutputMapping {
            out_weapon_enable      = 0,
            out_launch_command     = 1,
            out_tof                = 2,
            out_euler              = 3,
            out_z_cmd              = 6,
            out_y_cmd              = 7,
            out_laser_field        = 8,
            out_horiz_scan_freq    = 9,
            out_vert_scan_freq     = 10,
            out_laser_signal       = 11,
            out_miss_distance      = 12,
            out_track_lim_flg      = 13,
            out_track_rate_lim_flg = 14,
            out_mis_pos_n          = 15,
            out_mis_pos_e          = 16,
            out_mis_pos_d          = 17,
            out_mis_vel_n          = 18,
            out_mis_vel_e          = 19,
            out_mis_vel_d          = 20,
            out_launcher_az        = 21,
            out_launcher_el        = 22,
            out_mis_pos_ecef_roll  = 23,
            out_mis_pos_ecef_pitch = 24,
            out_mis_pos_ecef_yaw   = 25,
            out_launcher_pos_n     = 26,
            out_launcher_pos_e     = 27,
            out_launcher_pos_d     = 28,
            out_z_laser_err        = 29,
            out_y_laser_err        = 30,
            out_laser_diverg_az    = 31,
            out_laser_diverg_el    = 32,
            out_mis_ang_vel        = 33,
            out_mis_acc            = 36,
            out_r_time             = 39,
            out_mis_los_err        = 40
        };
        static const DefaultOutputMapping FIRST_OUTPUT_MAPPING = out_weapon_enable;  // Need this to set up output_mappings with the defaults.
        static const DefaultOutputMapping LAST_OUTPUT_MAPPING = out_mis_los_err;    // Need this to set up output_mappings with the defaults.
        int GetOutputIndex(int output_enum_value) { return output_mappings[output_enum_value]; };


        DllModel();
        ~DllModel();

        virtual std::string GetModelPath() { return ""; };
        virtual bool DoesModelRequireFastForward() { return false; };
        void BindDll();
        void InitDll(const ModelData::InitialiseParams& params);

        void SetInputs(std::unique_ptr<std::vector<double>>&& inputs);
        std::unique_ptr<std::vector<double>> GetOutputs();
        void Tick();

        void GetLauncherTransform(ModelData::PlatformTransform& launcher_transform);
        void GetMissileTransform(ModelData::MissileTransform& missile_transform);
        void GetMissDistance(double& miss_distance);
        void GetStepSize(double& step_size);
        void GetTOF(double& tof);

        // void SetTargetTransform(ModelData::PlatformTransform& platform_transform);
        void SetLauncherTransform(ModelData::PlatformTransform& launcher_transform);
        void SetMissileTransform(ModelData::MissileTransform& missile_transform);

        void Test();
        void Test2();
    protected:
        std::map<int, int> input_mappings;
        std::map<int, int> output_mappings;

        std::vector<double> inputs;
        std::vector<double> outputs;
        
	    HINSTANCE hDll;

        int model_id;
        double last_run_rate;
        bool has_model_fast_forwarded = false;
        double step_size;
        
        // Initialize function pointers to NULL
        CreateModel_fn                 pProcCreateModel = NULL;
        FreeModel_fn                   pProcFreeModel = NULL;
        InitializeModel_fn             pProcInitializeModel = NULL;
        SetInput_fn                    pProcSetInput = NULL;
        SetParameter_fn                pProcSetParameter = NULL;
        SetParameterString_fn          pProcSetParameterString = NULL;
        UpdateModel_fn                 pProcUpdateModel = NULL;
        GetOutput_fn                   pProcGetOutput = NULL;
        WasLastUpdateFastEnough_fn     pProcWasLastUpdateFastEnough = NULL;
        GetMaxElapsedTimeInSeconds_fn  pProcGetMaxElapsedTimeInSeconds = NULL;
        GetLastElapsedTimeInSeconds_fn pProcGetLastElapsedTimeInSeconds = NULL;
        GetLastRuntimeRate_fn          pProcGetLastRuntimeRate = NULL;
        SetInputArray_fn               pProcSetInputArray = NULL;
        SetParameterArray_fn           pProcSetParameterArray = NULL;
        GetOutputArray_fn              pProcGetOutputArray = NULL;

        virtual void ModifyInOutMappings() {};
    private:
        bool initial_tick;


        void SetupInOutMappings();

        // void SetInitialData(const ModelData::InitialiseParams& params);
        void UpdateLauncher(const ModelData::InitialiseParams& params);
        void UpdatePosition(const ModelData::Vec3D& location);
        void UpdateOrientation(const ModelData::Rot3D& orientation);
        void UpdateModel();
};