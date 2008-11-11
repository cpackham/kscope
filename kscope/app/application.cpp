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

#include <QMessageBox>
#include "application.h"
#include "mainwindow.h"
#include "managedproject.h"
#include "exception.h"

namespace KScope
{

namespace App
{

Application::Application(int& argc, char** argv)
	: QApplication(argc, argv), proj_(NULL)
{
	QCoreApplication::setOrganizationName("elad_lahav@users.sourceforge.net");
	QCoreApplication::setApplicationName("KScope");
}

Application::~Application()
{
}

int Application::run()
{
	MainWindow mainWnd;

	mainWnd_ = &mainWnd;
	mainWnd_->show();

	postEvent(this, new QEvent(static_cast<QEvent::Type>(LoadLastProject)));
	return exec();
}

bool Application::loadProject(const QString& projString)
{
	if (proj_)
		return false;

	try {
		proj_ = new Cscope::ManagedProject(projString);
		proj_->open();
	}
	catch (Core::Exception* e) {
		QMessageBox::warning(mainWnd_, tr("Failed to load project"),
		                     e->reason());
		delete e;
		if (proj_) {
			delete proj_;
			proj_ = NULL;
		}

		return false;
	}

	QSettings().setValue("Session/LastProject", projString);

	emit hasProject(true);
	return true;
}

void Application::closeProject()
{
	if (!proj_)
		return;

	proj_->close();
	delete proj_;
	proj_ = NULL;

	emit hasProject(false);
}

void Application::customEvent(QEvent* event)
{
	if (event->type() != static_cast<QEvent::Type>(LoadLastProject))
		return;

	QSettings settings;
	QString lastProj, name;

	lastProj = settings.value("Session/LastProject", "").toString();
	if (lastProj.isEmpty())
		return;

	Cscope::ManagedProject proj(lastProj);
	name = proj.name();

	if (QMessageBox::question(NULL, tr("Open Last Project"),
	                          tr("Would you like to reload '%1'?").arg(name),
	                          QMessageBox::Yes | QMessageBox::No)
	    == QMessageBox::Yes) {
		loadProject(lastProj);
	}
}

}

}
