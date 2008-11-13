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

/**
 * Class constructor.
 * @param  argc  The number of command-line arguments
 * @param  argv  Command-line argument list
 */
Application::Application(int& argc, char** argv)
	: QApplication(argc, argv), proj_(NULL)
{
	QCoreApplication::setOrganizationName("elad_lahav@users.sourceforge.net");
	QCoreApplication::setApplicationName("KScope");
}

/**
 * Class destructor.
 */
Application::~Application()
{
}

/**
 * Starts the application.
 * Note that this method only returns when the application terminates.
 * @return The return code of the application.
 */
int Application::run()
{
	// Create the main window.
	// We can do it on the stack, as the method does not return as long as the
	// application is running.
	MainWindow mainWnd;
	mainWnd_ = &mainWnd;
	mainWnd_->show();

	// Do application initialisation.
	// The reason for posting an event is to have a running application (event
	// loop) before starting the initialisation process. This way, the process
	// can take full advantage of the event mechanism.
	postEvent(this, new QEvent(static_cast<QEvent::Type>(AppInitEvent)));

	return exec();
}

/**
 * Loads the given project.
 * @param  projPath  The project path
 */
void Application::loadProject(const QString& projPath)
{
	// Do not load if another project is currently loaded.
	if (proj_)
		return;

	// Create and open a project.
	try {
		proj_ = new Cscope::ManagedProject(projPath);
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

		return;
	}

	// Save the project path.
	QSettings().setValue("Session/LastProject", projPath);

	// Signal the availability of a project.
	emit hasProject(true);

	// Does the database need to be rebuilt?
	if ((proj_->engine()->status() == Core::Engine::Build)
	    || (proj_->engine()->status() == Core::Engine::Rebuild)) {
		mainWnd_->buildProject();
	}
}

/**
 * Closes the active project.
 */
void Application::closeProject()
{
	// Nothing to do if there is no active project.
	if (!proj_)
		return;

	// Close the project and delete the object.
	proj_->close();
	delete proj_;
	proj_ = NULL;

	// Signal that there is no active project.
	emit hasProject(false);
}

/**
 * Handles custom events.
 * @param  event  The event to handle.
 */
void Application::customEvent(QEvent* event)
{
	if (event->type() == static_cast<QEvent::Type>(AppInitEvent))
		init();
}

/**
 * Performs application initialisation, after the event loop was started.
 */
void Application::init()
{
	// Get the path of the last active project.
	QString lastProj = QSettings().value("Session/LastProject", "").toString();
	if (lastProj.isEmpty())
		return;

	// Get the project's name.
	QString name = Cscope::ManagedProject(lastProj).name();

	// Prompt the user for opening the last project.
	// TODO: Want more options on start-up (list of last projects, create new,
	// do nothing).
	if (QMessageBox::question(NULL, tr("Open Last Project"),
	                          tr("Would you like to reload '%1'?").arg(name),
	                          QMessageBox::Yes | QMessageBox::No)
	    == QMessageBox::Yes) {
		loadProject(lastProj);
	}
}

} // namespace App

} // namespace KScope
