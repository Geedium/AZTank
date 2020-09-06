#pragma once
#include "..\shader.h"
#include "..\Renderers\2D\BatchRenderer2D.h"
#include "Layer.h"

namespace Augiwne { namespace Graphics {
	class TileLayer : public Layer {
	public:
		TileLayer(Shader* shader);
		~TileLayer();
	};
}}