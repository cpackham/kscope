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

#include <QFileDialog>
#include <QDebug>
#include "application.h"
#include "editorcontainer.h"
#include "editorconfigdialog.h"
#include "queryresultdialog.h"

namespace KScope
{

namespace App
{

/**
 * Class constructor.
 * @param  parent  Parent widget
 */
EditorContainer::EditorContainer(QWidget* parent)
	: QMdiArea(parent),
	  currentWindow_(NULL),
	  newFileIndex_(1),
	  blockWindowActivation_(false)
{
	// Load editor configuration settings.
	Settings& settings = Application::settings();
	settings.beginGroup("Editor");
	config_.load(settings);
	settings.endGroup();

	// Notify when an active editor is available.
	connect(this, SIGNAL(subWindowActivated(QMdiSubWindow*)), this,
	        SLOT(windowActivated(QMdiSubWindow*)));
}

/**
 * Class destructor.
 */
EditorContainer::~EditorContainer()
{
}

/**
 * Fills the "Window" menu with a list of all open sub-window titles.
 * @param  wndMenu  The menu to populate
 */
void EditorContainer::populateWindowMenu(QMenu* wndMenu) const
{
	// Add an entry for each open sub-window.
	QMap<QString, QMdiSubWindow*>::ConstIterator itr;
	for (itr = fileMap_.begin(); itr != fileMap_.end(); ++itr)
		wndMenu->addAction(itr.key());

	// Activate a sub-window when its menu entry is selected.
	connect(wndMenu, SIGNAL(triggered(QAction*)), this,
	        SLOT(handleWindowAction(QAction*)));
}

/**
 * Checks for any unsaved-changes in the currently open editors.
 * @return true if the application can terminate, false if the user cancels
 *         the operation due to unsaved changes
 */
bool EditorContainer::canClose()
{
	// TODO: Saving a file may take too long (e.g., for NFS-mounted files).
	// In this case, the application should not terminate until the file has
	// been saved. The current behaviour may lead to data loss!

	// Iterate over all editor windows.
	foreach (QMdiSubWindow* window, fileMap_) {
		Editor* editor = static_cast<Editor*>(window->widget());
		if (!editor->canClose())
			return false;
	}

	return true;
}

/**
 * Stores the locations of all editors in a session object.
 * @param  session  The session object to use
 */
void EditorContainer::saveSession(Session& session)
{
	Core::LocationList locList;

	// Create a list of locations for the open editors.
	foreach (QMdiSubWindow* window, fileMap_) {
		Editor* editor = static_cast<Editor*>(window->widget());

		Core::Location loc;
		editor->getCurrentLocation(loc);
		locList.append(loc);
	}

	session.setEditorList(locList);

	// Store the path of the currently active editor.
	if (currentEditor())
		session.setActiveEditor(currentEditor()->path());

	// Store the state of the active window.
	QMdiSubWindow* window = currentSubWindow();
	session.setMaxActiveEditor(window ? window->isMaximized() : false);
}

/**
 * Opens editors based on the locations stored in a session object.
 * @param  session  The session object to use
 */
void EditorContainer::loadSession(Session& session)
{
	const Core::LocationList& locList = session.editorList();
	Core::LocationList::ConstIterator itr;

	// Do not handle changes to the active editor while loading.
	blockWindowActivation_ = true;

	// Open an editor for each location.
	for (itr = locList.begin(); itr != locList.end(); ++itr)
		(void)getEditor(*itr, false);

	// Re-enable handling of changes to active windows.
	blockWindowActivation_ = false;

	// Activate the previously-active editor.
	// We have to call windowActivated() explicitly, in the case the active
	// window is the last one to be loaded. In that case, the signal will not
	// be emitted.
	QString activeEditor = session.activeEditor();
	QMdiSubWindow* window;
	if (!activeEditor.isEmpty()) {
		Core::Location loc(activeEditor);
		window = getEditor(loc, true);
	}
	else {
		// No active window was set, choose the current one.
		window = currentSubWindow();
		windowActivated(window);
	}

	// Maximise the active window, if required.
	if (session.maxActiveEditor())
		window->showMaximized();
}

/**
 * Creates an editor window with an empty, unnamed file.
 */
void EditorContainer::newFile()
{
	(void)getEditor(Core::Location(), true);
}

/**
 * Prompts the user for a file name, and creates an editor window for editing
 * the selected file.
 */
void EditorContainer::openFile()
{
	QString path = QFileDialog::getOpenFileName(0, tr("Open File"));
	if (!path.isEmpty())
		(void)getEditor(Core::Location(path), true);
}

/**
 * Creates an editor window for editing the given file.
 * @param  path  The path of the file to edit
 */
void EditorContainer::openFile(const QString& path)
{
	if (!path.isEmpty())
		(void)getEditor(Core::Location(path), true);
}

/**
 * Displays the editor configuration dialogue.
 * Any changes to the configuration are then applied to all open editor windows.
 */
void EditorContainer::configEditor()
{
	// Show the "Configure Editor" dialogue.
	EditorConfigDialog dlg(config_, this);
	if (dlg.exec() == QDialog::Rejected)
		return;

	// Get the configuration parameters from the dialogue.
	dlg.getConfig(config_);

	// Store the editor configuration.
	Settings& settings = Application::settings();
	settings.beginGroup("Editor");
	config_.store(settings);
	settings.endGroup();

	foreach (QMdiSubWindow* window, fileMap_) {
		Editor* editor = static_cast<Editor*>(window->widget());
		editor->applyConfig(config_);
	}
}

/**
 * Sets the focus to a line in an editor window matching the given location.
 * @param  loc  The location to go to
 */
void EditorContainer::gotoLocation(const Core::Location& loc)
{
	// Get the current location.
	Core::Location curLoc;
	bool addCurrent = false;
	if (currentEditor()) {
		currentEditor()->getCurrentLocation(curLoc);
		addCurrent = true;
	}

	// Go to the new location.
	if (getEditor(loc) == NULL)
		return;

	// Add both the previous and the new locations to the history list.
	if (addCurrent)
		history_.add(curLoc);
	history_.add(loc);
}

/**
 * Sets the focus to a line in an editor window matching the next location in
 * the history list.
 */
void EditorContainer::gotoNextLocation()
{
	Core::Location loc;
	if (history_.next(loc))
		(void)getEditor(loc);
}

/**
 * Sets the focus to a line in an editor window matching the previous location
 * in the history list.
 */
void EditorContainer::gotoPrevLocation()
{
	Core::Location loc;
	if (history_.prev(loc))
		(void)getEditor(loc);
}

/**
 * Shows a list of tags defined in the file of currently-active editor.
 */
void EditorContainer::showLocalTags()
{
	if (!currentEditor())
		return;

	// Create a query view dialogue.
	QueryResultDialog* dlg = new QueryResultDialog(this);
	dlg->setModal(true);

	// Go to selected locations.
	Core::QueryView* view = dlg->view();
	connect(view, SIGNAL(locationRequested(const Core::Location&)),
	        this, SLOT(gotoLocation(const Core::Location&)));

	dlg->setWindowTitle(tr("Local Tags"));
	dlg->show();

	try {
		// Run the query.
		view->model()->setRootPath(ProjectManager::project()->rootPath());
		view->model()->setColumns(ProjectManager::engine()
			.queryFields(Core::Query::LocalTags));
		ProjectManager::engine().query(view,
		                               Core::Query(Core::Query::LocalTags,
		                                           currentEditor()->path()));
	}
	catch (Core::Exception* e) {
		e->showMessage();
		delete e;
	}
}

/**
 * Shows a dialogue with the list of recently visited locations.
 */
void EditorContainer::browseHistory()
{
	// Construct the dialogue.
	QueryResultDialog dlg(this);
	dlg.setWindowTitle(tr("Location History"));

	// Add location history entries to the model.
	Core::QueryView* view = dlg.view();
	view->model()->add(history_.list(), QModelIndex());
	view->resizeColumns();

	// Setup the model's displayed columns.
	QList<Core::Location::Fields> columns;
	columns << Core::Location::File << Core::Location::Line
	        << Core::Location::Text;
	view->model()->setColumns(columns);

	try {
		// Set the root path.
		view->model()->setRootPath(ProjectManager::project()->rootPath());
	}
	catch (Core::Exception* e) {
		e->showMessage();
		delete e;
	}

	// Go to selected locations.
	connect(view, SIGNAL(locationRequested(const Core::Location&)),
	        this, SLOT(gotoLocation(const Core::Location&)));

	// Display the dialogue.
	dlg.exec();
}

/**
 * Finds an editor window for the given file.
 * If one does not exist, a new one is created.
 * @param  path      The path of the file to edit
 * @param  activate  Whether to also activate the window
 * @return The found/created editor if successful, NULL otherwise
 */
QMdiSubWindow* EditorContainer::getEditor(const Core::Location& loc,
                                          bool activate)
{
	qDebug() << __func__ << loc.file_ << loc.line_ << loc.column_ << activate;

	QMdiSubWindow* window;
	Editor* editor;

	// Try to find an existing editor window, based on the path.
	QMap<QString, QMdiSubWindow*>::Iterator itr = fileMap_.find(loc.file_);
	if (itr != fileMap_.end()) {
		window = *itr;
		editor = static_cast<Editor*>(window->widget());
	}
	else {
		// Create a new editor widget.
		editor = new Editor(this);

		// Open the given file in the editor.
		if (loc.isValid()) {
			if (!editor->load(loc.file_)) {
				delete editor;
				return NULL;
			}
		}
		else {
			// No path supplied, treat as a new file.
			editor->setNewFileIndex(newFileIndex_++);
		}

		editor->applyConfig(config_);
		connect(editor, SIGNAL(closed(const QString&)), this,
				SLOT(removeEditor(const QString&)));
		connect(editor, SIGNAL(titleChanged(const QString&, const QString&)),
		        this, SLOT(remapEditor(const QString&, const QString&)));

		// Create a new sub window for the editor.
		window = addSubWindow(editor);
		window->setAttribute(Qt::WA_DeleteOnClose);
		window->setWindowTitle(editor->title());
		window->show();
		fileMap_[editor->title()] = window;
	}

	// Move the cursor.
	editor->setCursorPosition(loc.line_, loc.column_);

	// Activate the editor, if required.
	if (activate) {
		if (window != currentSubWindow())
			setActiveSubWindow(window);
		else
			currentEditor()->setFocus();
	}

	return window;
}

/**
 * Closes all editor windows.
 */
void EditorContainer::closeAll()
{
	// Prompt the user for unsaved changes.
	if (!canClose())
		return;

	// Do not handle changes to the active editor while closing.
	blockWindowActivation_ = true;

	// Delete all editor windows.
	foreach (QMdiSubWindow* window, fileMap_)
		delete window;
	fileMap_.clear();

	// No active editor.
	currentWindow_ = NULL;
	emit hasActiveEditor(false);

	// Re-enable handling of changes to active windows.
	blockWindowActivation_ = false;
}

/**
 * Common handler for the file names in the "Window" menu.
 * Activates the window corresponding to the chosen file.
 * @param  action  The triggered action
 */
void EditorContainer::handleWindowAction(QAction* action)
{
	(void)getEditor(Core::Location(action->text()), true);
}

/**
 * Handles changes to the active editor window.
 * When an editor becomes active, it needs to get the keyboard focus. Also,
 * the container forwards certain signals to the active editor, which need to
 * be connected.
 * Note that this method is also called when the MDI area gains or loses focus.
 * If the latter happens, the active sub window becomes NULL, even though the
 * current sub window stays the same. This method is only concerned with changes
 * to the current sub window.
 * @param  window  The new active editor
 */
void EditorContainer::windowActivated(QMdiSubWindow* window)
{
	// Do nothing if activation signals are blocked.
	// TODO: This is just a workaround. The proper way should be to disconnect
	// from the subWindowActivated() signal.
	if (blockWindowActivation_)
		return;

	// Do nothing if the active window is not the current one (i.e., a NULL
	// active window due to the MDI area losing focus).
	if (window != currentSubWindow())
		return;

	// Do nothing if the new current window is the same as the old one.
	if (window == currentWindow_)
		return;

	// Stop forwarding signals to the active editor.
	if (currentWindow_)
		disconnect(static_cast<Editor*>(currentWindow_->widget()));

	// Remember the current window.
	currentWindow_ = window;

	// Update the active editor.
	Editor* editor = currentEditor();
	if (!editor) {
		qDebug() << "No current editor";
		return;
	}

	qDebug() << "Current editor" << (editor ? editor->path() : "");

	// Acquire keyboard focus.
	editor->setFocus();

	// Forward signals.
	connect(this, SIGNAL(find()), editor, SLOT(search()));
	connect(this, SIGNAL(findNext()), editor, SLOT(searchNext()));

	emit hasActiveEditor(true);
}

/**
 * Removes an editor from the file map when its window is closed.
 * @param  title  The unique title of the editor being closed
 */
void EditorContainer::removeEditor(const QString& title)
{
	fileMap_.remove(title);
	qDebug() << title << "removed";
}

/**
 * Changes the map key for an editor window.
 * This slot is called when an editor changes its title (e.g., following a
 * "Save As" operation).
 * @param  oldTitle The previous title of the editor
 * @param  newTitle The new title of the editor
 */
void EditorContainer::remapEditor(const QString& oldTitle,
                                  const QString& newTitle)
{
	QMap<QString, QMdiSubWindow*>::Iterator itr = fileMap_.find(oldTitle);
	if (itr != fileMap_.end()) {
		QMdiSubWindow* window = *itr;
		fileMap_.remove(oldTitle);
		fileMap_[newTitle] = window;
	}
}

} // namespace App

} // namespace KScope
