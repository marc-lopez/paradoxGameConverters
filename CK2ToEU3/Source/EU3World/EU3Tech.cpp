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



#include "EU3Tech.h"
#include "..\log.h"


enum techCategory
{
	GOVERNMENT = 0,
	PRODUCTION,
	TRADE,
	NAVAL,
	LAND
};


EU3Tech::EU3Tech(common::date startDate, Object* mainObj, Object* governmentObj, Object* productionObj,
                 Object* tradeObj, Object* navalObj, Object* landObj)
{
	vector<IObject*> groupsObjs = mainObj->getValue("groups")[0]->getLeaves();
	for(vector<IObject*>::iterator groupItr = groupsObjs.begin(); groupItr != groupsObjs.end(); groupItr++)
	{
		string	name			= (*groupItr)->getKey();
		int		startLevel	= atoi( (*groupItr)->getLeaf("start_level").c_str() );
		double	modifier		= atof( (*groupItr)->getLeaf("modifier").c_str() );
		groups.insert(make_pair(name, make_pair(startLevel, modifier) ));
	}

	vector<IObject*> governmentObjs = governmentObj->getLeaves();
	governmentYears.resize(governmentObjs.size());
	for (vector<IObject*>::iterator techItr = governmentObjs.begin(); techItr != governmentObjs.end(); techItr++)
	{
		int level	= atoi( (*techItr)->getLeaf("id").c_str() );
		int year		= atoi( (*techItr)->getLeaf("average_year").c_str() );
		governmentYears[level] = year;
	}

	vector<IObject*> productionObjs = productionObj->getLeaves();
	productionYears.resize(productionObjs.size());
	for (vector<IObject*>::iterator techItr = productionObjs.begin(); techItr != productionObjs.end(); techItr++)
	{
		int level	= atoi( (*techItr)->getLeaf("id").c_str() );
		int year		= atoi( (*techItr)->getLeaf("average_year").c_str() );
		productionYears[level] = year;
	}

	vector<IObject*> tradeObjs = tradeObj->getLeaves();
	tradeYears.resize(tradeObjs.size());
	for (vector<IObject*>::iterator techItr = tradeObjs.begin(); techItr != tradeObjs.end(); techItr++)
	{
		int level	= atoi( (*techItr)->getLeaf("id").c_str() );
		int year		= atoi( (*techItr)->getLeaf("average_year").c_str() );
		tradeYears[level] = year;
	}

	vector<IObject*> navalObjs = navalObj->getLeaves();
	navalYears.resize(navalObjs.size());
	for (vector<IObject*>::iterator techItr = navalObjs.begin(); techItr != navalObjs.end(); techItr++)
	{
		int level	= atoi( (*techItr)->getLeaf("id").c_str() );
		int year		= atoi( (*techItr)->getLeaf("average_year").c_str() );
		navalYears[level] = year;
	}

	vector<IObject*> landObjs = landObj->getLeaves();
	landYears.resize(landObjs.size());
	for (vector<IObject*>::iterator techItr = landObjs.begin(); techItr != landObjs.end(); techItr++)
	{
		int level	= atoi( (*techItr)->getLeaf("id").c_str() );
		int year		= atoi( (*techItr)->getLeaf("average_year").c_str() );
		landYears[level] = year;
	}

	for (map<string, pair<int, double> >::iterator groupsItr = groups.begin(); groupsItr != groups.end(); groupsItr++)
	{
		vector<double> startingLevels;
		startingLevels.resize(5);

		double	rate			= groupsItr->second.second;

		double	level			= groupsItr->second.first;
		int		nextLevel	= groupsItr->second.first + 1;
		common::date		currentDate("1399.2.1");
		int		yearCurrentTech	= governmentYears[nextLevel - 1];
		int		yearNextTech		= governmentYears[nextLevel];
		while (currentDate < startDate)
		{
			level += (rate / 12) / (yearNextTech - yearCurrentTech);
			if (level >= nextLevel)
			{
				yearCurrentTech	= governmentYears[nextLevel];
				nextLevel++;
				yearNextTech		= governmentYears[nextLevel];
			}

			currentDate.month++;
			if (currentDate.month > 12)
			{
				currentDate.year++;
				currentDate.month -= 12;
			}
		}
		startingLevels[GOVERNMENT] = level;

		level					= groupsItr->second.first;
		nextLevel			= groupsItr->second.first + 1;
		currentDate			= common::date("1399.2.1");
		yearCurrentTech	= productionYears[nextLevel - 1];
		yearNextTech		= productionYears[nextLevel];
		while (currentDate < startDate)
		{
			level += (rate / 12) / (yearNextTech - yearCurrentTech);
			if (level >= nextLevel)
			{
				yearCurrentTech	= productionYears[nextLevel];
				nextLevel++;
				yearNextTech		= productionYears[nextLevel];
			}

			currentDate.month++;
			if (currentDate.month > 12)
			{
				currentDate.year++;
				currentDate.month -= 12;
			}
		}
		startingLevels[PRODUCTION] = level;

		level					= groupsItr->second.first;
		nextLevel			= groupsItr->second.first + 1;
		currentDate			= common::date("1399.2.1");
		yearCurrentTech	= tradeYears[nextLevel - 1];
		yearNextTech		= tradeYears[nextLevel];
		while (currentDate < startDate)
		{
			level += (rate / 12) / (yearNextTech - yearCurrentTech);
			if (level >= nextLevel)
			{
				yearCurrentTech	= tradeYears[nextLevel];
				nextLevel++;
				yearNextTech		= tradeYears[nextLevel];
			}

			currentDate.month++;
			if (currentDate.month > 12)
			{
				currentDate.year++;
				currentDate.month -= 12;
			}
		}
		startingLevels[TRADE] = level;

		level					= groupsItr->second.first;
		nextLevel			= groupsItr->second.first + 1;
		currentDate			= common::date("1399.2.1");
		yearCurrentTech	= navalYears[nextLevel - 1];
		yearNextTech		= navalYears[nextLevel];
		while (currentDate < startDate)
		{
			level += (rate / 12) / (yearNextTech - yearCurrentTech);
			if (level >= nextLevel)
			{
				yearCurrentTech	= navalYears[nextLevel];
				nextLevel++;
				yearNextTech		= navalYears[nextLevel];
			}

			currentDate.month++;
			if (currentDate.month > 12)
			{
				currentDate.year++;
				currentDate.month -= 12;
			}
		}
		startingLevels[NAVAL] = level;

		level					= groupsItr->second.first;
		nextLevel			= groupsItr->second.first + 1;
		currentDate			= common::date("1399.2.1");
		yearCurrentTech	= landYears[nextLevel - 1];
		yearNextTech		= landYears[nextLevel];
		while (currentDate < startDate)
		{
			level += (rate / 12) / (yearNextTech - yearCurrentTech);
			if (level >= nextLevel)
			{
				yearCurrentTech	= landYears[nextLevel];
				nextLevel++;
				yearNextTech		= landYears[nextLevel];
			}

			currentDate.month++;
			if (currentDate.month > 12)
			{
				currentDate.year++;
				currentDate.month -= 12;
			}
		}
		startingLevels[LAND] = level;

		startingTechLevels.insert(make_pair(groupsItr->first, startingLevels));
	}
}


double EU3Tech::getGovernmentTech(string techGroup) const
{
	map<string, vector<double> >::const_iterator techItr = startingTechLevels.find(techGroup);
	if (techItr != startingTechLevels.end())
	{
		return techItr->second[GOVERNMENT];
	}
	else
	{
		return 0.0f;
	}
}


double EU3Tech::getProductionTech(string techGroup) const
{
	map<string, vector<double> >::const_iterator techItr = startingTechLevels.find(techGroup);
	if (techItr != startingTechLevels.end())
	{
		return techItr->second[PRODUCTION];
	}
	else
	{
		return 0.0f;
	}
}


double EU3Tech::getTradeTech(string techGroup) const
{
	map<string, vector<double> >::const_iterator techItr = startingTechLevels.find(techGroup);
	if (techItr != startingTechLevels.end())
	{
		return techItr->second[TRADE];
	}
	else
	{
		return 0.0f;
	}
}


double EU3Tech::getNavalTech(string techGroup) const
{
	map<string, vector<double> >::const_iterator techItr = startingTechLevels.find(techGroup);
	if (techItr != startingTechLevels.end())
	{
		return techItr->second[NAVAL];
	}
	else
	{
		return 0.0f;
	}
}


double EU3Tech::getLandTech(string techGroup) const
{
	map<string, vector<double> >::const_iterator techItr = startingTechLevels.find(techGroup);
	if (techItr != startingTechLevels.end())
	{
		return techItr->second[LAND];
	}
	else
	{
		return 0.0f;
	}
}


double EU3Tech::getGovernmentBaseCost(common::date startDate, int level) const
{
	int startDiff = governmentYears[level] - startDate.year;
	if (startDiff < 0)
	{
		startDiff = 0;
	}

	double earlyPenalty = 0.2 * (governmentYears[level] - startDate.year);
	if (earlyPenalty < 0)
	{
		earlyPenalty = 0.0f;
	}
	earlyPenalty += 1.0f;

	int levelDiff = governmentYears[level] - governmentYears[level - 1];
	double cost = (0.63*startDiff + 105)*levelDiff*earlyPenalty;
	return cost;
}


double EU3Tech::getProductionBaseCost(common::date startDate, int level) const
{
	int startDiff = productionYears[level] - startDate.year;
	if (startDiff < 0)
	{
		startDiff = 0;
	}

	double earlyPenalty = 0.2 * (governmentYears[level] - startDate.year);
	if (earlyPenalty < 0)
	{
		earlyPenalty = 0.0f;
	}
	earlyPenalty += 1.0f;

	int levelDiff = productionYears[level] - productionYears[level - 1];
	double cost = (0.63*startDiff + 105)*levelDiff*earlyPenalty;
	return cost;
}


double EU3Tech::getTradeBaseCost(common::date startDate, int level) const
{
	int startDiff = tradeYears[level] - startDate.year;
	if (startDiff < 0)
	{
		startDiff = 0;
	}

	double earlyPenalty = 0.2 * (governmentYears[level] - startDate.year);
	if (earlyPenalty < 0)
	{
		earlyPenalty = 0.0f;
	}
	earlyPenalty += 1.0f;

	int levelDiff = tradeYears[level] - tradeYears[level - 1];
	double cost = (0.63*startDiff + 105)*levelDiff*earlyPenalty;
	return cost;
}


double EU3Tech::getNavalBaseCost(common::date startDate, int level) const
{
	int startDiff = navalYears[level] - startDate.year;
	if (startDiff < 0)
	{
		startDiff = 0;
	}

	double earlyPenalty = 0.2 * (governmentYears[level] - startDate.year);
	if (earlyPenalty < 0)
	{
		earlyPenalty = 0.0f;
	}
	earlyPenalty += 1.0f;

	int levelDiff = navalYears[level] - navalYears[level - 1];
	double cost = (0.63*startDiff + 105)*levelDiff*earlyPenalty;
	return cost;
}


double EU3Tech::getLandBaseCost(common::date startDate, int level) const
{
	int startDiff = landYears[level] - startDate.year;
	if (startDiff < 0)
	{
		startDiff = 0;
	}

	double earlyPenalty = 0.2 * (governmentYears[level] - startDate.year);
	if (earlyPenalty < 0)
	{
		earlyPenalty = 0.0f;
	}
	earlyPenalty += 1.0f;

	int levelDiff = landYears[level] - landYears[level - 1];
	double cost = (0.63*startDiff + 105)*levelDiff*earlyPenalty;
	return cost;
}


double EU3Tech::getGroupModifier(string group) const
{
	map<string, pair<int, double> >::const_iterator groupItr = groups.find(group);
	if (groupItr != groups.end())
	{
		return groupItr->second.second;
	}
	else
	{
		log("Error: could not find tech group %s\n", group.c_str());
		return 1.0f;
	}
}
