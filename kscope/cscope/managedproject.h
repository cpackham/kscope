/***************************************************************************
 *   Copyright (C) 2008 by Elad Lahav
 *   elad_lahav@users.sourceforge.net
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

#ifndef __CSCOPE_MANAGEDPROJECT_H
#define __CSCOPE_MANAGEDPROJECT_H

#include "project.h"
#include "crossref.h"
#include "files.h"

namespace KScope
{

namespace Cscope
{

/**
 * A managed Cscope project.
 * This is a managed project, since KScope has control over the code base, which
 * is kept as a cscope.files file.
 * @author Elad Lahav
 */
class ManagedProject : public Core::Project<Crossref, Files>
{
public:
	ManagedProject(const QString&);
	virtual ~ManagedProject();
};

}

}

#endif /* __CSCOPE_MANAGEDPROJECT_H */
