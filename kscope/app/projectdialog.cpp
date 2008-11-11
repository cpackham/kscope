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

#include <QFileDialog>
#include "projectdialog.h"
#include "application.h"

namespace KScope
{

namespace App
{

ProjectDialog::ProjectDialog(QWidget* parent) : QDialog(parent),
                                                Ui::ProjectDialog()
{
	setupUi(this);

	Core::ProjectBase* project = theApp()->currentProject();
	if (project) {
		setWindowTitle(tr("Project Properties"));
		nameEdit_->setText(project->name());
		rootPathWidget_->setEnabled(false);
		projectPathWidget_->setEnabled(false);
	}
	else {
		setWindowTitle(tr("New Project"));
	}
}

ProjectDialog::~ProjectDialog()
{
}

void ProjectDialog::browseRootPath()
{
	QString dir = QFileDialog::getExistingDirectory(this,
	                                                tr("Select Directory"));
	if (!dir.isEmpty())
		rootPathEdit_->setText(dir);
}

void ProjectDialog::browseProjectPath()
{
	QString dir = QFileDialog::getExistingDirectory(this,
	                                                tr("Select Directory"));
	if (!dir.isEmpty())
		projectPathEdit_->setText(dir);
}

void ProjectDialog::setUseHiddenProjectDir(bool enable)
{
	if (enable) {
		connect(rootPathEdit_, SIGNAL(textChanged(const QString&)),
		        this, SLOT(updateProjectPath(const QString&)));
		updateProjectPath(rootPathEdit_->text());
	}
	else {
		disconnect(rootPathEdit_, SIGNAL(textChanged(const QString&)),
		           this, SLOT(updateProjectPath(const QString&)));
	}
}

void ProjectDialog::updateProjectPath(const QString& rootPath)
{
	QString path;
	if (!rootPath.endsWith("/"))
		path = rootPath + "/" + ".kscope";
	else
		path = rootPath + ".kscope";

	projectPathEdit_->setText(path);
}

}

}
