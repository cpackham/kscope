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

#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include "engine.h"

class QCloseEvent;

namespace KScope
{

namespace App
{

class Actions;
class EditorContainer;
class QueryResultDock;

/**
 * @author Elad Lahav
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = 0);
	~MainWindow();

public slots:
	void promptQuery(Core::Query::Type type = Core::Query::References);
	void promptCallGraph();

protected:
	virtual void closeEvent(QCloseEvent*);

private:
	friend class Actions;
	Actions* actions_;
	EditorContainer* editCont_;
	QueryResultDock* queryDock_;

	void readSettings();
	void writeSettings();

private slots:
	void setProjectTitle(bool);
};

}

}

#endif // __MAINWINDOW_H
