#include "CK2Title.h"



void CK2Title::init(Object* obj,  map<int, CK2Character*> characters)
{
	titleString = obj->getKey();
	vector<Object*> liegeObjs = obj->getValue("liege");
	if (liegeObjs.size() > 0)
	{
		liegeString = liegeObjs[0]->getLeaf();
	}

	independent	= true;
	inHRE			= false;

	vector<Object*> holderObjs = obj->getValue("holder");
	if (holderObjs.size() > 0)
	{
		holder = characters[ atoi( holderObjs[0]->getLeaf().c_str() ) ];
	}
}


void CK2Title::addLiege(CK2Title* newLiege)
{
	liege = newLiege;
	liege->addVassal(this);

	independent = false;
}


void CK2Title::addVassal(CK2Title* vassal)
{
	vassals.push_back(vassal);
}


void CK2Title::addToHRE()
{
	inHRE = true;
}


string CK2Title::getTitleString()
{
	return titleString;
}


string CK2Title::getLiegeString()
{
	return liegeString;
}


CK2Title* CK2Title::getLiege()
{
	return liege;
}


bool CK2Title::isIndependent()
{
	return independent;
}


bool CK2Title::isInHRE()
{
	return inHRE;
}



CK2Title::~CK2Title()
{
/*	while (vassals.size() > 0)
	{
		CK2Title* currentTitle = vassals[vassals.size() - 1];
		delete currentTitle;
		vassals.pop_back();
	}*/
}