#pragma once
#include "../module_interfaces/ntshengn_asset_loader_module_interface.h"
#include "../resources/ntshengn_resources_audio.h"
#include "../resources/ntshengn_resources_graphics.h"
#include "../utils/ntshengn_defines.h"
#include "../utils/ntshengn_utils_bimap.h"
#include "../utils/ntshengn_utils_math.h"
#include <string>
#include <forward_list>
#include <array>

#if defined(NTSHENGN_DEBUG)
#define NTSHENGN_ASSET_MANAGER_INFO(message) \
	do { \
		std::cerr << "\33[1m\33[94mASSET MANAGER \33[34mINFO\33[39m\33[0m: " << message << std::endl; \
	} while(0)
#else
#define NTSHENGN_ASSET_MANAGER_INFO(message) \
	do { \
	} while(0)
#endif

#if defined(NTSHENGN_DEBUG)
#define NTSHENGN_ASSET_MANAGER_WARNING(message) \
	do { \
		std::cerr << "\33[1m\33[94mASSET MANAGER \33[93mWARNING\33[39m\33[0m: " << message << std::endl; \
	} while(0)
#else
#define NTSHENGN_ASSET_MANAGER_WARNING(message) \
	do { \
	} while(0)
#endif

#if defined(NTSHENGN_DEBUG)
#define NTSHENGN_ASSET_MANAGER_ERROR(message, code) \
	do { \
		std::cerr << "\33[1m\33[94mASSET MANAGER \33[31mERROR\33[39m\33[0m: " << message << " (" << #code << ")" << std::endl; \
		exit(1); \
	} while(0)
#else
#define NTSHENGN_ASSET_MANAGER_ERROR(message, code) \
	do { \
		exit(1); \
	} while(0)
#endif

namespace NtshEngn {

	class AssetManager {
	public:
		Sound* createSound();
		Sound* loadSound(const std::string& filePath);
		void destroySound(Sound* sound);

		Model* createModel();
		Model* loadModel(const std::string& filePath);
		void destroyModel(Model* model);

		Image* createImage();
		Image* loadImage(const std::string& filePath);
		void destroyImage(Image* image);

		void calculateTangents(Mesh& mesh);
		std::array<Math::vec3, 2> calculateAABB(const Mesh& mesh);

	public:
		void setAssetLoaderModule(AssetLoaderModuleInterface* assetLoaderModule);

	private:
		void loadSoundNtsd(const std::string& filePath, Sound& sound);
		void loadMeshNtmh(const std::string& filePath, Mesh& mesh);
		void loadImageSamplerNtsp(const std::string& filePath, ImageSampler& imageSampler);
		void loadMaterialNtml(const std::string& filePath, Material& material);
		void loadModelNtmd(const std::string& filePath, Model& model);
		void loadImageNtim(const std::string& filePath, Image& image);

	private:
		AssetLoaderModuleInterface* m_assetLoaderModule;

		std::forward_list<Sound> m_soundResources;
		std::forward_list<Model> m_modelResources;
		std::forward_list<Image> m_imageResources;

		Bimap<std::string, Sound*> m_soundPaths;
		Bimap<std::string, Model*> m_modelPaths;
		Bimap<std::string, Image*> m_imagePaths;
	};

}