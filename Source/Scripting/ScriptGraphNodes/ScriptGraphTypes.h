#pragma once
#include "../../Utilities/CommonUtilities/Input.h"
class GameObject;
#include "Types/TypeRegistry.h"

DECLARE_TYPEGROUP(ScriptGraph);

//~ Internal type to handle Owner variable on ScriptGraphs.
DECLARE_REGISTERED_TYPE(void*);
//~

DECLARE_REGISTERED_TYPE(float);
DECLARE_REGISTERED_TYPE(std::string);
DECLARE_REGISTERED_TYPE(CommonUtilities::Vector3<float>);
DECLARE_REGISTERED_TYPE(bool)
DECLARE_REGISTERED_TYPE(int)
DECLARE_REGISTERED_TYPE(unsigned)
DECLARE_REGISTERED_TYPE(CommonUtilities::Keys);
DECLARE_REGISTERED_TYPE(GameObject*)