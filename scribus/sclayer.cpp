#include <utility>

/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include "sclayer.h"

#include <QHash>   //necessary to avoid msvc warnings induced by SCRIBUS_API on ScLayers + early instantiation of templates
#include <QObject>
#include <QVector> //necessary to avoid msvc warnings induced by SCRIBUS_API on ScLayers + early instantiation of templates
#include <QtAlgorithms>

ScLayer::ScLayer()
{
	Name         = QObject::tr("New Layer");
	ID           = 0;
	Level        = 0;
	isPrintable  = true;
	isViewable   = true;
	isEditable   = true;
	isSelectable = false;
	flowControl  = true;
	outlineMode  = false;
	transparency = 1.0;
	blendMode    = 0;
	markerColor  = QColor(0, 0, 0);
}

ScLayer::ScLayer(const QString& name, int level, int id)
{
	Name         = name;
	ID           = id;
	Level        = level;
	isPrintable  = true;
	isViewable   = true;
	isEditable   = true;
	isSelectable = false;
	flowControl  = true;
	outlineMode  = false;
	transparency = 1.0;
	blendMode    = 0;
	markerColor  = QColor(0, 0, 0);
	switch (ID % 7)
	{
		case 0:
			markerColor = Qt::black;
			break;
		case 1:
			markerColor = Qt::red;
			break;
		case 2:
			markerColor = Qt::green;
			break;
		case 3:
			markerColor = Qt::blue;
			break;
		case 4:
			markerColor = Qt::cyan;
			break;
		case 5:
			markerColor = Qt::magenta;
			break;
		case 6:
			markerColor = Qt::yellow;;
			break;
	}
}

bool ScLayer::operator< (const ScLayer& other) const
{
	return (Level < other.Level);
}

bool ScLayer::operator== (const ScLayer& other) const
{
	// ignore markerColor?
	return Name == other.Name && ID == other.ID && Level == other.Level &&
		isPrintable  == other.isPrintable  && isViewable  == other.isViewable &&
		flowControl  == other.flowControl  && outlineMode == other.outlineMode &&
		transparency == other.transparency && isEditable == other.isEditable &&
		blendMode   == other.blendMode;
}

int ScLayers::getMaxID()
{
	int nr, maxNr = -1;
	for (int i = 0; i < this->count(); ++i)
	{
		nr = this->at(i).ID;
		if (nr > maxNr)
			maxNr = nr;
	}
	return maxNr;
}

const ScLayer*  ScLayers::bottomLayer () const
{
	const ScLayer* layer, *bLayer = nullptr;
	for (int i = 0; i < this->count(); ++i)
	{
		layer = &this->at(i);
		if (!bLayer || (layer->Level < bLayer->Level))
			bLayer = layer;
	}
	return bLayer;
}

const ScLayer* ScLayers::topLayer() const
{
	const ScLayer *layer, *tLayer = nullptr;
	for (int i = 0; i < this->count(); ++i)
	{
		layer = &this->at(i);
		if (!tLayer || (layer->Level > tLayer->Level))
			tLayer = layer;
	}
	return tLayer;
}

void ScLayers::levelToLayer (ScLayer& layer, int level) const
{
	int layerCount = count();
	for (int i = 0; i < layerCount; ++i)
	{
		if (this->at(i).Level == level)
		{
			const ScLayer& ll  = this->at(i);
			layer.isViewable   = ll.isViewable;
			layer.isPrintable  = ll.isPrintable;
			layer.isEditable   = ll.isEditable;
			layer.isSelectable = ll.isSelectable;
			layer.ID           = ll.ID;
			layer.Name         = ll.Name;
			layer.flowControl  = ll.flowControl;
			layer.transparency = ll.transparency;
			layer.blendMode    = ll.blendMode;
			break;
		}
	}
}

ScLayer* ScLayers::byLevel(int level)
{
	ScLayers::Iterator itend = end();
	for (ScLayers::Iterator it = nullptr; it != itend; ++it)
	{
		if (it->Level == level)
			return &(*it);
	}
	return nullptr;
}

ScLayer* ScLayers::byID(int nr)
{
	ScLayers::Iterator itend = end();
	for (ScLayers::Iterator it = nullptr; it != itend; ++it)
	{
		if (it->ID == nr)
			return &(*it);
	}
	return nullptr;
}

ScLayer* ScLayers::bottom()
{
	ScLayer* bLayer = nullptr;
	ScLayers::Iterator it, itEnd = end();
	for (it = begin(); it != itEnd; ++it)
	{
		if (!bLayer || (it->Level < bLayer->Level))
			bLayer = &(*it);
	}
	return bLayer;
}

ScLayer* ScLayers::top()
{
	ScLayer *tLayer = nullptr;
	ScLayers::Iterator it, itEnd = end();
	for (it = begin(); it != itEnd; ++it)
	{
		if (!tLayer || (it->Level > tLayer->Level))
			tLayer = &(*it);
	}
	return tLayer;
}

ScLayer* ScLayers::above (int nr)
{
	ScLayer* lyr = byID(nr);
	if (lyr)
	{
		ScLayer *rlyr = top();
		int level     = lyr->Level;
		int maxLevel  = rlyr->Level;
		ScLayers::Iterator it, itEnd = end();
		for (it = begin(); it != itEnd; ++it)
		{
			if (it->Level > level && it->Level < maxLevel)
			{
				maxLevel = it->Level;
				rlyr     = &(*it);
			}
		}
		return rlyr;
	}
	return nullptr;
}

ScLayer* ScLayers::below (int nr)
{
	ScLayer* lyr = byID(nr);
	if (lyr)
	{
		ScLayer *rlyr = bottom();
		int level     = lyr->Level;
		int minLevel  = lyr->Level;
		ScLayers::Iterator it, itEnd = end();
		for (it = begin(); it != itEnd; ++it)
		{
			if (it->Level < level && it->Level > minLevel)
			{
				minLevel = it->Level;
				rlyr     = &(*it);
			}
		}
		return rlyr;
	}
	return nullptr;
}

const ScLayer* ScLayers::layerByLevel (int level) const
{
	const ScLayer *layer = nullptr;
	for (int i = 0; i < this->count(); ++i)
	{
		layer = &this->at(i);
		if (layer->Level == level)
			return layer;
	}
	return nullptr;
}

const ScLayer* ScLayers::layerByID (int nr) const
{
	const ScLayer *layer = nullptr;
	for (int i = 0; i < this->count(); ++i)
	{
		layer = &this->at(i);
		if (layer->ID == nr)
			return layer;
	}
	return nullptr;
}

const ScLayer* ScLayers::layerByName (const QString& name) const
{
	const ScLayer *layer = nullptr;
	for (int i = 0; i < this->count(); ++i)
	{
		layer = &this->at(i);
		if (layer->Name == name)
			return layer;
	}
	return nullptr;
}

const ScLayer* ScLayers::layerAbove (int level) const
{
	const ScLayer* top = topLayer();
	if (top)
	{
		const ScLayer *layer, *retlay = top;
		int   maxLevel = top->Level;
		for (int i = 0; i < this->count(); ++i)
		{
			layer = &this->at(i);
			if (layer->Level > level && layer->Level < maxLevel)
			{
				retlay = layer;
				maxLevel = layer->Level;
			}
		}
		return retlay;
	}
	return nullptr;
}

const ScLayer* ScLayers::layerAbove (const ScLayer& layer) const
{
	const ScLayer* top = topLayer();
	if (top)
	{
		const ScLayer *curlay, *retlay = top;
		int   level    = layer.Level;
		int   maxLevel = top->Level;
		for (int i = 0; i < this->count(); ++i)
		{
			curlay = &this->at(i);
			if (curlay->Level > level && curlay->Level < maxLevel)
			{
				maxLevel = curlay->Level;
				retlay   = curlay;
			}
		}
		return retlay;
	}
	return nullptr;
}

const ScLayer* ScLayers::layerBelow (int level) const
{
	const ScLayer* bottom = bottomLayer();
	if (bottom)
	{
		const ScLayer *layer, *retlay = bottom;
		int   minLevel = bottom->Level;
		for (int i = 0; i < this->count(); ++i)
		{
			layer = &this->at(i);
			if (layer->Level < level && layer->Level > minLevel)
			{
				minLevel = layer->Level;
				retlay   = layer;
			}
		}
		return retlay;
	}
	return nullptr;
}

const ScLayer* ScLayers::layerBelow (const ScLayer& layer) const
{
	const ScLayer* bottom = bottomLayer();
	if (bottom)
	{
		const ScLayer *curlay, *retlay = bottom;
		int   level    = layer.Level;
		int   minLevel = bottom->Level;
		for (int i = 0; i < this->count(); ++i)
		{
			curlay = &this->at(i);
			if (curlay->Level < level && curlay->Level > minLevel)
			{
				minLevel = curlay->Level;
				retlay   = curlay;
			}
		}
		return retlay;
	}
	return nullptr;
}

int ScLayers::addLayer(const QString& layerName)
{
	ScLayer* nl = newLayer(layerName);
	if (nl)
		return nl->ID;
	return -1;
}

int ScLayers::addLayer(const ScLayer& layer)
{
	int   newID = layer.ID;
	const ScLayer* lid = layerByID(newID);
	if (lid)
	{
		ScLayer newLyr(layer);
		newLyr.ID = newID = getMaxID() + 1;
		append(newLyr);
	}
	else
		append(layer);
	sort();
	return newID;
}

ScLayer* ScLayers::newLayer(const QString& layerName)
{
	QString lname;
	int     lId    = getMaxID() + 1;
	int     llevel = count();
	if (layerName.isEmpty())
	{
		QString tmp;
		lname = QObject::tr("New Layer")+" "+tmp.setNum(lId);
	}
	else
		lname = layerName;
	ScLayer ll(lname, llevel, lId);
	ScLayers::Iterator it = insert(end(), ll);
	return &(*it);
}

bool ScLayers::removeLayerByID(int id)
{
	int index = -1;
	int layerLevel = -1;
	for (int i = 0; i < count(); ++i)
	{
		if (this->at(i).ID == id)
		{
			index      = i;
			layerLevel = this->at(i).Level;
			break;
		}
	}
	if (index >= 0)
	{
		removeAt(index);
		ScLayers::Iterator it, itEnd = end();
		for (it = begin(); it != itEnd; ++it)
		{
			if ((*it).Level > layerLevel)
				(*it).Level -= 1;
		}
		return true;
	}
	return false;
}

bool ScLayers::removeLayerByLevel(int level)
{
	int index = -1;
	for (int i = 0; i < count(); ++i)
	{
		if (this->at(i).Level == level)
		{
			index = i;
			break;
		}
	}
	if (index >= 0)
	{
		removeAt(index);
		ScLayers::Iterator it, itEnd = end();
		for (it = begin(); it != itEnd; ++it)
		{
			if ((*it).Level > level)
				(*it).Level -= 1;
		}
	}
	return false;
}

bool ScLayers::raiseLayer(int nr)
{
	ScLayer* clyr = byID(nr);
	if (!clyr)
		return false;
	ScLayer* alyr = above(clyr->ID);
	if ((!alyr) || (clyr == alyr))
		return false;
	
	clyr->ID += 1;
	alyr->ID -= 1;
	return true;
}

bool ScLayers::lowerLayer(int nr)
{
	ScLayer* clyr = byID(nr);
	if (!clyr)
		return false;
	ScLayer* blyr = below(clyr->ID);
	if (!blyr || (clyr == blyr))
		return false;

	clyr->ID -= 1;
	blyr->ID += 1;
	return true;
}

void ScLayers::sort()
{
	int level = 0;
	ScLayers::Iterator it;
	ScLayers::Iterator itend = end();
	std::stable_sort(begin(), end());
	for (it = begin(); it != itend; ++it, ++level)
		it->Level = level;
}

bool ScLayers::layerPrintable(int layerID) const
{
	const ScLayer* layer = layerByID(layerID);
	if (layer)
		return layer->isPrintable;
	return false;
}

bool ScLayers::setLayerPrintable(int layerID, bool isPrintable)
{
	ScLayer* layer = byID(layerID);
	if (layer)
	{
		layer->isPrintable = isPrintable;
		return true;
	}
	return false;
}

bool ScLayers::layerVisible(int layerID) const
{
	const ScLayer* layer = layerByID(layerID);
	if (layer)
		return layer->isViewable;
	return false;
}

bool ScLayers::setLayerVisible(int layerID, bool isViewable)
{
	ScLayer* layer = byID(layerID);
	if (layer)
	{
		layer->isViewable = isViewable;
		return true;
	}
	return false;
}

bool ScLayers::layerLocked(int layerID) const
{
	const ScLayer* layer = layerByID(layerID);
	if (layer)
		return !(layer->isEditable);
	return false;
}

bool ScLayers::setLayerLocked(int layerID, bool isLocked)
{
	ScLayer* layer = byID(layerID);
	if (layer)
	{
		layer->isEditable = !isLocked;
		return true;
	}
	return false;
}

bool ScLayers::layerFlow(int layerID) const
{
	const ScLayer* layer = layerByID(layerID);
	if (layer)
		return layer->flowControl;
	return false;
}

bool ScLayers::setLayerFlow(int layerID, bool flow)
{
	ScLayer* layer = byID(layerID);
	if (layer)
	{
		layer->flowControl = flow;
		return true;
	}
	return false;
}

bool ScLayers::layerOutline(int layerID) const
{
	const ScLayer* layer = layerByID(layerID);
	if (layer)
		return layer->outlineMode;
	return false;
}

bool ScLayers::setLayerOutline(int layerID, bool outline)
{
	ScLayer* layer = byID(layerID);
	if (layer)
	{
		layer->outlineMode = outline;
		return true;
	}
	return false;
}

double ScLayers::layerTransparency(int layerID) const
{
	const ScLayer* layer = layerByID(layerID);
	if (layer)
		return layer->transparency;
	return 1.0;
}

bool ScLayers::setLayerTransparency(int layerID, double trans)
{
	ScLayer* layer = byID(layerID);
	if (layer)
	{
		layer->transparency = trans;
		return true;
	}
	return false;
}

int ScLayers::layerBlendMode(int layerID) const
{
	const ScLayer* layer = layerByID(layerID);
	if (layer)
		return layer->blendMode;
	return 0;
}

bool ScLayers::setLayerBlendMode(int layerID, int blend)
{
	ScLayer* layer = byID(layerID);
	if (layer)
	{
		layer->blendMode = blend;
		return true;
	}
	return false;
}

QColor ScLayers::layerMarker(int layerID) const
{
	const ScLayer* layer = layerByID(layerID);
	if (layer)
		return layer->markerColor;
	return Qt::black;
}

bool ScLayers::setLayerMarker(int layerID, QColor color)
{
	ScLayer* layer = byID(layerID);
	if (layer)
	{
		layer->markerColor = std::move(color);
		return true;
	}
	return false;
}

bool ScLayers::layerSelectable(int layerID) const
{
	const ScLayer* layer = layerByID(layerID);
	if (layer)
		return layer->isSelectable;
	return false;
}

bool ScLayers::setLayerSelectable(int layerID, bool isSelectable)
{
	ScLayer* layer = byID(layerID);
	if (layer)
	{
		layer->isSelectable = isSelectable;
		return true;
	}
	return false;
}

uint qHash(const ScLayer& layer)
{ 
	return qHash(&layer);
}
