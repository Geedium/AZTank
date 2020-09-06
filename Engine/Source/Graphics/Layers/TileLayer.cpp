#include "System.h"
#include "TileLayer.h"

namespace Augiwne { namespace Graphics {
	TileLayer::TileLayer(Shader* shader)
		: Layer(new BatchRenderer2D(), shader, Maths::Matrix4::Orthographic(-16,16,-9,9,-1,1))
	{

	}
	TileLayer::~TileLayer()
	{

	}
}}