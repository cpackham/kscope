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

#ifndef __APP_EDITOR_H
#define __APP_EDITOR_H

#include <qsciscintilla.h>
#include <QSettings>
#include "globals.h"

namespace KScope
{

namespace App
{

/**
 * An QScintilla editor widget used to view/edit files.
 * @author Elad Lahav
 */
class Editor : public QsciScintilla
{
	Q_OBJECT

public:
	Editor(QWidget* parent = 0);
	~Editor();

	/**
	 * Provides editor configuration parameters.
	 */
	struct Config
	{
		void load(QSettings& settings);
		void store(QSettings& settings);

		QFont font_;
		bool hlCurLine_;
		bool indentTabs_;
		int tabWidth_;
	};

	/**
	 * Options for text searches.
	 */
	struct SearchOptions
	{
		/**
		 * Whether the pattern represents a regular expression.
		 */
		bool regExp_;

		/**
		 * Whether the search is case sensitive.
		 */
		bool caseSensitive_;

		/**
		 * Whether to look for whole words only.
		 */
		bool wholeWordsOnly_;

		/**
		 * Whether the search should wrap at the end of the document.
		 */
		bool wrap_;

		/**
		 * Whether to search backward in the document.
		 */
		bool backward_;
	};

	bool load(const QString&);
	bool save();
	bool canClose();
	void setCursorPosition(uint, uint);
	QString currentSymbol() const;
	void setFocus();
	void applyConfig(const Config&);
	void getConfig(Config&);
	void getCurrentLocation(Core::Location&);

	QString path() const { return path_; }

public slots:
	void search();
	void searchNext();

signals:
	void closed(const QString& path);

protected:
	void closeEvent(QCloseEvent*);

private:
	/**
	 * The file being edited.
	 */
	QString path_;

	/**
	 * Whether a file is currently being loaded.
	 */
	bool isLoading_;

	/**
	 * The line to go to when loading finishes.
	 */
	uint onLoadLine_;

	/**
	 * The column to go to when loading finishes.
	 */
	uint onLoadColumn_;

	/**
	 * Whether to set the keyboard focus when loading finishes.
	 */
	bool onLoadFocus_;

private slots:
	void loadDone(const QString&);
};

} // namespace App

} // namespace KScope

#endif  // __APP_EDITOR_H
