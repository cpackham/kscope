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

#include "locationmodel.h"

namespace KScope
{

namespace Core
{

/**
 * Class constructor.
 * @param  parent   Parent object
 */
LocationModel::LocationModel(QObject* parent) : QAbstractItemModel(parent)
{
}

/**
 * Class destructor.
 */
LocationModel::~LocationModel()
{
}

/**
 * Sets a new common root path for display purposes.
 * @param  path  The new path to set
 */
void LocationModel::setRootPath(const QString& path)
{
	QString actPath = path;
	if (path == "/")
		actPath = QString();
	else if (!path.endsWith("/"))
		actPath += "/";

	if (actPath != rootPath_) {
		rootPath_ = actPath;
		reset();
	}
}

/**
 * Determines which fields of a location structure the model supports, and in
 * what order.
 * @param  colList  An ordered list of location structure fields
 */
void LocationModel::setColumns(const QList<Location::Fields>& colList)
{
	colList_ = colList;
}

/**
 * Creates an XML representation of the model, which can be used for storing the
 * model's data in a file.
 * @param  doc    The XML document object to use
 * @param  parent XML element under which new location elements should be
 *                created
 * @param  idx    The index to store (along with its children)
 */
void LocationModel::toXML(QDomDocument& doc, QDomElement& parent,
                          const QModelIndex& idx) const
{
	if (idx.isValid()) {
		// Translate the index into a location information structure.
		Location loc;
		if (!locationFromIndex(idx, loc))
			return;

		// Create an XML element for the location.
		QDomElement elem = doc.createElement("Location");
		parent.appendChild(elem);

		// Add a text node for each structure member.
		for (int i = 0; i < colList_.size(); i++) {
			QString name;
			QDomNode node;

			switch (colList_[i]) {
			case Location::File:
				name = "File";
				node = doc.createTextNode(loc.file_);
				break;

			case Location::Line:
				name = "Line";
				node = doc.createTextNode(QString::number(loc.line_));
				break;

			case Location::Column:
				name = "Column";
				node = doc.createTextNode(QString::number(loc.column_));
				break;

			case Location::TagName:
				name = "TagName";
				node = doc.createTextNode(loc.tag_.name_);
				break;

			case Location::TagType:
				name = "TagType";
				node = doc.createTextNode(QString::number(loc.tag_.type_));
				break;

			case Location::Scope:
				name = "Scope";
				node = doc.createTextNode(loc.tag_.scope_);
				break;

			case Location::Text:
				name = "Text";
				node = doc.createCDATASection(loc.text_);
				break;
			}

			QDomElement child = doc.createElement(name);
			child.appendChild(node);
			elem.appendChild(child);
		}
	}

	// Create an element list using the index's children.
	if (hasChildren(idx)) {
		QDomElement elem = doc.createElement("LocationList");
		parent.appendChild(elem);
		for (int i = 0; i < rowCount(idx); i++)
			toXML(doc, elem, index(i, 0, idx));
	}
}

/**
 * Populates a model with locations read from an XML document.
 * @param doc    The document to read from
 * @param parent An element representing a location list
 */
void LocationModel::fromXML(const QDomElement& parent, const QModelIndex& idx)
{
	// Get a list of location elements.
	QDomNodeList nodes = parent.elementsByTagName("Location");

	// Translate elements into a list of location objects.
	// The map is used to store sub-lists encountered inside the location
	// element. These will be loaded later.
	LocationList locList;
	QMap<int, QDomElement> childLists;
	for (int i = 0; i < nodes.size(); i++) {
		// Get the current location element.
		QDomElement elem = nodes.at(i).toElement();
		if (elem.isNull())
			continue;

		// Iterate over the sub-elements, which represent either location
		// properties, or nested location lists. We expect at most one of the
		// latter.
		Location loc;
		QDomNodeList childNodes = elem.childNodes();
		for (int j = 0; j < childNodes.size(); j++) {
			// Has to be an element.
			QDomElement child = childNodes.at(j).toElement();
			if (child.isNull())
				continue;

			// Extract location data from the element.
			if (child.tagName() == "File")
				loc.file_ = child.text();
			else if (child.tagName() == "Line")
				loc.line_ = child.text().toUInt();
			else if (child.tagName() == "Column")
				loc.column_ = child.text().toUInt();
			else if (child.tagName() == "TagName")
				loc.tag_.name_ = child.text();
			else if (child.tagName() == "TagType")
				loc.tag_.type_ = static_cast<Tag::Type>(child.text().toUInt());
			else if (child.tagName() == "Scope")
				loc.tag_.scope_ = child.text();
			else if (child.tagName() == "Text")
				loc.text_ = child.firstChild().toCDATASection().data();
			else if (child.tagName() == "LocationList")
				childLists[i] = child;
		}

		// Add to the location list.
		locList.append(loc);
	}

	// Store locations in the model.
	add(locList, idx);

	// Load any sub-lists encountered earlier.
	QMap<int, QDomElement>::Iterator itr;
	for (itr = childLists.begin(); itr != childLists.end(); ++itr)
		fromXML(itr.value(), index(itr.key(), 0, idx));
}

/**
 * Provides information for constructing a header when this model is displayed
 * in a view.
 * @param  section  Corresponds to the column number
 * @param  orient   Horizontal or vertical header
 * @param  role     How to interpret the information
 * @return For a horizontal header with a display role, returns the column
 *         title; An empty variant for all other type
 */
QVariant LocationModel::headerData(int section, Qt::Orientation orient,
                                   int role) const
{
	if (orient != Qt::Horizontal || role != Qt::DisplayRole)
		return QVariant();

	return columnText(colList_[section]);
}

/**
 * Returns the number of columns for the children of the given parent.
 * @param  parent  The parent index
 * @return The size of the column list for all the children of the root index
 */
int LocationModel::columnCount(const QModelIndex& parent) const
{
	if (!parent.isValid())
		return colList_.size();

	return 0;
}

/**
 * Extracts data from a location object, for the given column index.
 * @param  loc  The location object
 * @param  col  The requested column
 * @return Matching location data, QVariant() if the column is invalid
 */
QVariant LocationModel::locationData(const Location& loc, uint col,
                                     int role) const
{
	switch (role) {
	case Qt::DecorationRole:
		if ((colList_[col] == Location::TagName)
		    && (loc.tag_.type_ != Tag::UnknownTag)) {
			return Tag::icon(loc.tag_.type_);
		}
		return QVariant();

	case Qt::DisplayRole:
		break;

	default:
		return QVariant();
	}

	switch (colList_[col]) {
	case Location::File:
		// File path.
		// Replace root prefix with "$".
		if (!rootPath_.isEmpty() && loc.file_.startsWith(rootPath_))
			return QString("$/") + loc.file_.mid(rootPath_.length());

		return loc.file_;

	case Location::Line:
		// Line number.
		return loc.line_;

	case Location::Column:
		// Column number.
		return loc.column_;

	case Location::TagName:
		// Tag name.
		return loc.tag_.name_;

	case Location::TagType:
		// Tag type.
		return Tag::name(loc.tag_.type_);

	case Location::Scope:
		// Scope.
		return loc.tag_.scope_;

	case Location::Text:
		// Line text.
		return loc.text_;
	}

	return QVariant();
}

/**
 * Creates a column header title for the given column.
 * @param  col  The column for which the title is requested
 * @return A string describing the column
 */
QString LocationModel::columnText(Location::Fields col) const
{
	switch (col) {
	case Location::File:
		return tr("File");

	case Location::Line:
		return tr("Line");

	case Location::Column:
		return tr("Column");

	case Location::TagName:
		return tr("Tag");

	case Location::TagType:
		return tr("Type");

	case Location::Scope:
		return tr("Scope");

	case Location::Text:
		return tr("Text");
	}

	return "";
}

} // namespace Core

} // mamespace KScope
