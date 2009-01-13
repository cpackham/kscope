/***************************************************************************
 *   Copyright (C) 2007-2009 by Elad Lahav
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
#include "queryresultdialog.h"
#include "session.h"
#include "managedproject.h"

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
	setWindowTitle(false);

	// The main window icon.
	setWindowIcon(QIcon(":/images/kscope"));

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

	// Perform actions when a project is opened or closed.
	connect(ProjectManager::signalProxy(), SIGNAL(hasProject(bool)), this,
	        SLOT(projectOpenedClosed(bool)));

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
 * Terminates the current session.
 * Closes all editor windows, and saves the session (if it is part of a
 * project).
 * @return true if the session was closed, false if the user aborts the
 *         operation (e.g., chooses to cancel when prompted to save a modified
 *         editor)
 */
bool MainWindow::closeSession()
{
	// Check all editors for unsaved changes.
	if (!editCont_->canClose())
		return false;

	if (ProjectManager::hasProject()) {
		// Get tne project path.
		QString projPath = ProjectManager::project()->path();

		// Store session information.
		Session session(QDir(projPath).filePath("session.conf"));
		editCont_->saveSession(session);
		session.save();
	}

	// Close open editor windows.
	editCont_->closeAll();
	return true;
}

/**
 * Prompts the user for query information, and starts a query with the entered
 * parameters.
 * @param  type  The default query type to use
 */
void MainWindow::promptQuery(Core::Query::Type type)
{
	QueryDialog dlg(type, this);

	// Get the default pattern from the text under the cursor on the active
	// editor (if any).
	Editor* editor = editCont_->currentEditor();
	if (editor)
		dlg.setPattern(editor->currentSymbol());

	// Prompt the user.
	if (dlg.exec() != QDialog::Accepted)
		return;

	// Start a query with results shown in a view inside the query dock.
	queryDock_->query(Core::Query(dlg.type(), dlg.pattern()), false);
}

/**
 * Implements a definition query that does not display its results in the
 * query dock.
 * If the query results in a single location, this location is immediately
 * displayed in the editor container. Otherwise, the user is prompted for the
 * location, using a results dialogue.
 */
void MainWindow::quickDefinition()
{
	QString symbol;

	// Get the default pattern from the text under the cursor on the active
	// editor (if any).
	Editor* editor = editCont_->currentEditor();
	if (editor)
		symbol = editor->currentSymbol();

	// Prompt for a symbol, if none is selected in the current editor.
	if (symbol.isEmpty()) {
		// Create the query dialogue.
		QueryDialog::TypeList typeList;
		typeList << Core::Query::Definition;
		QueryDialog dlg(typeList, Core::Query::Definition, this);

		// Prompt for a symbol.
		if (dlg.exec() != QDialog::Accepted)
			return;

		// Get the symbol from the dialogue.
		symbol = dlg.pattern();
		if (symbol.isEmpty())
			return;
	}

	// Create a query view dialogue.
	QueryResultDialog* dlg = new QueryResultDialog(this);
	dlg->setModal(true);

	// Automatically select a single result.
	Core::QueryView* view = dlg->view();
	view->setAutoSelectSingleResult(true);

	// Forward selected locations to the editor container.
	connect(view, SIGNAL(locationRequested(const Core::Location&)),
	        editCont_, SLOT(gotoLocation(const Core::Location&)));

	// Only show the dialogue if needed.
	connect(view, SIGNAL(needToShow()), dlg, SLOT(show()));

	try {
		// Run the query.
		view->model()->setRootPath(ProjectManager::project()->rootPath());
		view->model()->setColumns(ProjectManager::engine()
			.queryFields(Core::Query::Definition));
		ProjectManager::engine().query(view,
		                               Core::Query(Core::Query::Definition,
		                                           symbol));
	}
	catch (Core::Exception* e) {
		e->showMessage();
		delete e;
	}
}

void MainWindow::promptCallTree()
{
	// Create the query dialogue.
	QueryDialog::TypeList typeList;
	typeList << Core::Query::CalledFunctions << Core::Query::CallingFunctions;
	QueryDialog dlg(typeList, Core::Query::CalledFunctions, this);
	dlg.setWindowTitle(tr("Call Tree"));

	// Get the default pattern from the text under the cursor on the active
	// editor (if any).
	Editor* editor = editCont_->currentEditor();
	if (editor)
		dlg.setPattern(editor->currentSymbol());

	// Prompt the user.
	if (dlg.exec() != QDialog::Accepted)
		return;

	// Start a query with results shown in a view inside the query dock.
	queryDock_->query(Core::Query(dlg.type(), dlg.pattern()), true);
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
		ProjectManager::engine().build(&buildProgress_);
	}
	catch (Core::Exception* e) {
		e->showMessage();
		delete e;
	}
}

/**
 * Opens the given file for editing.
 * @param  path  The path of the requested file
 */
void MainWindow::openFile(const QString& path)
{
	editCont_->openFile(path);
}

/**
 * Called before the main window closes.
 * @param  event  Information on the closing event
 */
void MainWindow::closeEvent(QCloseEvent* event)
{
	if (!closeSession()) {
		event->ignore();
		return;
	}

	// Save main-window geometry.
	writeSettings();

	// Close the main window and terminate the application.
	event->accept();
}

/**
 * Stores configuration in the settings file.
 */
void MainWindow::writeSettings()
{
	// Store main window position and size.
	QSettings settings;
	settings.beginGroup("MainWindow");
	settings.setValue("size", size());
	settings.setValue("pos", pos());
	settings.setValue("state", saveState());
	settings.endGroup();
}

/**
 * Loads configuration from the settings file.
 */
void MainWindow::readSettings()
{
	// Restore main window position and size.
	QSettings settings;
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
void MainWindow::setWindowTitle(bool hasProject)
{
	QString title = qApp->applicationName();
	if (hasProject)
		title += " - " + ProjectManager::project()->name();

	QMainWindow::setWindowTitle(title);
}

/**
 * Performs actions following the opening or closing of a project.
 * This slot is connected to the hasProject() signal emitted by the project
 * manager.
 * @param  opened  true if a project was opened, false if a project was closed
 */
void MainWindow::projectOpenedClosed(bool opened)
{
	// Adjust the window title.
	setWindowTitle(opened);

	// Nothing else to to if a project was closed.
	if (!opened)
		return;

	// Get the project path.
	QString projPath = ProjectManager::project()->path();

	// Restore the session.
	Session session(QDir(projPath).filePath("session.conf"));
	session.load();
	editCont_->loadSession(session);

	try {
		// Show the project files dialogue if files need to be added to the
		// project.
		if (ProjectManager::codebase().needFiles())
			actions_.projectFiles();
	}
	catch (Core::Exception* e) {
		e->showMessage();
		delete e;
	}
}

} // namespace App

} // namespace KScope
