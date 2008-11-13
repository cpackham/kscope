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
#include "projectconfig.h"
#include "crossref.h"
#include "files.h"
#include "configwidget.h"

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

namespace Core
{

template<>
struct ProjectConfig<Cscope::ManagedProject>
{
	static QWidget* createConfigWidget(Cscope::ManagedProject* project,
	                                   QWidget* parent) {
		Cscope::ConfigWidget* widget = new Cscope::ConfigWidget(parent);

		if (project) {
			ProjectBase::Params params;
			project->getCurrentParams(params);

			QStringList args = params.engineString_.split(":");
			widget->kernelCheck_->setChecked(args.contains("-k"));
			widget->invIndexCheck_->setChecked(args.contains("-q"));
			widget->compressCheck_->setChecked(!args.contains("-c"));
		}
		else {
			widget->kernelCheck_->setChecked(false);
			widget->invIndexCheck_->setChecked(true);
			widget->compressCheck_->setChecked(true);
		}

		return widget;
	}

	static void paramsFromWidget(QWidget* widget, ProjectBase::Params& params) {
		params.engineString_ = params.projPath_;
		params.codebaseString_ = params.projPath_;

		Cscope::ConfigWidget* confWidget
			= dynamic_cast<Cscope::ConfigWidget*>(widget);
		if (confWidget) {
			if (confWidget->kernelCheck_->isChecked())
				params.engineString_ += ":-k";
			if (confWidget->invIndexCheck_->isChecked())
				params.engineString_ += ":-q";
			if (!confWidget->compressCheck_->isChecked())
				params.engineString_ += ":-c";
		}
	}
};

}

}

#endif // __CSCOPE_MANAGEDPROJECT_H
