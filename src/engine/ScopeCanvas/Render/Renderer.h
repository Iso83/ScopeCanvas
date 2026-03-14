#pragma once

#include "Engine/DiagramModel.h"
#include "Render/EdgeRenderer.h"
#include "Render/GridRenderer.h"
#include "Render/NodeRenderer.h"
#include "Render/SelectionRectRenderer.h"
#include "View/Camera2D.h"
#include "View/Viewport.h"

using GraphDocument = DiagramModel;

class Renderer {
public:
	Renderer();

	bool init(
		int viewportWidth,
		int viewportHeight);
	void resize(int width, int height);
	void render(const GraphDocument &model,
		const GraphView &view,
		const Viewport &viewport,
		bool gridEnabled,
		float gridCellSize,
		bool renderEdges = true);

	Camera2D &camera() { return m_camera; }
	int viewportWidth() const { return m_viewportWidth; }
	int viewportHeight() const { return m_viewportHeight; }

private:
	Camera2D m_camera;
	GridRenderer m_gridRenderer;
	EdgeRenderer m_edgeRenderer;
	NodeRenderer m_nodeRenderer;
	SelectionRectRenderer m_selectionRectRenderer;
	int m_viewportWidth;
	int m_viewportHeight;
};
