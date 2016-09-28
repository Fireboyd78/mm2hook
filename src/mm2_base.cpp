#include "mm2_base.h"
using namespace MM2;

/*
    Base
*/
MM2FnHook<void>     Base::$$dtor                ( NULL, NULL, 0x4C8200 );

/*
    asCullable
*/
MM2FnHook<void>     asCullable::$$dtor          ( NULL, NULL, 0x460EA0 );
MM2FnHook<void>     asCullable::$Cull           ( NULL, NULL, 0x4A3440 );

/*
    asNode
*/
MM2FnHook<void>     asNode::$FileIO             ( NULL, NULL, 0x403330 );
MM2FnHook<void>     asNode::$AfterLoad          ( NULL, NULL, 0x403340 );
MM2FnHook<void>     asNode::$BeforeSave         ( NULL, NULL, 0x403350 );

MM2FnHook<char *>   asNode::$GetClassNameA      ( NULL, NULL, 0x403360 );

MM2FnHook<void>     asNode::$$ctor              ( NULL, NULL, 0x4A0CE0 );
MM2FnHook<void>     asNode::$$dtor              ( NULL, NULL, 0x4A0D00 );

MM2FnHook<void>     asNode::$SetName            ( NULL, NULL, 0x4A0D70 );

MM2FnHook<const char *>
                    asNode::$GetDirName         ( NULL, NULL, 0x4A0DA0 );

MM2FnHook<void>     asNode::$Update             ( NULL, NULL, 0x4A0DB0 );
MM2FnHook<void>     asNode::$UpdatePaused       ( NULL, NULL, 0x4A0DD0 );
MM2FnHook<void>     asNode::$Reset              ( NULL, NULL, 0x4A0E10 );
MM2FnHook<void>     asNode::$ResChange          ( NULL, NULL, 0x4A0E30 );

MM2FnHook<int>      asNode::$AddChild           ( NULL, NULL, 0x4A0E60 );
MM2FnHook<int>      asNode::$InsertChild        ( NULL, NULL, 0x4A0EE0 );
MM2FnHook<int>      asNode::$RemoveChild_$1     ( NULL, NULL, 0x4A0F50 );
MM2FnHook<int>      asNode::$RemoveChild_$2     ( NULL, NULL, 0x4A0FD0 );
MM2FnHook<void>     asNode::$RemoveAllChildren  ( NULL, NULL, 0x4A1010 );
MM2FnHook<asNode *> asNode::$GetChild           ( NULL, NULL, 0x4A1030 );
MM2FnHook<asNode *> asNode::$GetNext            ( NULL, NULL, 0x4A1060 );
MM2FnHook<asNode *> asNode::$GetLastChild       ( NULL, NULL, 0x4A1070 );
MM2FnHook<int>      asNode::$NumChildren        ( NULL, NULL, 0x4A10A0 );
MM2FnHook<void>     asNode::$SwitchTo           ( NULL, NULL, 0x4A10C0 );

MM2FnHook<bool>     asNode::$Load               ( NULL, NULL, 0x4A1120 );
MM2FnHook<bool>     asNode::$Save               ( NULL, NULL, 0x4A11D0 );