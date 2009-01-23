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

#include "queryview.h"
#include "locationlistmodel.h"
#include "locationtreemodel.h"
#include "exception.h"

namespace KScope
{

namespace Core
{

/**
 * Class constructor.
 * @param  parent  The parent widget
 * @param  type    Whether the view works in list or tree modes
 */
QueryView::QueryView(QWidget* parent, Type type)
	: QTreeView(parent), type_(type), progBar_(NULL),
	  autoSelectSingleResult_(false)
{
	// Set tree view properties.
	setRootIsDecorated(type_ == Tree);
	setUniformRowHeights(true);
	setExpandsOnDoubleClick(false);

	// Create a location model.
	switch (type_) {
	case List:
		setModel(new LocationListModel(this));
		break;

	case Tree:
		setModel(new LocationTreeModel(this));
		break;
	}

	// Emit requests for locations when an item is double-clicked.
	connect(this, SIGNAL(activated(const QModelIndex&)), this,
	        SLOT(requestLocation(const QModelIndex&)));

	// Query child items when expanded (in a tree view).
	if (type_ == Tree) {
		connect(this, SIGNAL(expanded(const QModelIndex&)), this,
		        SLOT(queryTreeItem(const QModelIndex&)));
	}
}

/**
 * Class destructor.
 */
QueryView::~QueryView()
{
}

/**
 * @param  query  The query to run
 */
void QueryView::query(const Query& query)
{
	// Reset the model.
	model()->clear();

	try {
		// Get an engine for running the query.
		Engine* eng;
		if ((eng = engine()) != NULL) {
			// Run the query.
			query_ = query;
			model()->setColumns(eng->queryFields(query_.type_));
			eng->query(this, query_);
		}
	}
	catch (Exception* e) {
		e->showMessage();
		delete e;
	}
}

/**
 * Adjusts all columns to fit the their contents.
 */
void QueryView::resizeColumns()
{
	for (int i = 0; i < model()->columnCount(); i++)
		resizeColumnToContents(i);
}

/**
 * Creates an XML representation of the view, which can be used for storing the
 * model's data in a file.
 * The representation is rooted at a <QueryView> element, which holds a <Query>
 * element for the query information, a <Columns> element with a list of
 * columns, and a <LocationList> element that describes the list or tree of
 * locations.
 * @param  doc    The XML document object to use
 * @return The root element of the view's representation
 */
QDomElement QueryView::toXML(QDomDocument& doc) const
{
	// Create an element for storing the view.
	QDomElement viewElem = doc.createElement("QueryView");
	viewElem.setAttribute("name", windowTitle());
	viewElem.setAttribute("type", QString::number(type_));

	// Store query information.
	QDomElement queryElem = doc.createElement("Query");
	queryElem.setAttribute("type", QString::number(query_.type_));
	queryElem.setAttribute("flags", QString::number(query_.flags_));
	queryElem.appendChild(doc.createCDATASection(query_.pattern_));
	viewElem.appendChild(queryElem);

	// Create a "Columns" element.
	QDomElement colsElem = doc.createElement("Columns");
	viewElem.appendChild(colsElem);

	// Add an element for each column.
	foreach (Location::Fields field, model()->columns()) {
		QDomElement colElem = doc.createElement("Column");
		colElem.setAttribute("field", QString::number(field));
		colsElem.appendChild(colElem);
	}

	// Add locations.
	locationToXML(doc, viewElem, QModelIndex());

	return viewElem;
}

/**
 * Loads a query view from an XML representation.
 * @param  root The root element for the query's XML representation
 */
void QueryView::fromXML(const QDomElement& viewElem)
{
	// Get query information.
	QDomElement queryElem = viewElem.elementsByTagName("Query").at(0).toElement();
	if (queryElem.isNull())
		return;

	query_.type_ = static_cast<Core::Query::Type>
	              (queryElem.attribute("type").toUInt());
	query_.flags_ = queryElem.attribute("flags").toUInt();
	query_.pattern_ = queryElem.childNodes().at(0).toCDATASection().data();

	// Reset the model.
	model()->clear();

	// TODO: Is there a guarantee of order?
	QDomNodeList columnNodes = viewElem.elementsByTagName("Column");
	QList<Location::Fields> colList;
	for (int i = 0; i < columnNodes.size(); i++) {
		QDomElement elem = columnNodes.at(i).toElement();
		if (elem.isNull())
			continue;

		colList.append(static_cast<Location::Fields>
		               (elem.attribute("field").toUInt()));
	}
	model()->setColumns(colList);

	// Find the <LocationList> element that is a child of the root element.
	QDomNodeList childNodes = viewElem.childNodes();
	for (int i = 0; i < childNodes.size(); i++) {
		QDomElement elem = childNodes.at(i).toElement();
		if (elem.isNull() || elem.tagName() != "LocationList")
			continue;

		// Load locations.
		locationFromXML(elem, QModelIndex());
	}
}

/**
 * Called by the engine when results are available.
 * Adds the list of locations to the model.
 * @param  locList  Query results
 */
void QueryView::onDataReady(const LocationList& locList)
{
	model()->add(locList, queryIndex_);
}

/**
 * Displays progress information in a progress-bar at the top of the view.
 * @param  text  Progress message
 * @param  cur   Current value
 * @param  total Expected final value
 */
void QueryView::onProgress(const QString& text, uint cur, uint total)
{
	// Create the progress-bar widget, if it does not exist.
	if (!progBar_) {
		progBar_ = new ProgressBar(this);
		connect(progBar_, SIGNAL(cancelled()), this, SLOT(stopQuery()));
		progBar_->show();
	}

	// Update progress information in the progress bar.
	progBar_->setLabel(text);
	progBar_->setProgress(cur, total);

	if (!isVisible())
		emit needToShow();
}

/**
 * Called by the engine when a query terminates normally.
 */
void QueryView::onFinished()
{
	// Handle an empty result set.
	if (model()->rowCount(queryIndex_) == 0)
		model()->setEmpty(queryIndex_);

	// Destroy the progress-bar, if it exists.
	if (progBar_) {
		delete progBar_;
		progBar_ = NULL;
	}

	// Adjust column sizes.
	resizeColumns();

	// Auto-select a single result, if required.
	Location loc;
	if (autoSelectSingleResult_ && model()->rowCount(queryIndex_) == 1
	                            && model()->firstLocation(loc)) {
		emit locationRequested(loc);
	}
	else if (!isVisible()) {
		emit needToShow();
	}
}

/**
 * Called by the engine when a query terminates abnormally.
 */
void QueryView::onAborted()
{
	// Destroy the progress-bar, if it exists.
	if (progBar_) {
		delete progBar_;
		progBar_ = NULL;
	}
}

/**
 * Selects the next location in the list.
 */
void QueryView::selectNext()
{
	QModelIndex selIndex = model()->nextIndex(currentIndex());
	if (selIndex.isValid())
		setCurrentIndex(selIndex);

	Location loc;
	if (model()->locationFromIndex(selIndex, loc))
		emit locationRequested(loc);
}

/**
 * Selects the previous location in the list.
 */
void QueryView::selectPrev()
{
	QModelIndex selIndex = model()->prevIndex(currentIndex());
	if (selIndex.isValid())
		setCurrentIndex(selIndex);

	Location loc;
	if (model()->locationFromIndex(selIndex, loc))
		emit locationRequested(loc);
}

/**
 * Recursively transforms the location hierarchy stored in the model to an XML
 * sub-tree.
 * The XML representation of locations is composed of a <LocationList> element
 * holding a list of <Location> elements. For a tree model, <Location> elements
 * may in turn hold a <LocationList> sub-element, and so on.
 * @param  doc        The XML document object to use
 * @param  parentElem XML element under which new location elements should be
 *                    created
 * @param  idx        The index to store (along with its children)
 */
void QueryView::locationToXML(QDomDocument& doc, QDomElement& parentElem,
                              const QModelIndex& idx) const
{
	QDomElement elem;

	if (idx.isValid()) {
		// A non-root index.
		// Translate the index into a location information structure.
		Location loc;
		if (!model()->locationFromIndex(idx, loc))
			return;

		// Create an XML element for the location.
		elem = doc.createElement("Location");
		parentElem.appendChild(elem);

		// Add a text node for each structure member.
		const QList<Location::Fields>& colList = model()->columns();
		foreach (Location::Fields field, colList) {
			QString name;
			QDomNode node;

			switch (field) {
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
	else {
		// For the root index, use the given parent element as the parent of
		// the location list created by top-level items.
		elem = parentElem;
	}

	// Create an element list using the index's children.
	if (model()->hasChildren(idx)) {
		QDomElement locListElem = doc.createElement("LocationList");
		elem.setAttribute("expanded", isExpanded(idx) ? "1" : "0");
		elem.appendChild(locListElem);
		for (int i = 0; i < model()->rowCount(idx); i++)
			locationToXML(doc, locListElem, model()->index(i, 0, idx));
	}
}

/**
 * Loads a hierarchy of locations from an XML document into the model.
 * See locationToXML() for the XML format.
 * @param  locListElem A <LocationList> XML element
 * @param  parentIndex The model index under which locations should be added
 */
void QueryView::locationFromXML(const QDomElement& locListElem,
                                const QModelIndex& parentIndex)
{
	// Get a list of location elements.
	QDomNodeList nodes = locListElem.childNodes();

	// Translate elements into a list of location objects.
	// The map is used to store sub-lists encountered inside the location
	// element. These will be loaded later. It's better to first construct the
	// list of locations for the current level, rather than follow the XML
	// tree depth-first, due to the behaviour of the add() method.
	LocationList locList;
	QMap<int, QDomElement> childLists;
	for (int i = 0; i < nodes.size(); i++) {
		// Get the current location element.
		QDomElement elem = nodes.at(i).toElement();
		if (elem.isNull() || elem.tagName() != "Location")
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
	model()->add(locList, parentIndex);

	// Load any sub-lists encountered earlier.
	QMap<int, QDomElement>::Iterator itr;
	for (itr = childLists.begin(); itr != childLists.end(); ++itr) {
		QDomElement elem = itr.value();
		QModelIndex index = model()->index(itr.key(), 0, parentIndex);
		locationFromXML(elem, index);
		if (elem.attribute("expanded").toUInt())
			expand(index);
	}
}

/**
 * Called when the user double-clicks a location item in the list.
 * Emits the locationRequested() signal for this location.
 * @param  index  Identifies the clicked item
 */
void QueryView::requestLocation(const QModelIndex& index)
{
	Location loc;
	if (model()->locationFromIndex(index, loc))
		emit locationRequested(loc);
}

/**
 * Called when the "Cancel" button is clicked in the progress-bar.
 * Informs the engine that the query process should be stopped.
 */
void QueryView::stopQuery()
{
	stop();
}

/**
 * Called when a tree item is expanded.
 * If this item was not queried before, a query is performed.
 * @param  idx  The expanded item
 */
void QueryView::queryTreeItem(const QModelIndex& idx)
{
	// TODO: DO not query twice! Need a LocationTreeModel method to determine
	// if an item was already queried.

	// Get the location information from the index.
	Location loc;
	if (!model()->locationFromIndex(idx, loc))
		return;

	// Run a query on this location.
	try {
		Engine* eng;
		if ((eng = engine()) != NULL) {
			queryIndex_ = idx;
			eng->query(this, Query(query_.type_, loc.tag_.scope_));
		}
	}
	catch (Exception* e) {
		e->showMessage();
		delete e;
	}
}

} // namespace Core

} // namespace KScope
