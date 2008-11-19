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

#include <QDockWidget>
#include <QSettings>
#include <QCloseEvent>
#include <QStatusBar>
#include "mainwindow.h"
#include "editorcontainer.h"
#include "querydialog.h"
#include "queryresultdock.h"
#include "editor.h"
#include "projectmanager.h"

namespace KScope
{

namespace App
{

/**
 * Class constructor.
 */
MainWindow::MainWindow() : QMainWindow(), actions_(this)
{
	// Set the window title.
	// This changes whenever a project is opened/closed.
	setProjectTitle(false);
	connect(ProjectManager::signalProxy(), SIGNAL(hasProject(bool)), this,
	        SLOT(setProjectTitle(bool)));

	// Create the central widget (the editor manager).
	editCont_ = new EditorContainer(this);
	setCentralWidget(editCont_);

	// Create the query result dock.
	queryDock_ = new QueryResultDock(this);
	addDockWidget(Qt::RightDockWidgetArea, queryDock_);
	connect(queryDock_, SIGNAL(locationRequested(const Core::Location&)),
	        editCont_, SLOT(gotoLocation(const Core::Location&)));

	// Create a status bar.
	statusBar();

	// Initialise actions.
	// The order is important: make sure the child widgets are created BEFORE
	// calling setup().
	actions_.setup();

	// Apply saved window settings.
	readSettings();

	// Rebuild the project when signalled by the project manager.
	connect(ProjectManager::signalProxy(), SIGNAL(buildProject()), this,
	        SLOT(buildProject()));
}

/**
 * Class destrutor.
 */
MainWindow::~MainWindow()
{
}

/**
 * Prompts the user for query information, and starts a query with the entered
 * parameters.
 * @param  type  The default query type to use
 */
void MainWindow::promptQuery(Core::Query::Type type)
{
	QueryDialog dlg(type);

	// Get the default pattern from the text under the cursor on the active
	// editor (if any).
	Editor* editor = editCont_->currentEditor();
	if (editor)
		dlg.setPattern(editor->currentSymbol());

	// Prompt the user.
	if (dlg.exec() != QDialog::Accepted)
		return;

	// Start a query with results shown in a view inside the query dock.
	queryDock_->query(Core::Query(dlg.type(), dlg.pattern()));
}

/**
 * Starts a build process for the current project's engine.
 * Provides progress information in either a modal dialogue or a progress-bar
 * in the window's status bar. The modal dialogue is used for initial builds,
 * while the progress-bar is used for rebuilds.
 */
void MainWindow::buildProject()
{
	try {
		// Create a build progress widget.
		if (ProjectManager::engine().status() == Core::Engine::Build) {
			buildProgress_.init(true, this);
		}
		else {
			QWidget* widget = buildProgress_.init(false, this);
			statusBar()->addWidget(widget);
		}

		// Start the build process.
		ProjectManager::engine().build(buildProgress_);
	}
	catch (Core::Exception* e) {
		e->showMessage();
		delete e;
	}
}

/**
 * Called before the main window closes.
 * @param  event  Information on the closing event
 */
void MainWindow::closeEvent(QCloseEvent* event)
{
	// TODO: Check all editors for unsaved changes.

	writeSettings();
	event->accept();
}

/**
 * Stores configuration in the settings file.
 */
void MainWindow::writeSettings()
{
	QSettings settings;

	// Store main window position and size.
	settings.beginGroup("MainWindow");
	settings.setValue("size", size());
	settings.setValue("pos", pos());
	settings.setValue("state", saveState());
	settings.endGroup();

	// Store project settings.
	settings.beginGroup("Project");

	settings.endGroup();
}

/**
 * Loads configuration from the settings file.
 */
void MainWindow::readSettings()
{
	QSettings settings;

	// Restore main window position and size.
	settings.beginGroup("MainWindow");
	resize(settings.value("size", QSize(1000, 600)).toSize());
	move(settings.value("pos", QPoint(200, 200)).toPoint());
	restoreState(settings.value("state").toByteArray());
	settings.endGroup();
}

/**
 * Changes the window title to reflect the availability of a project.
 * @param  hasProject  true if a project is currently open, false otherwise
 */
void MainWindow::setProjectTitle(bool hasProject)
{
	(void)hasProject;

	QString title = qApp->applicationName();
	if (ProjectManager::project())
		title += " - " + ProjectManager::project()->name();

	setWindowTitle(title);
}

} // namespace App

} // namespace KScope
