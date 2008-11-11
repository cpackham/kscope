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
#include "application.h"
#include "mainwindow.h"
#include "actions.h"
#include "editorcontainer.h"
#include "querydialog.h"
#include "queryresultdock.h"
#include "editor.h"

namespace KScope
{

namespace App
{

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
	actions_ = new Actions(this);

	editCont_ = new EditorContainer(this);
	setCentralWidget(editCont_);

	queryDock_ = new QueryResultDock(tr("Query Results"), this);
	addDockWidget(Qt::RightDockWidgetArea, queryDock_);
	connect(queryDock_, SIGNAL(locationRequested(const Core::Location&)),
	        editCont_, SLOT(gotoLocation(const Core::Location&)));
	connect(theApp(), SIGNAL(hasProject(bool)), this,
	        SLOT(setProjectTitle(bool)));

	statusBar();

	actions_->setup();

	readSettings();
}

MainWindow::~MainWindow()
{
}

void MainWindow::promptQuery(Core::Query::Type type)
{
	QueryDialog dlg(type);
	Editor* editor;

	editor = editCont_->currentEditor();
	if (editor)
		dlg.setPattern(editor->currentText());

	if (dlg.exec() != QDialog::Accepted)
		return;

	queryDock_->query(Core::Query(dlg.type(), dlg.pattern()));
}

void MainWindow::promptCallGraph()
{
	QueryDialog dlg;
	Editor* editor;

	editor = editCont_->currentEditor();
	if (editor)
		dlg.setPattern(editor->currentText());

	if (dlg.exec() != QDialog::Accepted)
		return;
}

/**
 * Called before the main window closes.
 * @param  event  Information on the closing event
 */
void MainWindow::closeEvent(QCloseEvent* event)
{
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

void MainWindow::setProjectTitle(bool hasProject)
{
	QString title = theApp()->applicationName();

	if (hasProject)
		title += " - " + currentProject()->name();

	setWindowTitle(title);
}

}

}
