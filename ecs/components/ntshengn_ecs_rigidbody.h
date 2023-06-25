#pragma once
#include <array>

namespace NtshEngn {

	struct Rigidbody {
		bool isStatic = false;
		std::array<float, 3> force = { 0.0f, 0.0f, 0.0f };
		float mass = 1.0f;
		float restitution = 0.0f;
		std::array<float, 3> torque = { 0.0f, 0.0f, 0.0f };
		float inertia = 1.0f;
	};

}