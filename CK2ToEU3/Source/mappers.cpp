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



#include "mappers.h"
#include "Log.h"
#include "Configuration.h"
#include "Parsers\Object.h"
#include "CK2World\CK2Version.h"
#include "CK2World\CK2Title.h"
#include "CK2World\CK2Province.h"
#include "CK2World\CK2Barony.h"
#include "CK2World\CK2Religion.h"
#include "CK2World\Character\CK2Character.h"
#include "EU3World\EU3Country.h"
#include <cstdio>
#include <sstream>



provinceMapping initProvinceMap(Object* obj, CK2Version* version)
{
	provinceMapping mapping;
	provinceMapping::iterator mapIter;

	vector<IObject*> leaves = obj->getLeaves();

	if (leaves.size() < 1)
	{
		log ("Error: No province mapping definitions loaded.\n");
		printf("Error: No province mapping definitions loaded.\n");
		return mapping;
	}

	for (vector<IObject*>::iterator itr = leaves.begin(); itr < leaves.end(); itr++)
	{
		if (  CK2Version( (*itr)->getKey() ) > *version  )
		{
			continue;
		}
		vector<IObject*> data = (*itr)->getLeaves();

		for (unsigned int i = 0; i < data.size(); i++)
		{
			vector<int> CK2nums;
			vector<int> EU3nums;

			vector<IObject*> maps = data[i]->getLeaves();

			for (unsigned int j = 0; j < maps.size(); j++)
			{
				if (maps[j]->getKey().compare("ck2") == 0)
				{
					CK2nums.push_back(atoi(maps[j]->getLeaf().c_str()));
				}
				else if (maps[j]->getKey().compare("eu3") == 0)
				{
					EU3nums.push_back(atoi(maps[j]->getLeaf().c_str()));
				}
				else
				{
					log("Warning: unknown data while mapping provinces.\n");
				}
			}

			if (CK2nums.size() == 0)
			{
				CK2nums.push_back(0);
			}

			for (unsigned int k = 0; k < EU3nums.size(); k++)
			{
				pair< int, vector<int> > insertMe;
				insertMe.first = EU3nums[k];
				insertMe.second = CK2nums;
				mapping.insert(insertMe);
			}
		}
	}

	return mapping;
}


inverseProvinceMapping invertProvinceMap(provinceMapping& provinceMap)
{
	inverseProvinceMapping retval;
	for (provinceMapping::iterator j = provinceMap.begin(); j != provinceMap.end(); j++)
	{
		for (unsigned int k = 0; k < j->second.size(); k++)
		{
			retval[j->second[k]].push_back(j->first);
		}
	}
	return retval;
}


continentMapping initContinentMap(Object* obj)
{
	continentMapping continentMap;

	vector<IObject*> continentObjs = obj->getLeaves();
	for (vector<IObject*>::iterator continentItr = continentObjs.begin(); continentItr < continentObjs.end(); continentItr++)
	{
		string continent = (*continentItr)->getKey();
		vector<string> provinceObjs = (*continentItr)->getTokens();
		for (vector<string>::iterator provItr = provinceObjs.begin(); provItr != provinceObjs.end(); provItr++)
		{
			continentMap.insert(make_pair( atoi(provItr->c_str()), continent ));
		}
	}

	return continentMap;
}


adjacencyMapping initAdjacencyMap()
{
	FILE* adjacenciesBin;
	fopen_s( &adjacenciesBin, (Configuration::getEU3Path() + "\\map\\cache\\adjacencies.bin").c_str(), "rb");
	if (adjacenciesBin == NULL)
	{
		log("Error: Could not open adjacencies.bin\n");
		exit(1);
	}

	adjacencyMapping adjacencyMap;
	while( !feof(adjacenciesBin) )
	{
		int numAdjacencies;
		if (fread(&numAdjacencies, sizeof(numAdjacencies), 1, adjacenciesBin) != 1)
		{
			break;
		}
		vector<adjacency> adjacencies;
		for(int i = 0; i < numAdjacencies; i++)
		{
			adjacency newAdjacency;
			fread(&newAdjacency, sizeof(newAdjacency), 1, adjacenciesBin);
			adjacencies.push_back(newAdjacency);
		}
		adjacencyMap.push_back(adjacencies);
	}
	fclose(adjacenciesBin);

	/*FILE* adjacenciesData;
	fopen_s(&adjacenciesData, "adjacenciesData.csv", "w");
	fprintf(adjacenciesData, "From,To,Type,Via,Unknown1,PathX,PathY\n");
	for (unsigned int from = 0; from < adjacencyMap.size(); from++)
	{
		vector<adjacency> adjacencies = adjacencyMap[from];
		for (unsigned int i = 0; i < adjacencies.size(); i++)
		{
			fprintf(adjacenciesData, "%d,%d,%d,%d,%d,%d,%d\n", from, adjacencies[i].to, adjacencies[i].type, adjacencies[i].via, adjacencies[i].unknown1, adjacencies[i].pathX, adjacencies[i].pathY);
		}
	}
	fclose(adjacenciesData);*/

	return adjacencyMap;
}


vector<string> processBlockedNations(Object* obj)
{
	vector<string> blockedNations;

	vector<IObject*> leaves = obj->getLeaves();
	if (leaves.size() < 1)
	{
		return blockedNations;
	}

	vector<IObject*> nations = leaves[0]->getLeaves();
	for (unsigned int i = 0; i < nations.size(); i++)
	{
		blockedNations.push_back(nations[i]->getLeaf());
	}

	return blockedNations;
}


cultureMapping initCultureMap(Object* obj) // TODO: consider cleaning up the distinguishers
{
	cultureMapping cultureMap;
	vector<IObject*> links = obj->getLeaves();

	for (vector<IObject*>::iterator i = links.begin(); i != links.end(); i++)
	{
		vector<IObject*>			cultures	= (*i)->getLeaves();

		vector<string>				srcCultures;
		string						dstCulture;
		vector< distinguisher > distinguishers;
		for (vector<IObject*>::iterator j = cultures.begin(); j != cultures.end(); j++)
		{
			if ( (*j)->getKey() == "eu3" )
			{
				dstCulture = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "ck2" )
			{
				srcCultures.push_back( (*j)->getLeaf() );
			}
			if ( (*j)->getKey() == "de_jure" )
			{
				distinguisher newD;
				newD.first	= DTDeJure;
				newD.second	= (*j)->getLeaf();
				distinguishers.push_back(newD);
			}
			if ( (*j)->getKey() == "kingdom_culture" )
			{
				distinguisher newD;
				newD.first	= DTKingdomCulture;
				newD.second	= (*j)->getLeaf();
				distinguishers.push_back(newD);
			}
			if ( (*j)->getKey() == "religion" )
			{
				distinguisher newD;
				newD.first	= DTReligion;
				newD.second	= (*j)->getLeaf();
				distinguishers.push_back(newD);
			}
			if ( (*j)->getKey() == "hre_member" )
			{
				distinguisher newD;
				newD.first	= DTHREMember;
				newD.second	= (*j)->getLeaf();
				distinguishers.push_back(newD);
			}
		}

		for (vector<string>::iterator j = srcCultures.begin(); j != srcCultures.end(); j++)
		{
			cultureStruct rule;
			rule.srcCulture		= (*j);
			rule.dstCulture		= dstCulture;
			rule.distinguishers	= distinguishers;
			cultureMap.push_back(rule);
		}
	}

	return cultureMap;
}


string determineEU3Culture(string CK2Culture, const cultureMapping& cultureMap, const CK2Province* srcProvince)
{
	if (CK2Culture[0] == '"')
	{
		CK2Culture = CK2Culture.substr(1, CK2Culture.size() - 2);
	}
	for (cultureMapping::const_iterator itr = cultureMap.begin(); itr < cultureMap.end(); itr++)
	{
		if (itr->srcCulture != CK2Culture)
		{
			continue;
		}

		bool matchConditions = true;
		for (vector<distinguisher>::const_iterator DTItr = itr->distinguishers.begin(); DTItr < itr->distinguishers.end(); DTItr++)
		{
			switch (DTItr->first)
			{
				case DTDeJure:
					{
						bool subCondition = false;
						if (srcProvince->getBaronies().empty())
							break;
						CK2Title* title = srcProvince->getBaronies()[0]->getTitle()->getLiege();
						while(title != NULL)
						{
							if (title->getTitleString() == DTItr->second)
							{
								subCondition = true;
								break;
							}
							title = title->getDeJureLiege();
						}
						matchConditions = subCondition;
					}
					break;
				case DTKingdomCulture:
					{
						if (srcProvince->getBaronies().empty())
							break;
						CK2Title* kingdomTitle = srcProvince->getBaronies()[0]->getTitle();
						while (!kingdomTitle->isIndependent())
						{
							kingdomTitle = kingdomTitle->getLiege();
						}
						if (kingdomTitle->getLastHolder()->getCulture() != DTItr->second)
						{
							matchConditions = false;
						}
					}
					break;
				case DTReligion:
					if (srcProvince->getReligion()->getName() != DTItr->second)
					{
						matchConditions = false;
					}
					break;
				case DTHREMember:
					{
						bool subCondition = false;
						if (srcProvince->getBaronies().empty())
							break;
						CK2Title* kingdomTitle = srcProvince->getBaronies()[0]->getTitle();
						while (!kingdomTitle->isIndependent())
						{
							if (kingdomTitle->getLiegeString() == "e_hre")
							{
								subCondition = true;
								break;
							}
							kingdomTitle = kingdomTitle->getLiege();
						}
						matchConditions = subCondition;
					}
					break;
			}
		}
		if (!matchConditions)
		{
			continue;
		}
		return itr->dstCulture;
	}

	return "";
}


void addCultureGroupMappings(Object* obj, cultureGroupMapping& map)
{
	if (obj == NULL)
	{
		return;
	}
	vector<IObject*> groups = obj->getLeaves();
	for (vector<IObject*>::iterator groupsItr = groups.begin(); groupsItr < groups.end(); groupsItr++)
	{
		vector<IObject*> cultures = (*groupsItr)->getLeaves();
		for (vector<IObject*>::iterator culturesItr = cultures.begin(); culturesItr < cultures.end(); culturesItr++)
		{
			map.insert( make_pair((*culturesItr)->getKey(), (*groupsItr)->getKey()) );
		}
	}
}


religionMapping initReligionMap(Object* obj)
{
	religionMapping religionMap;
	vector<IObject*> links = obj->getLeaves();

	for (vector<IObject*>::iterator i = links.begin(); i != links.end(); i++)
	{
		vector<IObject*>			link	= (*i)->getLeaves();

		string srcReligion;
		string dstReligion;
		for (vector<IObject*>::iterator j = link.begin(); j != link.end(); j++)
		{
			if ( (*j)->getKey() == "eu3" )
			{
				dstReligion = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "ck2" )
			{
				srcReligion = (*j)->getLeaf();
			}
		}

		religionMap.insert( make_pair(srcReligion, dstReligion) );
	}

	return religionMap;
}


void addReligionGroupMappings(Object* obj, religionGroupMapping& map)
{
	vector<IObject*> groups = obj->getLeaves();
	for (vector<IObject*>::iterator groupsItr = groups.begin(); groupsItr < groups.end(); groupsItr++)
	{
		vector<IObject*> religions = (*groupsItr)->getLeaves();
		for (vector<IObject*>::iterator religionsItr = religions.begin(); religionsItr < religions.end(); religionsItr++)
		{
			map.insert( make_pair((*religionsItr)->getKey(), (*groupsItr)->getKey()) );
		}
	}
}


tradeGoodMapping initTradeGoodMapping(Object* obj)
{
	tradeGoodMapping tradeGoodMap;

	vector<IObject*> tradeGoodObjs = obj->getLeaves();
	for(vector<IObject*>::iterator tradeItr = tradeGoodObjs.begin(); tradeItr < tradeGoodObjs.end(); tradeItr++)
	{
		tradeGood newTradeGood;
		string tradeGoodName = (*tradeItr)->getKey();
		newTradeGood.basePrice = atoi( (*tradeItr)->getValue("base_price")[0]->getLeaf().c_str() );

		vector<IObject*> supplyObjs = (*tradeItr)->getValue("supply");
		vector<IObject*> modifierObjs;
		if (supplyObjs.size() > 0)
		{
			modifierObjs = supplyObjs[0]->getLeaves();
		}
		for(vector<IObject*>::iterator modsItr = modifierObjs.begin(); modsItr < modifierObjs.end(); modsItr++)
		{
			double factor		= 1.0f;
			string modifier	= "";
			vector<IObject*> leaves = (*modsItr)->getLeaves();
			for(vector<IObject*>::iterator itr = leaves.begin(); itr < leaves.end(); itr++)
			{
				if ( (*itr)->getKey() == "factor" )
				{
					factor = atof( (*itr)->getLeaf().c_str() );
				}
				else if ( (*itr)->getKey() == "looted" )
				{
					modifier = "looted";
				}
				else if ( (*itr)->getKey() == "is_blockaded" )
				{
					modifier = "blockaded";
				}
				else if ( (*itr)->getKey() == "NOT" )
				{
					vector<IObject*> subObjs = (*itr)->getLeaves();
					if (subObjs[0]->getKey() == "controlled_by")
					{
						modifier = "other controller";
					}
					else if (subObjs[0]->getKey() == "owner")
					{
						vector<IObject*> subSubObjs = subObjs[0]->getLeaves();
						if (subSubObjs[0]->getKey() == "stability")
						{
							modifier = "stability not " + subSubObjs[0]->getLeaf();
						}
						else if (subSubObjs[0]->getKey() == "aristocracy_plutocracy")
						{
							modifier = "aristocracy_plutocracy not " + subSubObjs[0]->getLeaf();
						}
						else if (subSubObjs[0]->getKey() == "land_naval")
						{
							modifier = "land_naval not " + subSubObjs[0]->getLeaf();
						}
					}
				}
				else if ( (*itr)->getKey() == "owner" )
				{
					vector<IObject*> subObjs = (*itr)->getLeaves();
					if (subObjs[0]->getKey() == "serfdom_freesubjects")
					{
						modifier = "serfdom_freesubjects " + subObjs[0]->getLeaf();
					}
					else if (subObjs[0]->getKey() == "mercantilism_freetrade")
					{
						modifier = "mercantilism_freetrade " + subObjs[0]->getLeaf();
					}
					else if (subObjs[0]->getKey() == "aristocracy_plutocracy")
					{
						modifier = "aristocracy_plutocracy " + subObjs[0]->getLeaf();
					}
					else if (subObjs[0]->getKey() == "has_country_modifier")
					{
						modifier = "has country modifier " + subObjs[0]->getLeaf();
					}
				}
				else if ( (*itr)->getKey() == "units_in_province" )
				{
					modifier = "units_in_province";
				}
			}
			newTradeGood.supplyModifiers.push_back(make_pair(modifier, factor));
		}

		vector<IObject*> demandObjs = (*tradeItr)->getValue("demand");
		if (demandObjs.size() > 0)
		{
			modifierObjs = demandObjs[0]->getLeaves();
		}
		for(vector<IObject*>::iterator modsItr = modifierObjs.begin(); modsItr < modifierObjs.end(); modsItr++)
		{
			double			factor = 1.0f;
			vector<string>	modifiers;
			vector<IObject*> leaves = (*modsItr)->getLeaves();
			for(vector<IObject*>::iterator itr = leaves.begin(); itr < leaves.end(); itr++)
			{
				if ( (*itr)->getKey() == "factor" )
				{
					factor = atof( (*itr)->getLeaf().c_str() );
				}
				else if ( (*itr)->getKey() == "owner" )
				{
					vector<IObject*> subObjs = (*itr)->getLeaves();
					for(vector<IObject*>::iterator subItr = subObjs.begin(); subItr < subObjs.end(); subItr++)
					{
						if ((*subItr)->getKey() == "war")
						{
							string modifier = "war";
							modifiers.push_back(modifier);
						}
						else if ((*subItr)->getKey() == "stability")
						{
							string modifier = "stability is " + (*subItr)->getLeaf();
							modifiers.push_back(modifier);
						}
						else if ((*subItr)->getKey() == "government")
						{
							string modifier = "government is " + (*subItr)->getLeaf();
							modifiers.push_back(modifier);
						}
						else if ((*subItr)->getKey() == "land_naval")
						{
							string modifier = "land_naval is " + (*subItr)->getLeaf();
							modifiers.push_back(modifier);
						}
						else if ((*subItr)->getKey() == "big_ship")
						{
							string modifier = "has big_ships " + (*subItr)->getLeaf();
							modifiers.push_back(modifier);
						}
						else if ((*subItr)->getKey() == "artillery")
						{
							string modifier = "has artillery " + (*subItr)->getLeaf();
							modifiers.push_back(modifier);
						}
						else if ((*subItr)->getKey() == "infantry")
						{
							string modifier = "has infantry " + (*subItr)->getLeaf();
							modifiers.push_back(modifier);
						}
						else if ((*subItr)->getKey() == "land_tech")
						{
							string modifier = "has land tech " + (*subItr)->getLeaf();
							modifiers.push_back(modifier);
						}
						else if ((*subItr)->getKey() == "has_country_modifier")
						{
							string modifier = "has country modifier " + (*subItr)->getLeaf();
							modifiers.push_back(modifier);
						}
						else if ((*subItr)->getKey() == "gold_income_percentage")
						{
							string modifier = "gold income percent " + (*subItr)->getLeaf();
							modifiers.push_back(modifier);
						}
						else if ((*subItr)->getKey() == "prestige")
						{
							string modifier = "prestige " + (*subItr)->getLeaf();
							modifiers.push_back(modifier);
						}
						else if ((*subItr)->getKey() == "num_of_trade_embargos")
						{
							string modifier = "trade embargos " + (*subItr)->getLeaf();
							modifiers.push_back(modifier);
						}
						else if ( (*subItr)->getKey() == "NOT" )
						{
							vector<IObject*> subSubObjs = (*subItr)->getLeaves();
							for(vector<IObject*>::iterator subSubItr = subSubObjs.begin(); subSubItr < subSubObjs.end(); subSubItr++)
							{
								if ((*subSubItr)->getKey() == "trade_income_percentage")
								{
									string modifier = "trade income percentage not " + (*subSubItr)->getLeaf();
									modifiers.push_back(modifier);
								}
								else if ((*subSubItr)->getKey() == "technology_group")
								{
									string modifier = "tech group not " + (*subSubItr)->getLeaf();
									modifiers.push_back(modifier);
								}
								else if ((*subSubItr)->getKey() == "land_naval")
								{
									string modifier = "land_naval not " + (*subSubItr)->getLeaf();
									modifiers.push_back(modifier);
								}
								else if ((*subSubItr)->getKey() == "mercantilism_freetrade")
								{
									string modifier = "mercantilism_freetrade not " + (*subSubItr)->getLeaf();
									modifiers.push_back(modifier);
								}
							}
						}
					}
				}
				else if ( (*itr)->getKey() == "NOT" )
				{
					vector<IObject*> subObjs = (*itr)->getLeaves();
					for(vector<IObject*>::iterator subItr = subObjs.begin(); subItr < subObjs.end(); subItr++)
					{
						if ((*subItr)->getKey() == "owner")
						{
							vector<IObject*> subSubObjs = (*subItr)->getLeaves();
							if (subSubObjs[0]->getKey() == "war")
							{
								string modifier = "not war";
								modifiers.push_back(modifier);
							}
							else if (subSubObjs[0]->getKey() == "num_of_ports")
							{
								string modifier = "owner has this many ports: " + subSubObjs[0]->getLeaf();
								modifiers.push_back(modifier);
							}
							else if (subSubObjs[0]->getKey() == "quality_quantity")
							{
								string modifier = "quality_quantity not " + subSubObjs[0]->getLeaf();
								modifiers.push_back(modifier);
							}
							else if (subSubObjs[0]->getKey() == "serfdom_freesubjects")
							{
								string modifier = "serfdom_freesubjects not " + subSubObjs[0]->getLeaf();
								modifiers.push_back(modifier);
							}
							else if (subSubObjs[0]->getKey() == "land_naval")
							{
								string modifier = "land_naval not " + subSubObjs[0]->getLeaf();
								modifiers.push_back(modifier);
							}
							else if (subSubObjs[0]->getKey() == "technology_group")
							{
								string modifier = "tech group not " + subSubObjs[0]->getLeaf();
								modifiers.push_back(modifier);
							}
						}
						else if ((*subItr)->getKey() == "trade_goods")
						{
							string modifier = "trade goods not " + (*subItr)->getLeaf();
							modifiers.push_back(modifier);
						}
						else if ((*subItr)->getKey() == "has_building")
						{
							string modifier = "has not building " + (*subItr)->getLeaf();
							modifiers.push_back(modifier);
						}
					}
				}
				else if ((*itr)->getKey() == "OR")
				{
					vector<IObject*> subObjs = (*itr)->getLeaves();
					for(vector<IObject*>::iterator subItr = subObjs.begin(); subItr < subObjs.end(); subItr++)
					{
						if ((*subItr)->getKey() == "trade_goods")
						{
							string modifier = "OR trade goods " + (*subItr)->getLeaf();
							modifiers.push_back(modifier);
						}
					}
				}
				else if ( (*itr)->getKey() == "has_building" )
				{
					string modifier = "has building " + (*itr)->getLeaf();
					modifiers.push_back(modifier);
				}
				else if ( (*itr)->getKey() == "trade_goods" )
				{
					string modifier = "trade good " + (*itr)->getLeaf();
					modifiers.push_back(modifier);
				}
				else if ( (*itr)->getKey() == "revolt_risk" )
				{
					string modifier = "revolt risk is " + (*itr)->getLeaf();
					modifiers.push_back(modifier);
				}
				else if ( (*itr)->getKey() == "religion" )
				{
					string modifier = "religion is " + (*itr)->getLeaf();
					modifiers.push_back(modifier);
				}
				else if ( (*itr)->getKey() == "religion_group" )
				{
					string modifier = "religion group is " + (*itr)->getLeaf();
					modifiers.push_back(modifier);
				}
				else if ( (*itr)->getKey() == "is_overseas" )
				{
					string modifier = "overseas";
					modifiers.push_back(modifier);
				}
				else if ( (*itr)->getKey() == "is_capital" )
				{
					string modifier = "capital";
					modifiers.push_back(modifier);
				}
				else if ( (*itr)->getKey() == "looted" )
				{
					string modifier = "looted";
					modifiers.push_back(modifier);
				}
				else if ( (*itr)->getKey() == "port" )
				{
					string modifier = "port";
					modifiers.push_back(modifier);
				}
				else if ( (*itr)->getKey() == "continent" )
				{
					string modifier = "continent " + (*itr)->getLeaf();
					modifiers.push_back(modifier);
				}
			}
			newTradeGood.demandModifiers.push_back(make_pair(modifiers, factor));
		}

		tradeGoodMap.insert(make_pair(tradeGoodName, newTradeGood));
	}
	return tradeGoodMap;
}


cultureRuleOverrideMapping initCultureRuleOverrideMap(Object* obj, cultureRuleOverrideMapping cultureRules)
{
	cultureRuleOverrideMapping mapping;

	vector<IObject*> links = obj->getLeaves();

	if (links.size() < 1)
	{
		return mapping;
	}

	for (vector<IObject*>::iterator itr = links.begin(); itr < links.end(); itr++)
	{
		vector<IObject*> data = (*itr)->getLeaves();

		mapping.insert(make_pair(data[0]->getLeaf(), cultureRules[data[1]->getLeaf()]));
	}

	return mapping;
}


localeOverrideMapping initLocaleOverrideMap(Object* obj)
{
	localeOverrideMapping mapping;

	vector<IObject*> links = obj->getLeaves();

	if (links.size() < 1)
	{
		return mapping;
	}

	for (vector<IObject*>::iterator i = links.begin(); i != links.end(); i++)
	{
		//vector<Object*> data = (*itr)->getLeaves();

		//mapping.insert( make_pair(data[0]->getLeaf(), data[1] -> getLeaf()) );

		vector<IObject*>			locales	= (*i)->getLeaves();

		if (locales.size()<=1) continue;
		string						code;
		string						english;
		string						french;
		string						german;
		string						polski;
		string						spanish;
		string						italian;
		string						swedish;
		string						czech;
		string						hungarian;
		string						dutch;
		string						portugese;
		string						russian;
		string						finnish;
		string						default_language;
		stringstream				ss;
		for (vector<IObject*>::iterator j = locales.begin(); j != locales.end(); j++)
		{
			if ( (*j)->getKey() == "code" )
			{
				code = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "english" )
			{
				english = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "french" )
			{
				french = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "german" )
			{
				german = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "polski" )
			{
				polski = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "spanish" )
			{
				spanish = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "italian" )
			{
				italian = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "swedish" )
			{
				swedish = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "czech" )
			{
				czech = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "hungarian" )
			{
				hungarian = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "dutch" )
			{
				dutch = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "portugese" )
			{
				portugese = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "russian" )
			{
				russian = (*j)->getLeaf();
			}
			if ( (*j)->getKey() == "finnish" )
			{
				finnish = (*j)->getLeaf();
			}
		}
		default_language = code;
		for (unsigned int i = 0; i<default_language.size(); i++) // c_country_name -> c_Country_Name
		{
			if (default_language[i] == '_')
			{
				default_language[i+1] = toupper(default_language[i+1]);
			}
		}
		default_language = default_language.substr(2); // c_Country_Name -> Country_Name
		const string adj = "_Adj";
		if ( default_language != adj &&
			 default_language.size() > adj.size() &&
			 default_language.substr( default_language.size() - adj.size() ) == "_Adj" ) // Remove "_Adj" from default value;
		{
		   default_language = default_language.substr(0, default_language.size() - adj.size());
		}
		if (english.size() == 0)
		{
			english = default_language;
		}
		if (french.size() == 0)
		{
			french = default_language;
		}
		if (german.size() == 0)
		{
			german = default_language;
		}
		if (spanish.size() == 0)
		{
			spanish = default_language;
		}
		ss << ";" << english;
		ss << ";" << french;
		ss << ";" << german;
		ss << ";" << polski;
		ss << ";" << spanish;
		ss << ";" << italian;
		ss << ";" << swedish;
		ss << ";" << czech;
		ss << ";" << hungarian;
		ss << ";" << dutch;
		ss << ";" << portugese;
		ss << ";" << russian;
		ss << ";" << finnish;
		ss << ";x;;;;;;;;;;;;\r\n";
		string str = ss.str();
		replace(str.begin(), str.end(), '_',' '); // Country_Name = Country Name

		mapping.insert( make_pair(code, str) );
	}

	return mapping;
}
