/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include "autoform.h"
#include "autoformbuttongroup.h"

Autoforms::Autoforms( QWidget* parent ) : QToolButton( parent, "auto" )
{
	Men = new QMenu();
	buttonGroup1 = new AutoformButtonGroup( NULL );
	MenAct = new QWidgetAction(this);
	MenAct->setDefaultWidget(buttonGroup1);
	Men->addAction(MenAct);
	setPopup(Men);
	setPopupMode(QToolButton::InstantPopup);
	setPixmap(buttonGroup1->getIconPixmap(0));
	connect(buttonGroup1, SIGNAL(buttonClicked(int)), this, SLOT(selForm(int)));
}

void Autoforms::selForm(int a)
{
	int n;
	double* AutoShapes;
//	Men->activateItemAt(0);
	setPixmap(buttonGroup1->getIconPixmap(a));
	AutoShapes = buttonGroup1->getShapeData(a, &n);
	emit FormSel(a, n, AutoShapes);
}

QPixmap Autoforms::getIconPixmap(int nr)
{
	return buttonGroup1->getIconPixmap(nr);
}

