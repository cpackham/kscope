/***************************************************************************
 *   Copyright (C) 2007-2008 by Elad Lahav
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

#include "projectmanager.h"
#include "exception.h"

namespace KScope
{

namespace App
{

Core::ProjectBase* ProjectManager::proj_ = NULL;
ProjectManagerSignals ProjectManager::signals_;

const Core::ProjectBase* ProjectManager::project()
{
	return proj_;
}

Core::Engine& ProjectManager::engine()
{
	if (proj_ == NULL)
		throw Core::Exception("No project is currently loaded");

	Core::Engine* engine = proj_->engine();
	if (engine == NULL)
		throw Core::Exception("No engine is available");

	return *engine;
}

Core::Codebase& ProjectManager::codebase()
{
	if (proj_ == NULL)
		throw Core::Exception("No project is currently loaded");

	Core::Codebase* cbase = proj_->codebase();
	if (cbase == NULL)
		throw Core::Exception("No engine is available");

	return *cbase;
}

const ProjectManagerSignals* ProjectManager::signalProxy()
{
	return &signals_;
}

void ProjectManager::close()
{
		if (!proj_)
			return;

		proj_->close();
		delete proj_;
		proj_ = NULL;

		signals_.emitHasProject(false);
}

} // namespace App

} // namespace KScope
