#pragma once

#include "Interaction/Commands/Command.h"

#include <memory>
#include <vector>

class CommandManager {
public:
    void execute(std::unique_ptr<Command> command);
    bool canUndo() const;
    bool canRedo() const;
    bool undo();
    bool redo();

private:
    std::vector<std::unique_ptr<Command>> m_undoStack;
    std::vector<std::unique_ptr<Command>> m_redoStack;
};
