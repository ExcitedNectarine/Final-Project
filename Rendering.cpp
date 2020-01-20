#include "Rendering.h"

namespace ENG
{
	/**
	* Draws model components.
	*/
	void drawModels(ENG::Entities& entities)
	{
		auto& ts = entities.getPool<CS::Transform>();
		auto& ms = entities.getPool<CS::Model>();

		for (ENG::EntityID entity : entities.entitiesWith<CS::Transform, CS::Model>())
		{
			CS::Model& m = ms[entity];

			m.shader->setUniform("transform", ts[entity].get());
			m.shader->bind();
			m.mesh->bind();
			m.texture->bind();
			glDrawArrays(GL_TRIANGLES, 0, m.mesh->vertexCount());
		}
	}
}