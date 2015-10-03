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

#include <iostream>
#include <boost\bind.hpp>
#include <boost\foreach.hpp>
#include "CK2World.h"
#include "..\Log.h"
#include "..\Configuration.h"
#include "..\Parsers\Object.h"
#include "Common\Date.h"
#include "CK2Building.h"
#include "CK2Version.h"
#include "CK2Title.h"
#include "CK2Province.h"
#include "CK2Barony.h"
#include "CK2World\Character\CK2Character.h"
#include "CK2Dynasty.h"
#include "CK2Trait.h"
#include "CK2Techs.h"
#include "CK2War.h"



CK2World::CK2World(boost::shared_ptr<LogBase> logger) : logOutput(logger)
{
	buildingFactory = NULL;

	version = NULL;
	endDate = common::date();
	independentTitles.clear();
	hreMembers.clear();
	dynasties.clear();
	characters.clear();
	traits.clear();
	potentialTitles.clear();
	titles.clear();
	hreTitle = NULL;
	provinces.clear();
	baronies.clear();
	wars.clear();
}


void CK2World::init(IObject* obj, const cultureGroupMapping& cultureGroupMap)
{
	buildingFactory = new CK2BuildingFactory(&cultureGroupMap);

	// get version
	vector<IObject*> versionObj = obj->getValue("version");
	if (versionObj.size() > 0)
	{
		version = new CK2Version( versionObj[0]->getLeaf() );
	}
	else
	{
		log("\tError: Unknown version format.\n");
		printf("\tError: Unknown version format.\n");
		version = new CK2Version("0.0");
	}

	// get conversion date
	vector<IObject*> dateObj = obj->getValue("date");
	if (dateObj.size() > 0)
	{
		common::date newDate( dateObj[0]->getLeaf() );
		endDate = newDate;
	}
	else
	{
		common::date newDate("1399.10.14");
	}

	// get dynasties
	printf("\tGetting dynasties from save\n");
	vector<IObject*> dynastyLeaves = obj->getValue("dynasties");
	dynastyLeaves = dynastyLeaves[0]->getLeaves();
	for (unsigned int i = 0; i < dynastyLeaves.size(); i++)
	{
		int number = atoi( dynastyLeaves[i]->getKey().c_str() );
		CK2Dynasty* newDynasty = new CK2Dynasty(static_cast<Object*>(dynastyLeaves[i]));
		dynasties.insert( make_pair(number, newDynasty) );
	}
	CK2Dynasty* newDynasty = new CK2Dynasty(0, "Lowborn");
	dynasties.insert( make_pair(0, newDynasty) );

	// get characters
	printf("\tGetting characters\n");
	vector<IObject*> characterLeaves = obj->getValue("character");
	characterLeaves = characterLeaves[0]->getLeaves();
	for (unsigned int i = 0; i < characterLeaves.size(); i++)
	{
		int number = atoi( characterLeaves[i]->getKey().c_str() );
		CK2Character* newCharacter = new CK2Character(static_cast<Object*>(characterLeaves[i]), dynasties,
			traits, endDate);
		characters.insert( make_pair(number, newCharacter) );
	}

	printf("\tCreating family trees\n");
	for (map<int, CK2Character*>::iterator i = characters.begin(); i != characters.end(); i++)
	{
		i->second->setParents(characters);
	}

	printf("\tGetting opinion modifiers\n");
	vector<IObject*> leaves = obj->getLeaves();
	for (vector<IObject*>::iterator itr = leaves.begin(); itr != leaves.end(); ++itr)
	{
		string key = (*itr)->getKey();
		if (key.substr(0, 4) == "rel_")
		{
			int charId = atoi(key.c_str() + 4);
			map<int, CK2Character*>::const_iterator chitr = characters.find(charId);
			if (chitr == characters.end())
			{
				log("%s bad LHS character ID %d\n", key.c_str(), charId);
				continue;
			}
			chitr->second->readOpinionModifiers(static_cast<Object*>(*itr));
		}
	}

	printf("\tGetting wars\n");
	leaves = obj->getLeaves();
	for (vector<IObject*>::iterator itr = leaves.begin(); itr != leaves.end(); ++itr)
	{
		string key = (*itr)->getKey();
		if (key == "active_war")
		{
			CK2War* war = new CK2War(static_cast<Object*>(*itr));
			wars.push_back(war);
			for (vector<int>::iterator witr = war->attackers.begin(); witr != war->attackers.end(); ++witr)
			{
				map<int, CK2Character*>::iterator attacker = characters.find(*witr);
				if (attacker != characters.end())
					attacker->second->addWar(war);
			}
			for (vector<int>::iterator witr = war->defenders.begin(); witr != war->defenders.end(); ++witr)
			{
				map<int, CK2Character*>::iterator defender = characters.find(*witr);
				if (defender != characters.end())
					defender->second->addWar(war);
			}
		}
	}

	// get titles
	printf("\tGetting titles\n");
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		string key = leaves[i]->getKey();
		if ( (key == "title") )
		{
			vector<IObject*> titleList = leaves[i]->getLeaves();
			for (vector<IObject*>::iterator itr = titleList.begin() ; itr != titleList.end(); ++itr)
			{
				key = (*itr)->getKey();
				if (key == "k_seljuk_turks")
				{
					key = "e_seljuk_turks";
				}
				map<string, CK2Title*>::iterator titleItr = potentialTitles.find(key);
				if (titleItr == potentialTitles.end())
				{
					int color[3] = {0,0,0};
					CK2Title* dynTitle = new CK2Title(key, color);
					dynTitle->init(static_cast<Object*>(*itr), characters, buildingFactory);
					if (!dynTitle->isDynamic())
					{
						log("\t\tWarning: tried to create title %s, but it is neither a potential title nor a dynamic title.\n", key.c_str());
						continue;
					}
					titles.insert( make_pair(dynTitle->getTitleString(), dynTitle) );
				}
				else
				{
					titleItr->second->init(static_cast<Object*>(*itr), characters, buildingFactory);
					titles.insert( make_pair(titleItr->second->getTitleString(), titleItr->second) );
				}
			}
		}
		else if ( (key.substr(0, 2) == "e_") || (key.substr(0, 2) == "k_") || (key.substr(0, 2) == "d_") || (key.substr(0, 2) == "c_") || (key.substr(0, 2) == "b_") )
		{
			map<string, CK2Title*>::iterator titleItr = potentialTitles.find(key);
			if (titleItr == potentialTitles.end())
			{
				int color[3] = {0,0,0};
				CK2Title* dynTitle = new CK2Title(key, color);
				dynTitle->init(static_cast<Object*>(leaves[i]), characters, buildingFactory);
				if (!dynTitle->isDynamic())
				{
					log("\t\tWarning: tried to create title %s, but it is neither a potential title nor a dynamic title.\n", key.c_str());
					continue;
				}
				titles.insert( make_pair(dynTitle->getTitleString(), dynTitle) );
			}
			else
			{
				titleItr->second->init(static_cast<Object*>(leaves[i]), characters, buildingFactory);
				titles.insert( make_pair(titleItr->second->getTitleString(), titleItr->second) );
			}
		}
	}

	// set primary titles
	for (map<int, CK2Character*>::iterator i = characters.begin(); i != characters.end(); i++)
	{
		if (!i->second) continue;
		i->second->setPrimaryTitle(titles);
	}

	std::cout << "\tGetting provinces" << std::endl;
	for (unsigned int i = 0; i < leaves.size(); i++)
	{
		string key = leaves[i]->getKey();

		if (atoi(key.c_str()) > 0)
		{
			CK2Province* newProvince = new CK2Province(static_cast<Object*>(leaves[i]), titles, characters,
				buildingFactory, *version);
			provinces.insert( make_pair(atoi(key.c_str()), newProvince) );

			vector<CK2Barony*> newBaronies = newProvince->getBaronies();
			for (unsigned int j = 0; j < newBaronies.size(); j++)
			{
				string title = newBaronies[j]->getTitle()->getTitleString();
				baronies.insert( make_pair(title, newBaronies[j]) );
			}
		}
	}
	if (provinces.empty())
    {
        for (auto provinceObj : obj->getValue("provinces")[0]->getLeaves())
        {
            string key = provinceObj->getKey();

            if (atoi(key.c_str()) > 0)
            {
                CK2Province* newProvince = new CK2Province(static_cast<Object*>(provinceObj), titles, characters,
                    buildingFactory, *version);
                provinces.insert( make_pair(atoi(key.c_str()), newProvince) );

                vector<CK2Barony*> newBaronies = newProvince->getBaronies();
                for (unsigned int j = 0; j < newBaronies.size(); j++)
                {
                    string title = newBaronies[j]->getTitle()->getTitleString();
                    baronies.insert( make_pair(title, newBaronies[j]) );
                }
            }
        }
    }

	// create tree of vassal/liege relationships
	printf("\tRelating vassals and lieges\n");
	string hreTitleString = Configuration::getHRETitle();
	for (map<string, CK2Title*>::iterator i = titles.begin(); i != titles.end(); i++)
	{
		string liege = i->second->getLiegeString();
		if (i->second->getHolder() == NULL)
		{
			continue;
		}
		if (i->first == hreTitleString)
		{
			hreTitle = i->second;
		}
		if (liege == "")
		{
			independentTitles.insert(make_pair(i->first, i->second));
		}
		else if (liege == hreTitleString)
		{
			i->second->addToHRE();
			independentTitles.insert((make_pair(i->first, i->second)));
			hreMembers.insert(make_pair(i->first, i->second));
		}
		else
		{
			map<string, CK2Title*>::iterator title = titles.find(liege);
			if (title == titles.end())
			{
				log("Error: title %s is used in your save as a liege, but does not exist in your CK2 install.\n", liege.c_str());
				continue;
			}
			i->second->setLiege(title->second);
		}
	}

	// create tree of De Jure lieges
	for (map<string, CK2Title*>::iterator i = titles.begin(); i != titles.end(); i++)
	{
		i->second->setDeJureLiege(potentialTitles);
	}

	// merge independent baronies with their de jure liege
	map<string, CK2Title*> newIndependentTitles;
	for (map<string, CK2Title*>::iterator titleItr = independentTitles.begin(); titleItr != independentTitles.end(); titleItr++)
	{
		if (titleItr->first.substr(0, 1) == "b")
		{
			CK2Title* deJureLiege = titleItr->second->getDeJureLiege();
			if (deJureLiege != NULL)
			{
				titleItr->second->setLiege(deJureLiege);
			}
			else
			{
				log("Error: barony %s is independent and has no de jure liege.\n", titleItr->first.c_str());
			}
		}
		else
		{
			newIndependentTitles.insert(*titleItr);
		}
	}
	independentTitles.swap(newIndependentTitles);

	TitleFilter(this).removeDeadTitles();

	// determine heirs
	printf("\tDetermining heirs\n");
	for (map<string, CK2Title*>::iterator i = titles.begin(); i != titles.end(); i++)
	{
		i->second->determineHeir(characters);
	}

	printf("\tSetting employers\n");
	for (map<int, CK2Character*>::iterator i = characters.begin(); i != characters.end(); i++)
	{
		CK2Character* character = i->second;
		if (character != NULL)
		{
			character->setEmployer(characters, baronies);
		}
	}

	printf("\tCalculating state stats\n");
	for (map<int, CK2Character*>::iterator itr = characters.begin(); itr != characters.end(); ++itr)
	{
		CK2Character* character = itr->second;
		if (character != NULL)
		{
			character->setStateStats();
		}
	}

	log("\tThere are a total of %d titles\n", titles.size());
	log("\tThere are a total of %d independent titles\n", independentTitles.size());
	log("\tThere are a total of %d hre members\n", hreMembers.size());
}

void CK2World::addBuildingTypes(Object* obj)
{
	if (obj != NULL)
	{
		CK2BuildingFactory::addBuildingTypes(obj);
	}
}


void CK2World::addDynasties(Object* obj)
{
	if (obj == NULL)
	{
		return;
	}
	vector<IObject*> dynastyLeaves = obj->getLeaves();
	for (unsigned int i = 0; i < dynastyLeaves.size(); i++)
	{
		int number = atoi( dynastyLeaves[i]->getKey().c_str() );
		CK2Dynasty* newDynasty = new CK2Dynasty(static_cast<Object*>(dynastyLeaves[i]));
		dynasties.insert( make_pair(number, newDynasty) );
	}
}


void CK2World::addTraits(Object* obj)
{
	if (obj == NULL)
	{
		return;
	}
	vector<IObject*> traitLeaves = obj->getLeaves();
	int offset = traits.size() + 1;
	for (unsigned int i = 0; i < traitLeaves.size(); i++)
	{
		CK2Trait* newTrait = new CK2Trait(static_cast<Object*>(traitLeaves[i]));
		traits.insert( make_pair(i + offset, newTrait) );
	}
}


void CK2World::addPotentialTitles(Object* obj)
{
	if (obj == NULL)
	{
		return;
	}
	vector<IObject*> leaves = obj->getLeaves();
	for (vector<IObject*>::iterator itr = leaves.begin(); itr < leaves.end(); itr++)
	{
		map<string, CK2Title*>::iterator titleItr = potentialTitles.find( (*itr)->getKey() );
		if (titleItr == potentialTitles.end())
		{
			int color[3] = {0, 0, 0};
			vector<IObject*> colorObjs = (*itr)->getValue("color");
			if (colorObjs.size() > 0)
			{
				color[0] = atoi(colorObjs[0]->getTokens()[0].c_str() );
				color[1] = atoi(colorObjs[0]->getTokens()[1].c_str() );
				color[2] = atoi(colorObjs[0]->getTokens()[2].c_str() );
			}
			CK2Title* newTitle = new CK2Title( (*itr)->getKey(), color);
			potentialTitles.insert( make_pair((*itr)->getKey(), newTitle) );
			titleItr = potentialTitles.find( (*itr)->getKey() );
		}
		else
		{
			log("Note! The CK2World::addPotentialTitles() condition is needed!\n");
		}
		titleItr->second->addDeJureVassals( (*itr)->getLeaves(), potentialTitles, this );
	}
}

void CK2World::addTitle(pair<string, CK2Title*> titleInfo)
{
	titles.insert(titleInfo);
}

void CK2World::mergeTitles()
{
	bool useInheritance = true; // default to "inheritance"
	std::string mergeTitlesSetting = Configuration::getMergeTitles();
	if (mergeTitlesSetting == "never")
		return;
	else if (mergeTitlesSetting == "inheritance")
		useInheritance = true;
	else if (mergeTitlesSetting == "always")
		useInheritance = false;

	for (map<int, CK2Character*>::iterator citr = characters.begin(); citr != characters.end(); ++citr)
	{
		if (!citr->second) continue;
		citr->second->mergeTitles(useInheritance);
	}

	independentTitles.clear();
	for (map<string, CK2Title*>::iterator titleItr = titles.begin(); titleItr != titles.end(); titleItr++)
	{
		if ((titleItr->second->getLiege() == NULL) && (titleItr->second->getHolder() != NULL))
		{
			independentTitles.insert(*titleItr);
		}
	}

	log("\tThere are a total of %d titles\n", titles.size());
	log("\tThere are a total of %d independent titles\n", independentTitles.size());
	log("\tThere are a total of %d hre members\n", hreMembers.size());
}


vector<double> CK2World::getAverageTechLevels(CK2Version& version) const
{
	vector<double> avgTechLevels;
	if (CK2Version("1.10") > version)
	{
		avgTechLevels.resize(TECH_LEGALISM_OLD + 1);
		for (unsigned int i = 0; i <= TECH_LEGALISM_OLD; i++)
		{
			avgTechLevels[i] = 0.0f;
		}
		for(map<int, CK2Province*>::const_iterator provItr = provinces.begin(); provItr != provinces.end(); provItr++)
		{
			vector<double> currentTechLevels = provItr->second->getTechLevels();
			for (unsigned int i = 0; i <= TECH_LEGALISM_OLD; i++)
			{
				avgTechLevels[i] += currentTechLevels[i];
			}
		}
		for (unsigned int i = 0; i <= TECH_LEGALISM_OLD; i++)
		{
			avgTechLevels[i] /= provinces.size();
		}
	}
	else
	{
		avgTechLevels.resize(TECH_LEGALISM + 1);
		for (unsigned int i = 0; i <= TECH_LEGALISM; i++)
		{
			avgTechLevels[i] = 0.0f;
		}
		for(map<int, CK2Province*>::const_iterator provItr = provinces.begin(); provItr != provinces.end(); provItr++)
		{
			vector<double> currentTechLevels = provItr->second->getTechLevels();
			for (unsigned int i = 0; i <= TECH_LEGALISM; i++)
			{
				avgTechLevels[i] += currentTechLevels[i];
			}
		}
		for (unsigned int i = 0; i <= TECH_LEGALISM; i++)
		{
			avgTechLevels[i] /= provinces.size();
		}
	}

	return avgTechLevels;
}

void CK2World::setAllTitles(title_map_t* newTitles)
{
	titles.swap(*newTitles);
}

void CK2World::setIndependentTitles(title_map_t* newTitles)
{
	independentTitles.swap(*newTitles);
}

void CK2World::setHREMembers(title_map_t* newMembers)
{
	hreMembers.swap(*newMembers);
}

TitleFilter::TitleFilter(CK2World* world) : world(world), newTitles(), newIndependentTitles(),
	newHreMembers()
{
}

void TitleFilter::removeDeadTitles()
{
	BOOST_FOREACH(const title_map_t::value_type &title, world->getAllTitles())
	{
		CK2Title *titleInfo = title.second;
		if (titleInfo->hasMapImpact() || titleInfo->hasHolders())
		{
			insertUsedTitle(title);
		}
		else
		{
			LOG(LogLevel::Debug) << "\tRemoving dead title " << title.first << "\n";
		}
	}
	saveTitles();
}

void TitleFilter::insertUsedTitle(const title_map_t::value_type &title)
{
	newTitles.insert(title);
	insertToMappingIfPresent(title, boost::bind(&CK2World::getIndependentTitles, world),
		&newIndependentTitles);
	insertToMappingIfPresent(title, boost::bind(&CK2World::getHREMembers, world), &newHreMembers);
}

void TitleFilter::insertToMappingIfPresent(const title_map_t::value_type &title,
	const boost::function<title_map_t()>& searchedTitleGetter, title_map_t* listToBeAppended)
{
	title_map_t titlesList = searchedTitleGetter();
	if (titlesList.find(title.first) != titlesList.end())
	{
		listToBeAppended->insert(title);
	}
}

void TitleFilter::saveTitles()
{
	world->setAllTitles(&newTitles);
	world->setIndependentTitles(&newIndependentTitles);
	world->setHREMembers(&newHreMembers);
}
