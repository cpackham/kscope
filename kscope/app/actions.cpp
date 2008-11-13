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

#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include "actions.h"
#include "mainwindow.h"
#include "application.h"
#include "editorcontainer.h"
#include "projectdialog.h"
#include "projectfilesdialog.h"

namespace KScope
{

namespace App
{

/**
 * Class constructor.
 * @param  Pointer to the main window
 */
Actions::Actions(MainWindow* parent) : QObject(parent)
{
}

/**
 * Class destructor.
 */
Actions::~Actions()
{
}

/**
 * Creates the actions.
 * Must be called after the main window's child widgets have been created.
 */
void Actions::setup()
{
	QMenu* menu;
	QAction* action;
	QActionGroup* group;

	// File menu.
	menu = mainWnd()->menuBar()->addMenu(tr("&File"));

	// Create a new file.
	action = new QAction(tr("&New"), this);
	action->setShortcut(tr("Ctrl+N"));
	action->setStatusTip(tr("New file"));
	connect(action, SIGNAL(triggered()), mainWnd()->editCont_,
	        SLOT(newFile()));
	menu->addAction(action);

	// Open an existing file.
	action = new QAction(tr("&Open"), this);
	action->setShortcut(tr("Ctrl+O"));
	action->setStatusTip(tr("Open existing file"));
	connect(action, SIGNAL(triggered()), mainWnd()->editCont_,
			SLOT(openFile()));
	menu->addAction(action);

	menu->addSeparator();

	// Exit the application.
	action = new QAction(tr("&Quit"), this);
	action->setShortcut(tr("Alt+F4"));
	action->setStatusTip(tr("Exit the application"));
	connect(action, SIGNAL(triggered()), mainWnd(), SLOT(close()));
	menu->addAction(action);

	// Project menu.
	menu = mainWnd()->menuBar()->addMenu(tr("&Project"));

	// Create a new project.
	action = new QAction(tr("&New..."), this);
	action->setStatusTip(tr("New project"));
	connect(action, SIGNAL(triggered()), this, SLOT(newProject()));
	menu->addAction(action);

	// Open an existing project.
	action = new QAction(tr("&Open..."), this);
	action->setShortcut(tr("Ctrl+P"));
	action->setStatusTip(tr("Open project"));
	connect(action, SIGNAL(triggered()), this, SLOT(openProject()));
	menu->addAction(action);

	// A group for project actions requiring an open project.
	// Only enabled when there is an active project.
	group = new QActionGroup(this);
	connect(theApp(), SIGNAL(hasProject(bool)), group, SLOT(setEnabled(bool)));
	group->setEnabled(false);

	menu->addSeparator();

	// Manage project files.
	action = new QAction(tr("&Files..."), this);
	action->setShortcut(tr("Ctrl+Shift+O"));
	action->setStatusTip(tr("Manage project files"));
	connect(action, SIGNAL(triggered()), this, SLOT(projectFiles()));
	menu->addAction(action);
	group->addAction(action);

	// View/edit project parameters.
	action = new QAction(tr("&Properties..."), this);
	action->setStatusTip(tr("View/edit project parameters"));
	connect(action, SIGNAL(triggered()), this, SLOT(projectProperties()));
	menu->addAction(action);
	group->addAction(action);

	// Build the project's database.
	action = new QAction(tr("&Build Database"), this);
	action->setShortcut(tr("Ctrl+B"));
	action->setStatusTip(tr("Build the project database"));
	connect(action, SIGNAL(triggered()), mainWnd(), SLOT(buildProject()));
	menu->addAction(action);
	group->addAction(action);

	// Query menu.
	menu = mainWnd()->menuBar()->addMenu(tr("&Query"));

	// A group for all query actions.
	// This group is enabled only when there is an active project.
	group = new QActionGroup(this);
	connect(group, SIGNAL(triggered(QAction*)), this, SLOT(query(QAction*)));
	connect(theApp(), SIGNAL(hasProject(bool)), group, SLOT(setEnabled(bool)));
	group->setEnabled(false);

	// Query references.
	action = new QAction(tr("&References"), this);
	action->setShortcut(tr("Ctrl+0"));
	action->setStatusTip(tr("Find all symbol references"));
	action->setData(Core::Query::References);
	menu->addAction(action);
	group->addAction(action);

	// Query definition.
	action = new QAction(tr("&Definition"), this);
	action->setShortcut(tr("Ctrl+1"));
	action->setStatusTip(tr("Find symbol definition"));
	action->setData(Core::Query::Definition);
	menu->addAction(action);
	group->addAction(action);

	// Query called functions.
	action = new QAction(tr("&Called Functions"), this);
	action->setShortcut(tr("Ctrl+2"));
	action->setStatusTip(tr("Show functions called from function"));
	action->setData(Core::Query::CalledFunctions);
	menu->addAction(action);
	group->addAction(action);

	// Query calling functions.
	action = new QAction(tr("C&alling Functions"), this);
	action->setShortcut(tr("Ctrl+3"));
	action->setStatusTip(tr("Find functions calling function"));
	action->setData(Core::Query::CallingFunctions);
	menu->addAction(action);
	group->addAction(action);

	// Query text.
	action = new QAction(tr("&Text"), this);
	action->setShortcut(tr("Ctrl+4"));
	action->setStatusTip(tr("Find text in files"));
	action->setData(Core::Query::Text);
	menu->addAction(action);
	group->addAction(action);

	// Find file.
	action = new QAction(tr("&File"), this);
	action->setShortcut(tr("Ctrl+7"));
	action->setStatusTip(tr("Find a file"));
	action->setData(Core::Query::FindFile);
	menu->addAction(action);
	group->addAction(action);

	// Find including files.
	action = new QAction(tr("&Including Files"), this);
	action->setShortcut(tr("Ctrl+7"));
	action->setStatusTip(tr("Find files #including a given file"));
	action->setData(Core::Query::IncludingFiles);
	menu->addAction(action);
	group->addAction(action);

	menu->addSeparator();

#if 0
	// Call graph.
	// TODO: Implement.
	action = new QAction(tr("Call &Graph"), this);
	action->setShortcut(tr("Ctrl+\\"));
	action->setStatusTip(tr("Show a call graph"));
	connect(action, SIGNAL(triggered()), mainWnd(), SLOT(promptCallGraph()));
	connect(theApp(), SIGNAL(hasProject(bool)), action, SLOT(setEnabled(bool)));
	menu->addAction(action);
#endif

	// Settings menu.
	menu = mainWnd()->menuBar()->addMenu(tr("&Settings"));

	action = new QAction(tr("Configure &Editor"), this);
	action->setStatusTip(tr("Modify editor settings"));
	connect(action, SIGNAL(triggered()), mainWnd()->editCont_,
	        SLOT(configEditor()));
	menu->addAction(action);

	wndMenu_ = mainWnd()->menuBar()->addMenu(tr("&Window"));
	connect(wndMenu_, SIGNAL(aboutToShow()), this, SLOT(showWindowMenu()));
}

void Actions::newProject()
{
	// If an active project exists, it needs to be closed first.
	if (theApp()->currentProject()) {
		QString msg = tr("The active project needs to be closed.\n"
                         "Would you like to close it now?");
		int result = QMessageBox::question(mainWnd(),
		                                   tr("Close Project"),
		                                   msg,
		                                   QMessageBox::Yes | QMessageBox::No);
		if (result == QMessageBox::No)
			return;

		theApp()->closeProject();
	}

	// Show the "New Project" dialogue.
	ProjectDialog dlg(mainWnd());
	if (dlg.exec() == QDialog::Accepted)
		theApp()->loadProject(dlg.path());
}

void Actions::openProject()
{
	// If an active project exists, it needs to be closed first.
	if (theApp()->currentProject()) {
		QString msg = tr("The active project needs to be closed.\n"
                         "Would you like to close it now?");
		int result = QMessageBox::question(mainWnd(),
		                                   tr("Close Project"),
		                                   msg,
		                                   QMessageBox::Yes | QMessageBox::No);
		if (result == QMessageBox::No)
			return;

		theApp()->closeProject();
	}

	// Show the "Open Project" dialogue.
	QString path = QFileDialog::getOpenFileName(mainWnd(), tr("Open Project"),
	                                            QString(), "cscope.proj");
	if (!path.isEmpty())
		theApp()->loadProject(path);
}

void Actions::projectFiles()
{
	ProjectFilesDialog dlg(mainWnd());
	dlg.exec();
}

void Actions::projectProperties()
{
	ProjectDialog dlg(mainWnd());
	dlg.exec();
}

void Actions::query(QAction* action)
{
	Core::Query::Type type;

	type = static_cast<Core::Query::Type>(action->data().toUInt());
	mainWnd()->promptQuery(type);
}

void Actions::showWindowMenu()
{
	wndMenu_->clear();
	mainWnd()->editCont_->populateWindowMenu(wndMenu_);
}

MainWindow* Actions::mainWnd()
{
	return static_cast<MainWindow*>(parent());
}

} // namespace App

} // namespace KScope
