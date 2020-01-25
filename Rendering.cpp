#include "Rendering.h"

namespace ENG
{
	/**
	* Draws model components.
	*/
	void drawModels(Entities& entities)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& models = entities.getPool<CS::Model>();

		for (EntityID entity : entities.entitiesWith<CS::Transform, CS::Model>())
		{
			CS::Model& m = models[entity];

			m.shader->setUniform("transform", transforms[entity].get());
			m.shader->bind();
			m.mesh->bind();
			m.texture->bind();
			glDrawArrays(GL_TRIANGLES, 0, m.mesh->vertexCount());
		}
	}
}