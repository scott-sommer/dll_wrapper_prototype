#pragma once

namespace ModelData {
	struct Vec3D {
		double x{ 0.0 };
		double y{ 0.0 };
		double z{ 0.0 };

		friend bool operator==(const Vec3D& lhs, const Vec3D& rhs) {
			return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
		}
	};

	struct Rot3D {
		double pitch{ 0.0 };
		double yaw{ 0.0 };
		double roll{ 0.0 };

		friend bool operator==(const Rot3D& lhs, const Rot3D& rhs) {
			return lhs.pitch == rhs.pitch && lhs.yaw == rhs.yaw && lhs.roll == rhs.roll;
		}
	};

	typedef Vec3D Location;
	typedef Rot3D Orientation;

	struct PlatformTransform {
		Vec3D location;
        Vec3D velocity;
		Rot3D orientation;

		friend bool operator==(const PlatformTransform& lhs, const PlatformTransform& rhs) {
			return lhs.location == rhs.location && lhs.orientation == rhs.orientation;
		}
	};

    struct MissileTransform {
        Vec3D location;
        Vec3D velocity;
        Vec3D accelleration;
        Rot3D orientation;
        Rot3D angular_velocity;
    };
    
	struct InitialiseParams {
		PlatformTransform transform;
	};
    
	struct UpdateParams {
        double delta_seconds = 0.0;
		PlatformTransform launcher_transform;
		PlatformTransform target_transform;
	};
};