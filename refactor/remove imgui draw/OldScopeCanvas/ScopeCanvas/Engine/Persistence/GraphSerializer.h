#pragma once

#include "Engine/DiagramModel.h"

#include <string>

class GraphSerializer {
public:
	static bool save(const DiagramModel &model, const std::string &filepath);
	static bool load(DiagramModel &model, const std::string &filepath);
};
