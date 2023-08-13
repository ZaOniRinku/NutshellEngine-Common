#pragma once
#include "ntshengn_system_module_interface.h"
#include "../resources/ntshengn_resources_graphics.h"

namespace NtshEngn {

	class GraphicsModuleInterface : public SystemModuleInterface {
	public:
		GraphicsModuleInterface() {}
		GraphicsModuleInterface(const std::string& name) : SystemModuleInterface(ModuleType::Graphics, name) {}
		virtual ~GraphicsModuleInterface() {}

		virtual void init() = 0;
		virtual void update(double dt) = 0;
		virtual void destroy() = 0;

		// Loads the mesh described in the mesh parameter in the internal format and returns a unique identifier
		virtual MeshID load(const Mesh& mesh) = 0;
		// Loads the image described in the image parameter in the internal format and returns a unique identifier
		virtual ImageID load(const Image& image) = 0;
		// Loads the font described in the font parameter in the internal format and returns a unique identifier
		virtual FontID load(const Font& font) = 0;

		// Draws a text on the UI with the font in the fontID parameter using the position on screen and color
		virtual void drawUIText(FontID fontID, const std::string& text, const Math::vec2& position, const Math::vec4& color) = 0;
		// Draws a line on the UI according to its start and end points and its color
		virtual void drawUILine(const Math::vec2& start, const Math::vec2& end, const Math::vec4& color) = 0;
		// Draws a rectangle on the UI according to its position, its size (width and height) and its color
		virtual void drawUIRectangle(const Math::vec2& position, const Math::vec2& size, const Math::vec4& color) = 0;
	};

}