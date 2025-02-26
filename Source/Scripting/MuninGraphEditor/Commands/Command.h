#pragma once

struct Command
{
	Command() = default;
	virtual ~Command() = default;
	size_t aId;
	virtual bool Undo() = 0;
	virtual bool Redo() = 0;
	virtual bool Clean() = 0;
};