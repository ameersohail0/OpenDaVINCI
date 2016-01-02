/**
 * hesperia - Simulation environment
 * Copyright (C) 2008 - 2015 Christian Berger, Bernhard Rumpe
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef HESPERIA_SCENEGRAPH_SCENENODEVISITOR_H_
#define HESPERIA_SCENEGRAPH_SCENENODEVISITOR_H_

// native.h must be included as first header file for definition of _WIN32_WINNT.
#include "core/opendavinci.h"

namespace hesperia {
    namespace scenegraph {

        // Forward declaration for SceneNode.
        class SceneNode;

        /**
         * Interface for all SceneNodeVisitors.
         */
        class OPENDAVINCI_API SceneNodeVisitor {
            public:
                virtual ~SceneNodeVisitor();

                /**
                 * This method is called from a visited scene node.
                 *
                 * @param sceneNode Scene node to visit.
                 */
                virtual void visit(SceneNode *sceneNode) = 0;
        };

    }
} // hesperia::scenegraph

#endif /*HESPERIA_SCENEGRAPH_SCENENODEVISITOR_H_*/
