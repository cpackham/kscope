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

#ifndef __APP_MAINWINDOW_H
#define __APP_MAINWINDOW_H

#include <QMainWindow>
#include "actions.h"
#include "globals.h"
#include "buildprogress.h"

class QCloseEvent;

namespace KScope
{

namespace App
{

class EditorContainer;
class QueryResultDock;

/**
 * KScope's main window.
 * This is an MDI-style main window. The central area is used for viewing and
 * editing source files, while docks are used for browsing information (e.g.,
 * engine query results).
 * @author Elad Lahav
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();

public slots:
	void promptQuery(Core::Query::Type type = Core::Query::References);
	void quickDefinition();
	void promptCallTree();
	void buildProject();

protected:
	virtual void closeEvent(QCloseEvent*);

private:
	friend class Actions;

	/**
	 * Main menu commands.
	 */
	Actions actions_;

	/**
	 * Manages the editor widgets.
	 */
	EditorContainer* editCont_;

	/**
	 * Holds query result views in a docked widget.
	 */
	QueryResultDock* queryDock_;

	/**
	 * Provides progress information on an engine's build process.
	 */
	BuildProgress buildProgress_;

	void readSettings();
	void writeSettings();

private slots:
	void setProjectTitle(bool);
};

} // namespace App

} // namespace KScope

#endif // __APP_MAINWINDOW_H
