#pragma once
#include "ntshengn_module_interface.h"
#include "../resources/ntshengn_resources_audio.h"
#include "../resources/ntshengn_resources_graphics.h"
#include "../utils/ntshengn_utils_math.h"
#include <string>
#include <array>

namespace NtshEngn {

	class AssetLoaderModuleInterface : public ModuleInterface {
	public:
		AssetLoaderModuleInterface() {}
		AssetLoaderModuleInterface(const std::string& name) : ModuleInterface(ModuleType::AssetLoader, name) {}
		virtual ~AssetLoaderModuleInterface() {}
		
		// Loads the sound in file at path filePath
		virtual Sound loadSound(const std::string& filePath) = 0;
		// Loads the image in file at path filePath
		virtual Image loadImage(const std::string& filePath) = 0;
		// Loads the model in file at path filePath
		virtual Model loadModel(const std::string& filePath) = 0;
		// Loads the font in file at path filePath
		virtual Font loadFont(const std::string& filePath, float fontHeight) = 0;

		// Calculate tangents for mesh
		virtual void calculateTangents(Mesh& mesh) = 0;
	};

}