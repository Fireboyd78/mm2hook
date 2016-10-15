#include "mm2_base.h"
using namespace MM2;

/*
    Base
*/
DECLARE_HOOK(Base::$$dtor);

/*
    asCullable
*/
DECLARE_HOOK(asCullable::$$dtor);
DECLARE_HOOK(asCullable::$Cull);

/*
    asNode
*/
DECLARE_HOOK(asNode::$FileIO);
DECLARE_HOOK(asNode::$AfterLoad);
DECLARE_HOOK(asNode::$BeforeSave);

DECLARE_HOOK(asNode::$GetClassNameA);

DECLARE_HOOK(asNode::$$ctor);
DECLARE_HOOK(asNode::$$dtor);

DECLARE_HOOK(asNode::$SetName);

DECLARE_HOOK(asNode::$GetDirName);

DECLARE_HOOK(asNode::$Update);
DECLARE_HOOK(asNode::$UpdatePaused);
DECLARE_HOOK(asNode::$Reset);
DECLARE_HOOK(asNode::$ResChange);

DECLARE_HOOK(asNode::$AddChild);
DECLARE_HOOK(asNode::$InsertChild);
DECLARE_HOOK(asNode::$RemoveChild_$1);
DECLARE_HOOK(asNode::$RemoveChild_$2);
DECLARE_HOOK(asNode::$RemoveAllChildren);
DECLARE_HOOK(asNode::$GetChild);
DECLARE_HOOK(asNode::$GetNext);
DECLARE_HOOK(asNode::$GetLastChild);
DECLARE_HOOK(asNode::$NumChildren);
DECLARE_HOOK(asNode::$SwitchTo);

DECLARE_HOOK(asNode::$Load);
DECLARE_HOOK(asNode::$Save);