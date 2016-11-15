#ifndef _T_PANE_SCENE_NODE_H_
#define _T_PANE_SCENE_NODE_H_

#include "RSceneNode.h"

namespace rs3 {

class TPaneSceneNode : public RSceneNode
{
public:
	// Pane Maneuver Quantifier
	enum PANE_MANEUVER
	{
		E_BILLBOARD_MANEUVER = 0,
		E_TOTAL_PANE_MANEUVER
	};
};


}

#endif