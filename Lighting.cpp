#include "Lighting.h"

namespace ENG
{
	/**
	* Uploads lighting information to shader.
	*/
	void setLights(ENG::Entities& entities, ENG::Shader& shader)
	{
		auto& ts = entities.getPool<CS::Transform>();
		auto& ls = entities.getPool<CS::Light>();

		std::vector<ENG::EntityID> ents = entities.entitiesWith<CS::Transform, CS::Light>();
		for (std::size_t i = 0; i < ents.size(); i++)
		{
			shader.setUniform("lights[" + std::to_string(i) + "].position", ts[ents[i]].position);
			shader.setUniform("lights[" + std::to_string(i) + "].colour", ls[ents[i]].colour);
			shader.setUniform("lights[" + std::to_string(i) + "].radius", ls[ents[i]].radius);
		}
	}
}