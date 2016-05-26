/*Copyright (c) 2013 The CK2 to EU3 Converter Project

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:

 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#include "CK2Building.h"
#include "CK2Barony.h"
#include "CK2Title.h"
#include "CK2World\Character\CK2Character.h"
#include "..\Parsers\Object.h"
#include "..\Log.h"
#include "..\Configuration.h"



CK2Barony::CK2Barony(Object* obj, CK2Title* newTitle, CK2Province* newProvince, const CK2BuildingFactory* buildingFactory)
{
	title		= newTitle;
	province	= newProvince;

	type = obj->getLeaf("type");

	qualityBuildings = 0;
	buildings.clear();
	vector<IObject*> leaves = obj->getLeaves();
	for (vector<IObject*>::iterator i = leaves.begin(); i < leaves.end(); i++)
	{
		string key = (*i)->getKey();
		if ( (key.substr(0, 3) == "ca_") || (key.substr(0, 3) == "ct_") || (key.substr(0, 3) == "tp_") )
		{
			const CK2Building* newBuilding = buildingFactory->getBuilding(key, title->getHolder());
			if (newBuilding != NULL)
			{
				buildings.push_back(newBuilding);
			}
		}
		if ( (key.substr(3, 8) == "training") || (key.substr(3, 7) == "culture") )
		{
			qualityBuildings++;
		}
	}

	proxyMultiplier = 1;
	if (Configuration::getProxyMultiplierMethod() == "counting")
	{
		if (title->getTitleString() == title->getHolder()->getCapitalString())
		{
			vector<CK2Title*> titles = title->getHolder()->getTitles();
			for (vector<CK2Title*>::const_iterator titleItr = titles.begin(); titleItr != titles.end(); titleItr++)
			{
				if ( ((*titleItr)->getTitleString().substr(0,2) == "c_") && (proxyMultiplier < 2) )
				{
					proxyMultiplier = 2;
				}
				if ( ((*titleItr)->getTitleString().substr(0,2) == "d_") && (proxyMultiplier < 3) )
				{
					proxyMultiplier = 3;
				}
				if ( ((*titleItr)->getTitleString().substr(0,2) == "k_") && (proxyMultiplier < 4) )
				{
					proxyMultiplier = 4;
				}
				if ( ((*titleItr)->getTitleString().substr(0,2) == "e_") && (proxyMultiplier < 5) )
				{
					proxyMultiplier = 5;
				}
			}
		}
	}
	else if (Configuration::getProxyMultiplierMethod() == "exponential")
	{
		if (title->getTitleString() == title->getHolder()->getCapitalString())
		{
			vector<CK2Title*> titles = title->getHolder()->getTitles();
			for (vector<CK2Title*>::const_iterator titleItr = titles.begin(); titleItr != titles.end(); titleItr++)
			{
				if ( ((*titleItr)->getTitleString().substr(0,2) == "c_") && (proxyMultiplier < 2) )
				{
					proxyMultiplier = 2;
				}
				if ( ((*titleItr)->getTitleString().substr(0,2) == "d_") && (proxyMultiplier < 4) )
				{
					proxyMultiplier = 4;
				}
				if ( ((*titleItr)->getTitleString().substr(0,2) == "k_") && (proxyMultiplier < 8) )
				{
					proxyMultiplier = 8;
				}
				if ( ((*titleItr)->getTitleString().substr(0,2) == "e_") && (proxyMultiplier < 16) )
				{
					proxyMultiplier = 16;
				}
			}
		}
	}

	determineBaseTaxProxy();
	determinePopProxy();
	determineManpowerProxy();
	determineTechBonus();
	determineFortLevel();

	PSE				= 0.0F;
	ships				= 0;
	maxShips			= 0;
	freeTroops		= 0.0F;
	serfTroops		= 0.0F;
	vector<IObject*> levyObjs = obj->getValue("levy")[0]->getLeaves();
	for (unsigned int i = 0; i < levyObjs.size(); i++)
	{
		if (levyObjs[i]->getKey() == "light_infantry")
		{
			vector<string> tokens = levyObjs[i]->getTokens();
			PSE			+= atoi(tokens[0].c_str()) * 0.305;
			serfTroops	+= atoi(tokens[0].c_str()) * 2.000;
		}
		else if (levyObjs[i]->getKey() == "archers")
		{
			vector<string> tokens = levyObjs[i]->getTokens();
			PSE			+= atoi(tokens[0].c_str()) * 0.384;
			freeTroops	+=	atoi(tokens[0].c_str()) * 1.000;
		}
		else if (levyObjs[i]->getKey() == "heavy_infantry")
		{
			vector<string> tokens = levyObjs[i]->getTokens();
			PSE			+= atoi(tokens[0].c_str()) * 0.610;
			freeTroops	+= atoi(tokens[0].c_str()) * 0.500;
		}
		else if (levyObjs[i]->getKey() == "pikemen")
		{
			vector<string> tokens = levyObjs[i]->getTokens();
			PSE			+= atoi(tokens[0].c_str()) * 1.00;
			freeTroops	+= atoi(tokens[0].c_str()) * 0.75;
		}
		else if (levyObjs[i]->getKey() == "light_cavalry")
		{
			vector<string> tokens = levyObjs[i]->getTokens();
			PSE			+= atoi(tokens[0].c_str()) * 0.23;
			freeTroops	+= atoi(tokens[0].c_str()) * 0.25;
		}
		else if (levyObjs[i]->getKey() == "knights")
		{
			vector<string> tokens = levyObjs[i]->getTokens();
			PSE += 1.00 * atoi(tokens[0].c_str());
		}
		else if (levyObjs[i]->getKey() == "horse_archers")
		{
			vector<string> tokens = levyObjs[i]->getTokens();
			PSE			+= atoi(tokens[0].c_str()) * 0.42;
			freeTroops	+= atoi(tokens[0].c_str()) * 0.50;
		}
		else if (levyObjs[i]->getKey() == "galleys")
		{
			vector<string> tokens = levyObjs[i]->getTokens();
			ships		+= atoi(tokens[0].c_str());
			maxShips	+= atoi(tokens[1].c_str());
		}
	}
}


void CK2Barony::determineBaseTaxProxy()
{
	baseTaxProxy = 0.0f;
	if (type == "city")
	{
		baseTaxProxy += 12.0f;
	}
	else if (type == "castle")
	{
		baseTaxProxy += 4.0f;
	}
	else if (type == "temple")
	{
		baseTaxProxy += 8.0f;
	}
	else if (type == "family_palace")
	{
		baseTaxProxy += 10.0f;
	}
	else
	{
		log("Note! Unhandled barony type %s\n", type.c_str());
	}

	for (vector<const CK2Building*>::iterator buildingItr = buildings.begin(); buildingItr != buildings.end(); buildingItr++)
	{
		if ( (*buildingItr)->getFortLevel() > 0.0f )
		{
			continue;
		}
		baseTaxProxy += (*buildingItr)->getTaxIncome();
	}

	baseTaxProxy *= proxyMultiplier;
}


void CK2Barony::determinePopProxy()
{
	popProxy = 0.0f;
	if (type == "city")
	{
		popProxy += 12.0f;
	}
	else if (type == "castle")
	{
		popProxy += 4.0f;
	}
	else if (type == "temple")
	{
		popProxy += 8.0f;
	}
	else if (type == "family_palace")
	{
		popProxy += 10.0f;
	}
	else
	{
		log("Note! Unhandled barony type %s\n", type.c_str());
	}

	for (vector<const CK2Building*>::iterator buildingItr = buildings.begin(); buildingItr != buildings.end(); buildingItr++)
	{
		popProxy += (*buildingItr)->getTaxIncome();
	}

	popProxy *= proxyMultiplier;
}


void CK2Barony::determineManpowerProxy()
{
	manpowerProxy = 0.0f;
	if (type == "castle")
	{
		manpowerProxy += 225;
	}
	else if (type == "city")
	{
		manpowerProxy += 115;
	}
	else if (type == "temple")
	{
		manpowerProxy += 130;
	}
	else if (type == "family_palace")
	{
		manpowerProxy += 50;
	}
	else
	{
		log("Note! Unhandled barony type %s\n", type.c_str());
	}

	double levyMultiplier = 1.0f;
	for (vector<const CK2Building*>::iterator buildingItr = buildings.begin(); buildingItr != buildings.end(); buildingItr++)
	{
		manpowerProxy	+= (*buildingItr)->getNumSoldiers();
		levyMultiplier	+= (*buildingItr)->getLevyMultiplier();
	}
	manpowerProxy *= levyMultiplier;

	manpowerProxy *= proxyMultiplier;
}


void CK2Barony::determineTechBonus()
{
	techBonus = 0.0f;
	for (vector<const CK2Building*>::iterator buildingItr = buildings.begin(); buildingItr != buildings.end(); buildingItr++)
	{
		techBonus	+= (*buildingItr)->getTechBonus();
	}
}


void CK2Barony::determineFortLevel()
{
	fortLevel = 0.0f;
	for (vector<const CK2Building*>::iterator buildingItr = buildings.begin(); buildingItr != buildings.end(); buildingItr++)
	{
		fortLevel	+= (*buildingItr)->getFortLevel();
	}
}
