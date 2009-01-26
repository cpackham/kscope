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

#ifndef __APP_SESSION_H__
#define __APP_SESSION_H__

#include <QDomDocument>
#include "globals.h"
#include "queryview.h"

namespace KScope
{

namespace App
{

/**
 * Manages a KScope session.
 * Responsible for storing a session when a project is closed, and for restoring
 * it when the project is opened again.
 * @author Elad Lahav
 */
class Session
{
public:
	Session(const QString&);
	~Session();

	struct QueryViewIterator
	{
		void load(QueryView*);

		bool isAtEnd() { return listPos_ == queryNodeList_.size(); }

		QueryViewIterator& operator++() {
			for (++listPos_; listPos_ < queryNodeList_.size(); listPos_++) {
				elem_ = queryNodeList_.at(listPos_).toElement();
				if (!elem_.isNull())
					break;
			}

			title_ = elem_.attribute("name");
			type_ = static_cast<Core::QueryView::Type>
			        (elem_.attribute("type").toUInt());
			return *this;
		}

		const QString& title() const { return title_; }
		Core::QueryView::Type type() const { return type_; }

	private:
		QDomNodeList queryNodeList_;
		int listPos_;
		QDomElement elem_;
		QString title_;
		Core::QueryView::Type type_;

		friend class Session;
	};

	void load();
	void save();

	void addQueryView(const QueryView*);
	QueryViewIterator beginQueryIteration() const;

	const Core::LocationList& editorList() const { return editorList_; }
	const QString& activeEditor() const { return activeEditor_; }

	void setEditorList(const Core::LocationList& list) { editorList_ = list; }
	void setActiveEditor(const QString& editor) { activeEditor_ = editor; }

private:
	/**
	 * The path of the configuration directory holding session information.
	 */
	QString path_;

	/**
	 * A list of locations representing open editor windows.
	 */
	Core::LocationList editorList_;

	/**
	 * The path of the file in the active editor.
	 */
	QString activeEditor_;

	/**
	 * An XML document used to manage open query views.
	 */
	QDomDocument queryViewDoc_;

	inline QString configFile() { return path_ + "/session.conf"; }

	inline QString queryViewFile() { return path_ + "/queries.xml"; }
};

} // namespace App

} // namespace KScope

#endif // __APP_SESSION_H__
