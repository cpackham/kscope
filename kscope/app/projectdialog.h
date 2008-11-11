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

#ifndef __KSCOPE_PROJECTDIALOG_H__
#define __KSCOPE_PROJECTDIALOG_H__

#include <QDialog>
#include "ui_projectdialog.h"

namespace KScope
{

namespace App
{

/**
 * Allows the configuration of project properties.
 * Used either to create a new project, or to modify an existing one.
 * @author Elad Lahav
 */
class ProjectDialog : public QDialog, public Ui::ProjectDialog
{
	Q_OBJECT

public:
	ProjectDialog(QWidget* parent = NULL);
	~ProjectDialog();

	QString path() { return projectPathEdit_->text(); }

protected slots:
	void browseRootPath();
	void browseProjectPath();
	void setUseHiddenProjectDir(bool);
	void updateProjectPath(const QString&);
};

}

}

#endif // __KSCOPE_PROJECTDIALOG_H__
