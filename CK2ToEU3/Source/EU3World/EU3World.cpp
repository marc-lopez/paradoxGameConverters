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



#include "EU3World.h"
#include <string>
#include <queue>
#include <algorithm>
#include <io.h>
#include <set>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include "..\Log.h"
#include "..\temp.h"
#include "..\Configuration.h"
#include "..\Parsers\Parser.h"
#include "..\Parsers\Object.h"
#include "..\CK2World\CK2Barony.h"
#include "..\CK2World\CK2Title.h"
#include "..\CK2World\CK2Province.h"
#include "..\CK2World\CK2World.h"
#include "..\CK2World\CK2Religion.h"
#include "..\ModWorld\ModCultureRule.h"
#include "CK2World\Character\CK2Character.h"
#include "EU3Province.h"
#include "EU3Country.h"
#include "EU3Ruler.h"
#include "EU3Advisor.h"
#include "EU3Diplomacy.h"
using namespace std;



EU3World::EU3World(CK2World* _srcWorld, EU3Tech* _techData)
{
	srcWorld		= _srcWorld;
	startDate	= srcWorld->getEndDate();
	techData		= _techData;

	provinces.clear();
	countries.clear();
	convertedCountries.clear();
	advisors.clear();
	mapSpreadStrings.clear();

	if (Configuration::getAdvisors() == "normal")
	{
		options[ADVISORS] = 0;
	}
	else if (Configuration::getAdvisors() == "historical")
	{
		options[ADVISORS] = 1;
	}
	else if (Configuration::getAdvisors() == "event")
	{
		options[ADVISORS] = 2;
	}

	if (Configuration::getLeaders() == "normal")
	{
		options[LEADERS] = 0;
	}
	else if (Configuration::getLeaders() == "historical")
	{
		options[LEADERS] = 1;
	}
	else if (Configuration::getLeaders() == "event")
	{
		options[LEADERS] = 2;
	}

	if (Configuration::getColonists() == "normal")
	{
		options[COLONISTS] = 0;
	}
	else if (Configuration::getColonists() == "free")
	{
		options[COLONISTS] = 1;
	}

	if (Configuration::getMerchants() == "normal")
	{
		options[MERCHANTS] = 0;
	}
	else if (Configuration::getMerchants() == "free")
	{
		options[MERCHANTS] = 1;
	}

	if (Configuration::getMissionaries() == "normal")
	{
		options[MISSIONARIES] = 0;
	}
	else if (Configuration::getMissionaries() == "free")
	{
		options[MISSIONARIES] = 1;
	}

	if (Configuration::getInflation() == "normal")
	{
		options[INFLATION] = 0;
	}
	else if (Configuration::getInflation() == "from gold")
	{
		options[INFLATION] = 1;
	}
	else if (Configuration::getInflation() == "none")
	{
		options[INFLATION] = 2;
	}

	if (Configuration::getColonist_size() == "normal")
	{
		options[COLONIST_SIZE] = 0;
	}
	else if (Configuration::getColonist_size() == "200")
	{
		options[COLONIST_SIZE] = 1;
	}
	else if (Configuration::getColonist_size() == "300")
	{
		options[COLONIST_SIZE] = 2;
	}
	else if (Configuration::getColonist_size() == "400")
	{
		options[COLONIST_SIZE] = 3;
	}

	if (Configuration::getDifficulty() == "very easy")
	{
		options[DIFFICULTY] = 0;
	}
	else if (Configuration::getDifficulty() == "easy")
	{
		options[DIFFICULTY] = 1;
	}
	else if (Configuration::getDifficulty() == "normal")
	{
		options[DIFFICULTY] = 2;
	}
	else if (Configuration::getDifficulty() == "hard")
	{
		options[DIFFICULTY] = 3;
	}
	else if (Configuration::getDifficulty() == "very hard")
	{
		options[DIFFICULTY] = 4;
	}

	if (Configuration::getAI_aggressiveness() == "normal")
	{
		options[AI_AGGRESSIVENESS] = 0;
	}
	else if (Configuration::getAI_aggressiveness() == "low")
	{
		options[AI_AGGRESSIVENESS] = 1;
	}
	else if (Configuration::getAI_aggressiveness() == "high")
	{
		options[AI_AGGRESSIVENESS] = 2;
	}

	if (Configuration::getLand_spread() == "25")
	{
		options[LAND_SPREAD] = 0;
	}
	else if (Configuration::getLand_spread() == "normal (50)")
	{
		options[LAND_SPREAD] = 1;
	}
	else if (Configuration::getLand_spread() == "75")
	{
		options[LAND_SPREAD] = 2;
	}
	else if (Configuration::getLand_spread() == "100")
	{
		options[LAND_SPREAD] = 3;
	}
	else if (Configuration::getLand_spread() == "200")
	{
		options[LAND_SPREAD] = 4;
	}

	if (Configuration::getSea_spread() == "25")
	{
		options[SEA_SPREAD] = 0;
	}
	else if (Configuration::getSea_spread() == "normal (50)")
	{
		options[SEA_SPREAD] = 1;
	}
	else if (Configuration::getSea_spread() == "75")
	{
		options[SEA_SPREAD] = 2;
	}
	else if (Configuration::getSea_spread() == "100")
	{
		options[SEA_SPREAD] = 3;
	}
	else if (Configuration::getSea_spread() == "200")
	{
		options[SEA_SPREAD] = 4;
	}

	if (Configuration::getSpies() == "normal")
	{
		options[SPIES] = 0;
	}
	else if (Configuration::getSpies() == "free")
	{
		options[SPIES] = 1;
	}

	if (Configuration::getLucky_nations() == "historical")
	{
		options[LUCKY_NATIONS] = 0;
	}
	else if (Configuration::getLucky_nations() == "random")
	{
		options[LUCKY_NATIONS] = 1;
	}
	else if (Configuration::getLucky_nations() == "none")
	{
		options[LUCKY_NATIONS] = 2;
	}

	centersOfTrade.clear();
	diplomacy			= new EU3Diplomacy();
	hreEmperor			= NULL;
	japaneseEmperor	= NULL;
	daimyos.clear();
	shogun				= NULL;
	shogunPower			= -1.0f;
}


void EU3World::output(FILE* output)
{
	fprintf(output, "date=\"%s\"\n", startDate.toString().c_str());
	outputTempHeader(output);
	fprintf(output, "gameplaysettings=\n");
	fprintf(output, "{\n");
	fprintf(output, "\tsetgameplayoptions=\n");
	fprintf(output, "\t{\n");
	fprintf(output, "\t\t");
	for (unsigned int i = 0; i < OPTIONS_END; i++)
	{
		fprintf(output, "%d ", options[i]);
	}
	fprintf(output, "\n");
	fprintf(output, "\t}\n");
	fprintf(output, "}\n");
	fprintf(output, "start_date=\"%s\"\n", startDate.toString().c_str());
	outputTempHeader2(output);
	if (hreEmperor != NULL)
	{
		fprintf(output, "emperor=\"%s\"\n", hreEmperor->getTag().c_str());
	}
	fprintf(output, "imperial_influence=20.000\n");
	fprintf(output, "internal_hre_cb=yes\n");
	fprintf(output, "trade=\n");
	fprintf(output, "{\n");
	for (vector<int>::iterator cotItr = centersOfTrade.begin(); cotItr != centersOfTrade.end(); cotItr++)
	{
		fprintf(output, "\tcot=\n");
		fprintf(output, "\t{\n");
		fprintf(output, "\t\tlocation=%d\n", *cotItr);
		fprintf(output, "\t}\n");
	}
	fprintf(output, "}\n");
	for (map<int, EU3Province*>::iterator i = provinces.begin(); i != provinces.end(); i++)
	{
		if (i->second != NULL)
		{
			i->second->output(output);
		}
		else
		{
			log("\tError: EU3 province %d is unmapped!\n", i->first);
		}
	}
	for (map<string, EU3Country*>::iterator i = countries.begin(); i != countries.end(); i++)
	{
		i->second->output(output);
	}
	fprintf(output, "active_advisors=\n");
	fprintf(output, "{\n");
	fprintf(output, "\tnomad_group=\n");
	fprintf(output, "\t{\n");
	for (unsigned int i = 0; i < advisors.size(); i++)
	{
		map<int, EU3Province*>::iterator home = provinces.find(advisors[i]->getLocation());
		if ((home != provinces.end()) &&(home->second->getOwner()->getTechGroup() == "nomad_group"))
		{
			advisors[i]->outputInActive(output);
		}
	}
	fprintf(output, "\twestern=\n");
	fprintf(output, "\t{\n");
	for (unsigned int i = 0; i < advisors.size(); i++)
	{
		map<int, EU3Province*>::iterator home = provinces.find(advisors[i]->getLocation());
		if ((home != provinces.end()) &&(home->second->getOwner()->getTechGroup() == "western"))
		{
			advisors[i]->outputInActive(output);
		}
	}
	fprintf(output, "\teastern=\n");
	fprintf(output, "\t{\n");
	for (unsigned int i = 0; i < advisors.size(); i++)
	{
		map<int, EU3Province*>::iterator home = provinces.find(advisors[i]->getLocation());
		if ((home != provinces.end()) &&(home->second->getOwner()->getTechGroup() == "eastern"))
		{
			advisors[i]->outputInActive(output);
		}
	}
	fprintf(output, "\tottoman=\n");
	fprintf(output, "\t{\n");
	for (unsigned int i = 0; i < advisors.size(); i++)
	{
		map<int, EU3Province*>::iterator home = provinces.find(advisors[i]->getLocation());
		if ((home != provinces.end()) &&(home->second->getOwner()->getTechGroup() == "ottoman"))
		{
			advisors[i]->outputInActive(output);
		}
	}
	fprintf(output, "\tmuslim=\n");
	fprintf(output, "\t{\n");
	for (unsigned int i = 0; i < advisors.size(); i++)
	{
		map<int, EU3Province*>::iterator home = provinces.find(advisors[i]->getLocation());
		if ((home != provinces.end()) &&(home->second->getOwner()->getTechGroup() == "muslim"))
		{
			advisors[i]->outputInActive(output);
		}
	}
	fprintf(output, "\tindian=\n");
	fprintf(output, "\t{\n");
	for (unsigned int i = 0; i < advisors.size(); i++)
	{
		map<int, EU3Province*>::iterator home = provinces.find(advisors[i]->getLocation());
		if ((home != provinces.end()) &&(home->second->getOwner()->getTechGroup() == "indian"))
		{
			advisors[i]->outputInActive(output);
		}
	}
	fprintf(output, "\tchinese=\n");
	fprintf(output, "\t{\n");
	for (unsigned int i = 0; i < advisors.size(); i++)
	{
		map<int, EU3Province*>::iterator home = provinces.find(advisors[i]->getLocation());
		if ((home != provinces.end()) &&(home->second->getOwner()->getTechGroup() == "chinese"))
		{
			advisors[i]->outputInActive(output);
		}
	}
	fprintf(output, "\tsub_saharan=\n");
	fprintf(output, "\t{\n");
	for (unsigned int i = 0; i < advisors.size(); i++)
	{
		map<int, EU3Province*>::iterator home = provinces.find(advisors[i]->getLocation());
		if ((home != provinces.end()) &&(home->second->getOwner()->getTechGroup() == "sub_saharan"))
		{
			advisors[i]->outputInActive(output);
		}
	}
	fprintf(output, "\tnew_world=\n");
	fprintf(output, "\t{\n");
	for (unsigned int i = 0; i < advisors.size(); i++)
	{
		map<int, EU3Province*>::iterator home = provinces.find(advisors[i]->getLocation());
		if ((home != provinces.end()) &&(home->second->getOwner()->getTechGroup() == "new_world"))
		{
			advisors[i]->outputInActive(output);
		}
	}
	fprintf(output, "\tnotechgroup=\n");
	fprintf(output, "\t{\n");
	for (unsigned int i = 0; i < advisors.size(); i++)
	{
		map<int, EU3Province*>::iterator home = provinces.find(advisors[i]->getLocation());
		if ((home != provinces.end()) &&(home->second->getOwner()->getTechGroup() == "notechgroup"))
		{
			advisors[i]->outputInActive(output);
		}
	}
	fprintf(output, "\t}\n");
	fprintf(output, "}\n");

	diplomacy->output(output);

	if (shogunPower != -1.0f)
	{
		fprintf(output, "shogun=\n");
		fprintf(output, "{\n");
		fprintf(output, "\tshogun=%f\n", shogunPower);
		fprintf(output, "\temperor=\n");
		fprintf(output, "\t{\n");
		fprintf(output, "\t\tstatus=0\n");
		fprintf(output, "\t\temperor=1\n");
		fprintf(output, "\t\tback_kampaku=0\n");
		fprintf(output, "\t\tactive=1\n");
		fprintf(output, "\t\tcountry=\"%s\"\n", japaneseEmperor->getTag().c_str());
		fprintf(output, "\t}\n");
		for(unsigned int i = 0; i < daimyos.size(); i++)
		{
			fprintf(output, "\tdaimyo=\n");
			fprintf(output, "\t{\n");
			if (daimyos[i] == shogun)
			{
				fprintf(output, "\t\tstatus=3\n");
			}
			else
			{
				fprintf(output, "\t\tstatus=0\n");
			}
			fprintf(output, "\t\temperor=0\n");
			fprintf(output, "\t\tback_kampaku=0\n");
			fprintf(output, "\t\tactive=1\n");
			fprintf(output, "\t\tcountry=\"%s\"\n", daimyos[i]->getTag().c_str());
			fprintf(output, "\t}\n");
		}
		fprintf(output, "}\n");
	}
}


void EU3World::addHistoricalCountries()
{
	string EU3Loc = Configuration::getEU3Path();

	struct _finddata_t	countryDirData;
	intptr_t					fileListing;
	if ( (fileListing = _findfirst( (EU3Loc + "\\history\\countries\\*").c_str(), &countryDirData)) == -1L)
	{
		log("Error: Could not open country history directory.\n");
		return;
	}
	do
	{
		if (strcmp(countryDirData.name, ".") == 0 || strcmp(countryDirData.name, "..") == 0 )
		{
				continue;
		}

		string filename;
		filename	=  EU3Loc + "\\history\\countries\\";
		filename	+= countryDirData.name;

		string tag;
		tag = string(countryDirData.name).substr(0, 3);
		transform(tag.begin(), tag.end(), tag.begin(), ::toupper);

		if (tag == "REB")
		{
			continue;
		}

		EU3Country* newCountry = new EU3Country(this, tag, filename, startDate, techData);
		countries.insert(make_pair(tag, newCountry));

	} while(_findnext(fileListing, &countryDirData) == 0);
	_findclose(fileListing);

	if (Configuration::getUseConverterMod() == "yes")
	{
		struct _finddata_t	countryDirData;
		intptr_t					fileListing;
		if ( (fileListing = _findfirst( (Configuration::getModPath() + "\\converter\\history\\countries\\*").c_str(), &countryDirData)) == -1L)
		{
			log("Error: Could not open country history directory.\n");
			return;
		}
		do
		{
			if (strcmp(countryDirData.name, ".") == 0 || strcmp(countryDirData.name, "..") == 0 )
			{
					continue;
			}

			string filename;
			filename	=  Configuration::getModPath() + "\\converter\\history\\countries\\";
			filename	+= countryDirData.name;

			string tag;
			tag = string(countryDirData.name).substr(0, 3);
			transform(tag.begin(), tag.end(), tag.begin(), ::toupper);

			if (tag == "REB")
			{
				continue;
			}

			EU3Country* newCountry = new EU3Country(this, tag, filename, startDate, techData);
			countries.insert(make_pair(tag, newCountry));

		} while(_findnext(fileListing, &countryDirData) == 0);
		_findclose(fileListing);
	}
}


void EU3World::setupProvinces(provinceMapping& provinceMap)
{
	for (provinceMapping::iterator i = provinceMap.begin(); i != provinceMap.end(); i++)
	{
		//parse relevant file
		Object* obj;
		char num[5];
		_itoa_s(i->first, num, 5, 10);

		string filename = Configuration::getEU3Path() + "\\history\\provinces\\" + num + "*-*.txt";
		struct _finddata_t	fileData;
		intptr_t					fileListing;
		if ( (fileListing = _findfirst(filename.c_str(), &fileData)) != -1L)
		{
			obj = doParseFile( (Configuration::getEU3Path() + "\\history\\provinces\\" + fileData.name).c_str() );
			if (obj == NULL)
			{
				log("Error: Could not open %s\n", (Configuration::getEU3Path() + "\\history\\provinces\\" + fileData.name).c_str());
				printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "\\history\\provinces\\" + fileData.name).c_str());
				exit(-1);
			}
			_findclose(fileListing);
		}
		else
		{
			obj = new Object("NULL Object");
		}

		//initialize province
		EU3Province* newProvince = new EU3Province(i->first, obj, startDate);

		//add to provinces list
		provinces.insert( make_pair(i->first, newProvince) );
	}
}


void EU3World::convertCountries(map<string, CK2Title*> CK2Titles, const religionMapping& religionMap, const cultureMapping& cultureMap, const inverseProvinceMapping inverseProvinceMap)
{
	// create EU3 nations
	for (map<string, CK2Title*>::iterator titleItr = CK2Titles.begin(); titleItr != CK2Titles.end(); titleItr++)
	{
		// if our source has no holder, no vassals, and no de jure vassals, we can ignore it (completely dead title)
		if (titleItr->second->getHolder() == NULL && titleItr->second->getVassals().empty() && titleItr->second->getDeJureVassals().empty())
			continue;
		EU3Country* newCountry = new EU3Country(titleItr->second, religionMap, cultureMap, inverseProvinceMap);
		newCountry->determineStartingAgents();
		convertedCountries.push_back(newCountry);
	}

	// create vassal/liege relationships
	for (vector<EU3Country*>::iterator countryItr = convertedCountries.begin(); countryItr != convertedCountries.end(); countryItr++)
	{
		CK2Title* CK2Liege = (*countryItr)->getSrcCountry()->getLiege();
		if (CK2Liege != NULL)
		{
			(*countryItr)->addLiege(CK2Liege->getDstCountry());
		}
	}
}


void EU3World::getCultureRules()
{
	// get mod culture rules
    Object* obj = doParseFile( (Configuration::getModPath() + "\\config\\culture_rules.txt").c_str() );
	if (obj == NULL)
	{
		log( ("Error: Could not open" + Configuration::getModPath() + "\\config\\culture_rules.txt\n").c_str() );
		printf("Error: Could not open culture_rules.txt\n");
		exit(-1);
	}

	vector<IObject*> objectList = obj->getLeaves();

	if (objectList.size() >0)
	{
		vector<IObject*> cultureRuleObj = objectList[0]->getLeaves(); // Get culture rules
		for (unsigned int i = 0; i < cultureRuleObj.size(); i++) // Loop through each culture rule
		{
			//initialize culture rule
			ModCultureRule* newCultureRule = new ModCultureRule(cultureRuleObj[i]->getKey(),
				static_cast<Object*>(cultureRuleObj[i]));

			//add to culture rules list
			cultureRules.insert( make_pair(cultureRuleObj[i]->getKey(), newCultureRule) );
		}
	}
}


void EU3World::convertProvinces(provinceMapping& provinceMap, map<int, CK2Province*>& allSrcProvinces, cultureMapping& cultureMap, religionMapping& religionMap, continentMapping& continentMap, const adjacencyMapping& adjacencyMap, const tradeGoodMapping& tradeGoodMap, const religionGroupMapping& EU3ReligionGroupMap, Object* positionObj)
{
	double totalHistoricalBaseTax		= 0.0f;
	double totalHistoricalPopulation = 0.0f;
	double totalHistoricalManpower	= 0.0f;
	for (provinceMapping::const_iterator provItr = provinceMap.begin(); provItr != provinceMap.end(); provItr++)
	{
		if (provItr->second[0] != 0)
		{
			totalHistoricalBaseTax		+= provinces.find(provItr->first)->second->getBaseTax();
			totalHistoricalPopulation	+= provinces.find(provItr->first)->second->getPopulation();
			totalHistoricalManpower		+= provinces.find(provItr->first)->second->getManpower();
		}
	}
	log("\tTotal historical base tax is %f.\n", totalHistoricalBaseTax);
	log("\tTotal historical population is %f.\n", totalHistoricalPopulation);
	log("\tTotal historical manpower is %f.\n", totalHistoricalManpower);

	double totalBaseTaxProxy	= 0.0f;
	double totalPopProxy			= 0.0f;
	double totalManpowerProxy	= 0.0f;
	for (map<int, CK2Province*>::const_iterator srcItr = allSrcProvinces.begin(); srcItr != allSrcProvinces.end(); srcItr++)
	{
		vector<CK2Barony*> baronies = srcItr->second->getBaronies();
		for (vector<CK2Barony*>::const_iterator baronyItr = baronies.begin(); baronyItr != baronies.end(); baronyItr++)
		{
			totalBaseTaxProxy		+= (*baronyItr)->getBaseTaxProxy();
			totalPopProxy			+= (*baronyItr)->getPopProxy();
			totalManpowerProxy	+= (*baronyItr)->getManpowerProxy();
		}
	}

	for(provinceMapping::iterator i = provinceMap.begin(); i != provinceMap.end(); i++)
	{
		if (i->second[0] == -1)
		{
			map<int, EU3Province*>::iterator		provItr	= provinces.find(i->first);
			provItr->second->setOwner(NULL);
			provItr->second->clearCores();
			provItr->second->setPopulation(0);
			continue;
		}
		if (i->second[0] == 0)
		{
			map<int, EU3Province*>::iterator		provItr	= provinces.find(i->first);
			map<string, EU3Country*>::iterator	owner		= countries.find(provItr->second->getOwnerStr());
			if (owner != countries.end())
			{
				provItr->second->setOwner(owner->second);
				owner->second->addProvince(provItr->second);
			}
			else
			{
				provItr->second->setOwner(NULL);
			}

			vector<string> coreStrings = provItr->second->getCoreStrings();
			for (vector<string>::iterator coreItr = coreStrings.begin(); coreItr != coreStrings.end(); coreItr++)
			{
				map<string, EU3Country*>::iterator country = countries.find(*coreItr);
				if (country != countries.end())
				{
					provItr->second->addCore(country->second);
				}
			}

			continue;
		}

		vector<int>	srcProvinceNums = i->second;
		vector<CK2Province*> srcProvinces;
		for (unsigned j = 0; j < srcProvinceNums.size(); j++)
		{
			CK2Province* srcProvince = allSrcProvinces[ srcProvinceNums[j] ];
			if (srcProvince != NULL)
			{
				srcProvinces.push_back(srcProvince);
			}
		}

		vector<CK2Barony*> baronies;
		int numBaronies = 0;
		for (unsigned int j = 0; j < srcProvinces.size(); j++)
		{
			vector<CK2Barony*> srcBaronies = srcProvinces[j]->getBaronies();
			for(unsigned int k = 0; k < srcBaronies.size(); k++)
			{
				baronies.push_back(srcBaronies[k]);
				numBaronies++;
			}
		}

		double	baseTaxProxy	= 0.0f;
		double	popProxy			= 0.0f;
		double	manpowerProxy	= 0.0f;
		bool		inHRE		= false;
		vector< pair<const CK2Title*, int > > owners;	// ownerTitle, numBaronies
		for (unsigned int j = 0; j < baronies.size(); j++)
		{
			baseTaxProxy	+= baronies[j]->getBaseTaxProxy();
			popProxy			+= baronies[j]->getPopProxy();
			manpowerProxy	+= baronies[j]->getManpowerProxy();

			const CK2Title* title = baronies[j]->getTitle();

			bool ownerFound = false;
			for(unsigned int k = 0; k < owners.size(); k++)
			{
				if (owners[k].first == title)
				{
					owners[k].second++;
					ownerFound = true;
				}
			}
			if (!ownerFound)
			{
				owners.push_back( make_pair(title, 1) );
				if (title->isInHRE())
				{
					inHRE = true;
				}
			}
		}

		vector<EU3Country*> cores;
		for (vector<CK2Province*>::iterator provItr = srcProvinces.begin(); provItr != srcProvinces.end(); provItr++)
		{
			vector<CK2Barony*> srcBaronies = (*provItr)->getBaronies();
			if (srcBaronies.size() > 0)
			{
				const CK2Title* current	= srcBaronies[0]->getTitle();
				const CK2Title* next		= current->getDeJureLiege();
				while( (next != NULL) && (next->getTitleString() != Configuration::getHRETitle()) )
				{
					EU3Country* core = next->getDstCountry();
					if (core != NULL)
					{
						cores.push_back(core);
					}
					current	= next;
					next		= current->getDeJureLiege();
				}
				if ( (next != NULL) && (next->getTitleString() == Configuration::getHRETitle()) )
				{
					inHRE = true;
				}
			}
		}

		if (Configuration::getMultipleProvsMethod() == "average")
		{
			baseTaxProxy	/= srcProvinces.size();
			popProxy			/= srcProvinces.size();
			manpowerProxy	/= srcProvinces.size();
		}

		map<int, EU3Province*>::iterator provItr = provinces.find(i->first);
		provItr->second->convert(i->first, inHRE, srcProvinces, srcProvinceNums, cores);
		for (vector<EU3Country*>::iterator coreItr = cores.begin(); coreItr != cores.end(); coreItr++)
		{
			(*coreItr)->addCore(provItr->second);
		}

		const CK2Title*	greatestOwner;
		int					greatestOwnerNum = 0;
		for (unsigned int j = 0; j < owners.size(); j++)
		{
			provItr->second->addCore(owners[j].first->getDstCountry());
			owners[j].first->getDstCountry()->addCore(provItr->second);
			if (owners[j].second > greatestOwnerNum)
			{
				greatestOwner		= owners[j].first;
				greatestOwnerNum	= owners[j].second;
			}
		}
		if (owners.size() > 0)
		{
			provItr->second->setOwner(greatestOwner->getDstCountry());
			provItr->second->setSrcOwner(greatestOwner);
			greatestOwner->getDstCountry()->addProvince(provItr->second);
			provItr->second->setContinent(continentMap[provItr->first]);
			if ( continentMap[greatestOwner->getDstCountry()->getCapital()] == continentMap[provItr->first])
			{
				provItr->second->setSameContinent(true);
			}
			else
			{
				provItr->second->setSameContinent(false);
			}
		}

		if (Configuration::getBasetax() == "converted")
		{
			provItr->second->setBaseTax(totalHistoricalBaseTax * baseTaxProxy / totalBaseTaxProxy);
		}
		else if (Configuration::getBasetax() == "blended")
		{
			double blendAmount = atof( Configuration::getBasetaxblendamount().c_str() );
			provItr->second->setBaseTax( (blendAmount * provItr->second->getBaseTax()) + ((1 - blendAmount) * totalHistoricalBaseTax * baseTaxProxy / totalBaseTaxProxy) );
		}
		if (Configuration::getPopulation() == "converted")
		{
			provItr->second->setPopulation(totalHistoricalPopulation * popProxy / totalPopProxy);
		}
		else if (Configuration::getPopulation() == "blended")
		{
			double blendAmount = atof( Configuration::getPopulationblendamount().c_str() );
			provItr->second->setPopulation( (blendAmount * provItr->second->getPopulation()) + ((1 - blendAmount) * totalHistoricalPopulation * popProxy / totalPopProxy) );
		}
		else if (Configuration::getPopulation() == "historical")
		{
			if (provItr->second->getPopulation() < 1000.0f)
			{
				provItr->second->setPopulation(1000.0f);
			}
		}
		if (Configuration::getManpower() == "converted")
		{
			provItr->second->setManpower(totalHistoricalManpower * manpowerProxy / totalManpowerProxy);
		}
		if (Configuration::getManpower() == "blended")
		{
			double blendAmount = atof( Configuration::getManpowerblendamount().c_str() );
			provItr->second->setManpower( (blendAmount * provItr->second->getManpower()) + ((1 - blendAmount) * totalHistoricalManpower * manpowerProxy / totalManpowerProxy) );
		}
		provItr->second->determineCulture(cultureMap, srcProvinces, baronies);
		provItr->second->determineReligion(religionMap, srcProvinces);
	}

	//find all coastal provinces
	vector<IObject*> provPositionObj = positionObj->getLeaves();
	for (unsigned int i = 0; i < provPositionObj.size(); i++)
	{
		vector<IObject*> portObjs = provPositionObj[i]->getValue("port");
		if (portObjs.size() > 0)
		{
			map<int, EU3Province*>::iterator provItr = provinces.find( atoi(provPositionObj[i]->getKey().c_str()) );
			if (provItr != provinces.end())
			{
				provItr->second->setCoastal(true);
			}
		}
	}

	// find all land connections to capitals
	for (map<string, EU3Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
	{
		map<int, EU3Province*>	openProvinces = provinces;
		queue<int>					goodProvinces;

		map<int, EU3Province*>::iterator openItr = openProvinces.find(countryItr->second->getCapital());
		if (openItr == openProvinces.end())
		{
			continue;
		}
		openItr->second->setLandConnection(true);
		goodProvinces.push(openItr->first);
		openProvinces.erase(openItr);

		do
		{
			int currentProvince = goodProvinces.front();
			goodProvinces.pop();
			vector<adjacency> adjacencies = adjacencyMap[currentProvince];
			for (unsigned int i = 0; i < adjacencies.size(); i++)
			{
				map<int, EU3Province*>::iterator openItr = openProvinces.find(i);
				if (openItr == openProvinces.end())
				{
					continue;
				}
				if (!openItr->second->isLand())
				{
					continue;
				}
				if (openItr->second->getOwner() != countryItr->second)
				{
					continue;
				}
				openItr->second->setLandConnection(true);
				goodProvinces.push(openItr->first);
				openProvinces.erase(openItr);
			}
		} while (goodProvinces.size() > 1);
	}

	// Determine supply and demand
	for(map<int, EU3Province*>::iterator itr = provinces.begin(); itr != provinces.end(); itr++)
	{
		if (itr->second->getOwner() == NULL)
		{
			continue;
		}
		itr->second->determineGoodsSupply(tradeGoodMap);
		itr->second->determineGoodsDemand(tradeGoodMap, EU3ReligionGroupMap);
	}

	for(map<int, EU3Province*>::iterator itr = provinces.begin(); itr != provinces.end(); itr++)
	{
		vector<CK2Province*> srcProvinces = itr->second->getSrcProvinces();
		if (srcProvinces.size() > 0)
		{
			bool	textile		= false;
			bool	university	= false;
			bool	fort2			= false;
			for (vector<CK2Province*>::iterator srcItr = srcProvinces.begin(); srcItr != srcProvinces.end(); srcItr++)
			{
				vector<CK2Barony*> baronies = (*srcItr)->getBaronies();
				for (vector<CK2Barony*>::iterator baronyItr = baronies.begin(); baronyItr != baronies.end(); baronyItr++)
				{
					if ((*baronyItr)->getFortLevel() > 6.9)
					{
						fort2 = true;
					}
					if ((*baronyItr)->getTechBonus() > 0.7)
					{
						university = true;
					}
					if ((*baronyItr)->getBaseTaxProxy() > 42.5)
					{
						textile = true;
					}
				}
			}
			if (textile)
			{
				if ((itr->second->getTradeGood() == "wool") || (itr->second->getTradeGood() == "cloth"))
				{
					university = false;
					itr->second->addBuilding("textile");
					log ("\tTextile manu added to province %d\n", itr->first);
				}
			}
			if (university)
			{
				itr->second->addBuilding("university");
				log ("\tUniversity added to province %d\n", itr->first);
			}
			if (fort2)
			{
				itr->second->addBuilding("fort2");
				log ("\tFort 2 added to province %d\n", itr->first);
			}
			else
			{
				itr->second->addBuilding("fort1");
			}
		}
	}
}


void EU3World::addAcceptedCultures()
{
	for(map<string, EU3Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
	{
		countryItr->second->addAcceptedCultures();
	}
}


void EU3World::convertAdvisors(inverseProvinceMapping& inverseProvinceMap, provinceMapping& provinceMap, CK2World& srcWorld)
{
	// CK2 Advisors
	map<string, CK2Title*> titles = srcWorld.getAllTitles();
	map<int, CK2Character*> rulers;
	for (map<string, CK2Title*>::iterator i = titles.begin(); i != titles.end(); i++)
	{
		CK2Character* ruler = i->second->getHolder();
		if (ruler != NULL)
		{
			rulers.insert( make_pair(ruler->getNum(), ruler) );
		}
	}

	for (map<int, CK2Character*>::iterator i = rulers.begin(); i != rulers.end(); i++)
	{
		CK2Character** srcAdvisors = i->second->getAdvisors();
		for (unsigned int i = 0; i < 5; i++)
		{
			if (srcAdvisors[i] != NULL)
			{
				EU3Advisor*	newAdvisor		= new EU3Advisor(srcAdvisors[i], inverseProvinceMap, provinces);
				int			CK2Location		= srcAdvisors[i]->getLocationNum();

				vector<int>		provinceNums;
				provinceNums.clear();
				if (CK2Location > 0)
				{
					provinceNums = inverseProvinceMap[CK2Location];
				}

				EU3Province*	advisorHome = NULL;
				if (provinceNums.size() > 0)
				{
					advisorHome = provinces[ provinceNums[0] ];
					newAdvisor->setLocation( provinceNums[0] );
				}
				if (advisorHome != NULL)
				{
					advisorHome->addAdvisor(newAdvisor);
					advisors.push_back(newAdvisor);
				}
			}
		}
	}

	// ROTW Advisors
	string EU3Loc = Configuration::getEU3Path();

	struct _finddata_t	advisorDirData;
	intptr_t					fileListing;
	if ( (fileListing = _findfirst( (EU3Loc + "\\history\\advisors\\*").c_str(), &advisorDirData)) == -1L)
	{
		log("\tError: Could not open advisors history directory.\n");
		return;
	}
	do
	{
		if (strcmp(advisorDirData.name, ".") == 0 || strcmp(advisorDirData.name, "..") == 0 )
		{
				continue;
		}

		string filename;
		filename = advisorDirData.name;

		Object* obj;
		obj = doParseFile((Configuration::getEU3Path() + "/history/advisors/" + filename).c_str());
		if (obj == NULL)
		{
			log("Error: Could not open %s\n", (Configuration::getEU3Path() + "/history/advisors/" + filename).c_str());
			printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "/history/advisors/" + filename).c_str());
			exit(-1);
		}

		vector<IObject*> advisorsObj = obj->getLeaves();
		for (unsigned int i = 0; i < advisorsObj.size(); i++)
		{
			EU3Advisor* newAdvisor = new EU3Advisor(static_cast<Object*>(advisorsObj[i]), provinces);
			if ( (newAdvisor->getStartDate() < startDate) && (startDate < newAdvisor->getDeathDate()) )
			{
				vector<int> srcLocationNums = provinceMap[newAdvisor->getLocation()];
				if ( (srcLocationNums.size() == 1) && (srcLocationNums[0] == 0) )
				{
					provinces[newAdvisor->getLocation()]->addAdvisor(newAdvisor);
					advisors.push_back(newAdvisor);
				}
			}
		}

	} while(_findnext(fileListing, &advisorDirData) == 0);
	_findclose(fileListing);
}


void EU3World::convertTech(const CK2World& srcWorld)
{
	if (Configuration::getTechGroupMethod() == "learningRate")
	{
		vector<double> avgTechLevels = srcWorld.getAverageTechLevels( *(srcWorld.getVersion()) );

		vector<EU3Country*> unlandedCountries;
		double highestLearningScore = 0.0f;
		for (vector<EU3Country*>::iterator countryItr = convertedCountries.begin(); countryItr != convertedCountries.end(); countryItr++)
		{
			if ((*countryItr)->getProvinces().size() > 0)
			{
				(*countryItr)->determineLearningScore();
				if ((*countryItr)->getLearningScore() > highestLearningScore)
				{
					highestLearningScore = (*countryItr)->getLearningScore();
				}
			}
			else
			{
				unlandedCountries.push_back(*countryItr);
			}
		}

		log("Highest tech score is %f\n", highestLearningScore);
		for (vector<EU3Country*>::iterator countryItr = convertedCountries.begin(); countryItr != convertedCountries.end(); countryItr++)
		{
			if ((*countryItr)->getProvinces().size() <= 0)
			{
				continue;
			}
			CK2Religion* religion = (*countryItr)->getSrcCountry()->getLastHolder()->getReligion();
			string title = (*countryItr)->getSrcCountry()->getTitleString();
			if (  ( (title == "e_golden_horde") || (title == "e_il-khanate") || (title == "e_timurids") || (title == "e_mongol_empire") ) && (religion->getGroup() != "christian")  )
			{
				(*countryItr)->setTechGroup("nomad_group");
			}
			else if ((*countryItr)->getLearningScore() <= 0.23 * highestLearningScore)
			{
				(*countryItr)->setTechGroup("muslim");
			}
			else if ((*countryItr)->getLearningScore() <= 0.28 * highestLearningScore)
			{
				(*countryItr)->setTechGroup("ottoman");
			}
			else if ((*countryItr)->getLearningScore() <= 0.48 * highestLearningScore)
			{
				(*countryItr)->setTechGroup("eastern");
			}
			else
			{
				(*countryItr)->setTechGroup("western");
			}
			log("\t,%s,%f,%s\n", (*countryItr)->getTag().c_str(), (*countryItr)->getLearningScore(), (*countryItr)->getTechGroup().c_str());

			(*countryItr)->determineTechLevels(avgTechLevels, techData, *(srcWorld.getVersion()));
		}
		for (vector<EU3Country*>::iterator countryItr = unlandedCountries.begin(); countryItr != unlandedCountries.end(); countryItr++)
		{
			map<int, EU3Province*>::iterator capitalItr = provinces.find( (*countryItr)->getCapital() );
			if (capitalItr != provinces.end())
			{
				(*countryItr)->setTechGroup( capitalItr->second->getOwner()->getTechGroup() );
			}
			else
			{
				(*countryItr)->setTechGroup("western");
				log("\tWarning: %s had no capital, defaulting tech group to western\n", (*countryItr)->getTag().c_str());
			}
			(*countryItr)->determineTechLevels(avgTechLevels, techData, *(srcWorld.getVersion()));
			log("\t,%s,%f,%s\n", (*countryItr)->getTag().c_str(), 0.0F, (*countryItr)->getTechGroup().c_str());
		}

		for(map<string, EU3Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
		{
			countryItr->second->determineTechInvestment(techData, startDate);
		}
	}
	else // (Configuration::getTechGroupMethod() == "culturalTech")
	{
		double catholicTech	=	1.0		+ ( ((double)(startDate.year - 1066) / (1453 - 1066)) * (3.5 - 1.0));
		double greekTech		=	(6.5/3)	+ ( ((double)(startDate.year - 1066) / (1453 - 1066)) * (3.5 - (6.5/3)) );
		double muslimTech		=	(6.2/3)	+ ( ((double)(startDate.year - 1066) / (1453 - 1066)) * (3.0 - (6.2/3)) );
		double otherTech		=	0.0		+ ( ((double)(startDate.year - 1066) / (1453 - 1066)) * (2.5 - 0.0) );
		log("\tCatholicTech: %f\n", catholicTech);
		log("\tgreekTech: %f\n", greekTech);
		log("\tmuslimTech: %f\n", muslimTech);
		log("\totherTech: %f\n", otherTech);

		for (vector<EU3Country*>::iterator countryItr = convertedCountries.begin(); countryItr != convertedCountries.end(); countryItr++)
		{
			// get tech score
			double techScore;
			if ((*countryItr)->getProvinces().size() <= 0)
			{
				map<int, EU3Province*>::iterator capitalItr = provinces.find( (*countryItr)->getCapital() );
				if (capitalItr != provinces.end())
				{
					capitalItr->second->getOwner()->determineTechScore();
					techScore = capitalItr->second->getOwner()->getTechScore();
				}
				else
				{
					(*countryItr)->setTechGroup("western");
					log("\tWarning: %s had no capital, defaulting tech group to western\n", (*countryItr)->getTag().c_str());
					(*countryItr)->determineTechLevels(srcWorld.getAverageTechLevels(*(srcWorld.getVersion())), techData, *(srcWorld.getVersion()));
					log("\t,%s,%f,%s\n", (*countryItr)->getTag().c_str(), 0.0F, (*countryItr)->getTechGroup().c_str());
					continue;
				}
			}
			else
			{
				(*countryItr)->determineTechScore();
				techScore = (*countryItr)->getTechScore();
			}

			// determine which decision category to use
			CK2Title* srcCountry = (*countryItr)->getSrcCountry();
			string category;
			if (srcCountry->getHolder()->getReligion()->getGroup() == "muslim")
			{
				category = "muslim";
			}
			else if (srcCountry->getHolder()->getReligion()->getGroup() == "pagan_group")
			{
				category = "other";
			}
			else if (srcCountry->getHolder()->getReligion()->getGroup() == "zoroastrian_group")
			{
				category = "greek";
			}
			else if (srcCountry->getHolder()->getReligion()->getName() == "miaphysite")
			{
				category = "greek";
			}
			else if (srcCountry->getHolder()->getReligion()->getName() == "monophysite")
			{
				category = "greek";
			}
			else if (srcCountry->getHolder()->getReligion()->getName() == "monothelite")
			{
				category = "greek";
			}
			else if (srcCountry->getHolder()->getCulture() == "greek")
			{
				category = "greek";
			}
			else if (srcCountry->getHolder()->getCulture() == "georgian")
			{
				category = "greek";
			}
			else if (srcCountry->getHolder()->getCulture() == "armenian")
			{
				category = "greek";
			}
			else if	(
							(srcCountry->getHolder()->getReligion()->getName() == "catholic") ||
							(srcCountry->getHolder()->getReligion()->getName() == "cathar") ||
							(srcCountry->getHolder()->getReligion()->getName() == "fraticelli") ||
							(srcCountry->getHolder()->getReligion()->getName() == "waldensian") ||
							(srcCountry->getHolder()->getReligion()->getName() == "lollard")
						)
			{
				if (srcCountry->getHolder()->getCulture() == "german")
				{
					category = "catholic";
				}
				else if (srcCountry->getHolder()->getCulture() == "english")
				{
					category = "catholic";
				}
				else if (srcCountry->getHolder()->getCulture() == "saxon")
				{
					category = "catholic";
				}
				else if (srcCountry->getHolder()->getCulture() == "dutch")
				{
					category = "catholic";
				}
				else if (srcCountry->getHolder()->getCulture() == "frankish")
				{
					category = "catholic";
				}
				else if (srcCountry->getHolder()->getCulture() == "norman")
				{
					category = "catholic";
				}
				else if (srcCountry->getHolder()->getCulture() == "italian")
				{
					category = "catholic";
				}
				else if (srcCountry->getHolder()->getCulture() == "occitan")
				{
					category = "catholic";
				}
				else if (srcCountry->getHolder()->getCulture() == "basque")
				{
					category = "catholic";
				}
				else if (srcCountry->getHolder()->getCulture() == "castillan")
				{
					category = "catholic";
				}
				else if (srcCountry->getHolder()->getCulture() == "catalan")
				{
					category = "catholic";
				}
				else if (srcCountry->getHolder()->getCulture() == "portuguese")
				{
					category = "catholic";
				}
				else if (srcCountry->getHolder()->getCulture() == "irish")
				{
					category = "catholic";
				}
				else if (srcCountry->getHolder()->getCulture() == "scottish")
				{
					category = "catholic";
				}
				else if (srcCountry->getHolder()->getCulture() == "welsh")
				{
					category = "catholic";
				}
				else if (srcCountry->getHolder()->getCulture() == "breton")
				{
					category = "catholic";
				}
				else
				{
					category = "other";
				}
			}
			else
			{
				category = "other";
			}

			//determine tech
			string title = (*countryItr)->getSrcCountry()->getTitleString();
			if (  ( (title == "e_golden_horde") || (title == "e_il-khanate") || (title == "e_timurids") || (title == "e_mexikha") || (title == "e_mongol_empire") ) && (srcCountry->getHolder()->getReligion()->getGroup() != "christian")  )
			{
				(*countryItr)->setTechGroup("nomad_group");
			}
			else if (category == "catholic")
			{
				if (techScore >= greekTech)
				{
					(*countryItr)->setTechGroup("western");
				}
				else
				{
					(*countryItr)->setTechGroup("eastern");
				}
			}
			else if (category == "greek")
			{
				if (techScore >= catholicTech + 1.0)
				{
					(*countryItr)->setTechGroup("western");
				}
				else if (techScore >= muslimTech)
				{
					(*countryItr)->setTechGroup("eastern");
				}
				else
				{
					(*countryItr)->setTechGroup("ottoman");
				}
			}
			else if (category == "other")
			{
				if (techScore >= catholicTech + 0.5)
				{
					(*countryItr)->setTechGroup("western");
				}
				else if (techScore >= muslimTech)
				{
					(*countryItr)->setTechGroup("eastern");
				}
				else
				{
					(*countryItr)->setTechGroup("ottoman");
				}
			}
			else // category == "muslim"
			{
				if (techScore >= catholicTech + 1.0)
				{
					(*countryItr)->setTechGroup("western");
				}
				else if (techScore >= catholicTech + 0.5)
				{
					(*countryItr)->setTechGroup("eastern");
				}
				else if (techScore >= greekTech)
				{
					(*countryItr)->setTechGroup("ottoman");
				}
				else
				{
					(*countryItr)->setTechGroup("muslim");
				}
			}
			log("\t,%s,%f,%s\n", (*countryItr)->getTag().c_str(), techScore, (*countryItr)->getTechGroup().c_str());
		}
	}

	for(map<string, EU3Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
	{
		countryItr->second->determineTechInvestment(techData, startDate);
		countryItr->second->setPreferredUnitType();
	}
}


#define MAX_PRESTIGE 50.0
void EU3World::convertGovernments()
{
	// find prestige factor
	double maxScore = 0.0;
	for (vector<EU3Country*>::iterator itr = convertedCountries.begin(); itr < convertedCountries.end(); itr++)
	{
		CK2Character* ruler = (*itr)->getSrcCountry()->getHolder();
		if (ruler)
		{
			double score = ruler->getTotalScore();
			if (score > maxScore)
				maxScore = score;
		}
	}
	double prestigeFactor = maxScore / MAX_PRESTIGE;

	for (vector<EU3Country*>::iterator itr = convertedCountries.begin(); itr < convertedCountries.end(); itr++)
	{
		(*itr)->determineGovernment(prestigeFactor);
	}
}


void EU3World::convertEconomies(const cultureGroupMapping& cultureGroups, const tradeGoodMapping& tradeGoodMap)
{
	// get goods supply and demand
	map<string, double> goodsSupply;
	map<string, double> goodsDemand;
	for (map<int, EU3Province*>::iterator provItr = provinces.begin(); provItr != provinces.end(); provItr++)
	{
		provItr->second->addSupplyContribution(goodsSupply);
		provItr->second->addDemandContribution(goodsDemand);
	}
	for (map<string, double>::iterator goodItr = goodsSupply.begin(); goodItr != goodsSupply.end(); goodItr++)
	{
		log("\tSupply of %s: %f\n", goodItr->first.c_str(), goodItr->second);
		if (goodItr->second < 0.01)
		{
			goodItr->second = 0.01;
		}
		if (goodItr->second > 2.00)
		{
			goodItr->second = 2.00;
		}
	}
	for (map<string, double>::iterator goodItr = goodsDemand.begin(); goodItr != goodsDemand.end(); goodItr++)
	{
		log("\tDemand for %s: %f\n", goodItr->first.c_str(), goodItr->second);
		if (goodItr->second < 0.01)
		{
			goodItr->second = 0.01;
		}
		if (goodItr->second > 2.00)
		{
			goodItr->second = 2.00;
		}
	}

	// find prices for various goods
	map<string, double> unitPrices;
	for (tradeGoodMapping::const_iterator tradeItr = tradeGoodMap.begin(); tradeItr != tradeGoodMap.end(); tradeItr++)
	{
		map<string, double>::iterator supplyItr = goodsSupply.find(tradeItr->first);
		if (supplyItr == goodsSupply.end())
		{
			LOG(LogLevel::Debug) << "Error: no supply for trade good " << tradeItr->first << ".\n";
			continue;
		}
		map<string, double>::iterator demandItr = goodsDemand.find(tradeItr->first);
		if (demandItr == goodsDemand.end())
		{
		    LOG(LogLevel::Debug) << "Error: no demand for trade good " << tradeItr->first << ".\n";
			continue;
		}
		double price = tradeItr->second.basePrice * (2.25 - supplyItr->second) * demandItr->second;
		unitPrices.insert(make_pair(tradeItr->first, price));
	}

	// determine economies
	for (map<string, EU3Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
	{
		if (countryItr != countries.end())
		{
			countryItr->second->determineEconomy(cultureGroups, unitPrices);
		}
	}
}


void EU3World::assignTags(Object* rulesObj, vector<string>& blockedNations, const provinceMapping& provinceMap, const religionMapping& religionMap, const cultureMapping& cultureMap, const inverseProvinceMapping& inverseProvinceMap, CK2Version& version)
{
	LOG(LogLevel::Debug) << "Total converted EU3 countries: " << convertedCountries.size() << "\n";

	removeUnusedCountries();
	LOG(LogLevel::Debug) << "Total converted EU3 countries after removing unused ones: " << convertedCountries.size() << "\n";

	int leftoverCountries = convertedCountries.size();
	int initialScore = atoi( Configuration::getVassalScore().c_str() );
	vector< tuple<EU3Country*, EU3Country*, string, string, int> > mappings;
	vector<EU3Country*> modCountries;
	set<string> mappedTags;
	while(leftoverCountries > 0)
	{
		vector<EU3Country*> independentCountries;
		for (vector<EU3Country*>::iterator convertedItr = convertedCountries.begin(); convertedItr != convertedCountries.end(); convertedItr++)
		{
			if ((*convertedItr)->getLiege() == NULL)
			{
				independentCountries.push_back(*convertedItr);
			}
		}
		for (vector<EU3Country*>::iterator independentItr = independentCountries.begin(); independentItr != independentCountries.end(); independentItr++)
		{
			vector<EU3Country*> absorbedCountries = (*independentItr)->convertVassals(initialScore, diplomacy, version);
			for (vector<EU3Country*>::iterator absorbedItr = absorbedCountries.begin(); absorbedItr != absorbedCountries.end(); absorbedItr++)
			{
				for(vector<EU3Country*>::iterator convertedItr = convertedCountries.begin(); convertedItr != convertedCountries.end(); convertedItr++)
				{
					if (*absorbedItr == *convertedItr)
					{
						convertedCountries.erase(convertedItr);
						break;
					}
				}
			}
		}
		LOG(LogLevel::Debug) << "Total converted EU3 countries after absorbing: " << convertedCountries.size() <<
            ". Merge score is " << initialScore << "\n";

		mappings.clear();
		leftoverCountries = matchTags(rulesObj, blockedNations, provinceMap, mappings);
		if (Configuration::getUseConverterMod() == "yes")
		{
			for (vector<EU3Country*>::iterator convertedItr = convertedCountries.begin(); convertedItr != convertedCountries.end(); convertedItr++)
			{
				bool mapped = false;
				for (unsigned int i = 0; i < mappings.size(); i++)
				{
					if (get<2>(mappings[i]) == (*convertedItr)->getSrcCountry()->getTitleString())
					{
						mapped = true;
						mappedTags.insert(get<2>(mappings[i]));
						break;
					}
				}
				if (!mapped)
				{
					modCountries.push_back(*convertedItr);
				}
			}
			leftoverCountries = 0;
		}

		if (leftoverCountries == -1)
		{
			log("Error while matching tags. Aborting!\n");
			exit (-1);
		}
		initialScore++;
		if (initialScore > 5200)
		{
			log("Too many independent CK2 countries (%d left over). Aborting!\n", leftoverCountries);
			exit(-1);
		}
	}

	if (Configuration::getUseConverterMod() == "yes")
	{
		LOG(LogLevel::Debug) << mappedTags.size() << " tags were mapped. " << modCountries.size() << "countries will be added to the mod.\n";
		addModCountries(modCountries, mappedTags, mappings, religionMap, cultureMap, inverseProvinceMap);
	}

	convertedCountries.clear();
	for (vector< tuple<EU3Country*, EU3Country*, string, string, int> >::iterator mappingsItr = mappings.begin(); mappingsItr != mappings.end(); mappingsItr++)
	{
		EU3Country* convertedCountry	= get<0>(*mappingsItr);//CK2TitlesPos->second->getDstCountry();
		EU3Country* historicalCountry	= get<1>(*mappingsItr);//countries[*EU3CountryPos];
		string		CK2TitleStr			= get<2>(*mappingsItr);
		string		EU3TagStr			= get<3>(*mappingsItr);
		int			distance				= get<4>(*mappingsItr);
		historicalCountry->replaceWith(convertedCountry, provinceMap);
		convertedCountries.push_back(historicalCountry);
		if (distance > 0)
		{
		    LOG(LogLevel::Debug) << "\tMapped countries " << CK2TitleStr << " -> " << EU3TagStr << "(#" << distance << ")\n";
		}
		else
		{
		    LOG(LogLevel::Debug) << "\tMapped countries " << CK2TitleStr << " -> " << EU3TagStr << "(fallback)\n";
		}
	}

	determineMapSpread();
	convertHRE();
}


void EU3World::convertDiplomacy(CK2Version& version)
{
	for (map<string, EU3Country*>::iterator itr = countries.begin(); itr != countries.end(); ++itr)
	{
		map<string, EU3Country*>::iterator jtr = itr;
		for (++jtr /*skip myself*/; jtr != countries.end(); ++jtr)
		{
			// Open Markets
			map<int, EU3Province*>::iterator provItr = provinces.find(itr->second->getCapital());
			if (provItr != provinces.end())
			{
				vector<string> discoverers = provItr->second->getDiscoveredBy();
				for (vector<string>::iterator i = discoverers.begin(); i != discoverers.end(); i++)
				{
					if (jtr->first == *i)
					{
						EU3Agreement* agr = new EU3Agreement;
						agr->type = "open_market";
						agr->startDate = date("1.1.1");
						agr->country1 = itr->second;
						agr->country2 = jtr->second;
						diplomacy->addAgreement(agr);
						itr->second->addAgreement(agr);
						jtr->second->addAgreement(agr);
						break;
					}
				}
			}
			provItr = provinces.find(jtr->second->getCapital());
			if (provItr != provinces.end())
			{
				vector<string> discoverers = provItr->second->getDiscoveredBy();
				for (vector<string>::iterator i = discoverers.begin(); i != discoverers.end(); i++)
				{
					if (itr->first == *i)
					{
						EU3Agreement* agr = new EU3Agreement;
						agr->type = "open_market";
						agr->startDate = date("1.1.1");
						agr->country1 = jtr->second;
						agr->country2 = itr->second;
						diplomacy->addAgreement(agr);
						itr->second->addAgreement(agr);
						jtr->second->addAgreement(agr);
						break;
					}
				}
			}

			CK2Title *lhs = (*itr).second->getSrcCountry(), *rhs = (*jtr).second->getSrcCountry();
			if (lhs == NULL || rhs == NULL)
			{
				continue;
			}

			if (!itr->second->hasProvinces() || !jtr->second->hasProvinces())
			{
				continue; // no relations with non-extant countries
			}

			// Personal Unions
			bool rhsDominant = false;
			if (lhs->hasUnionWith(rhs, rhsDominant))
			{
				EU3Agreement* agr = new EU3Agreement;
				agr->type = "union";
				agr->startDate = date("1.1.1");
				if (rhsDominant)
				{
					agr->country1 = jtr->second;
					agr->country2 = itr->second;
				}
				else
				{
					agr->country1 = itr->second;
					agr->country2 = jtr->second;
				}
				diplomacy->addAgreement(agr);
				itr->second->addAgreement(agr);
				jtr->second->addAgreement(agr);
			}

			// Royal Marriages
			if (lhs->hasRMWith(rhs))
			{
				EU3Agreement* agr = new EU3Agreement;
				agr->type = "royal_marriage";
				agr->startDate = date("1.1.1");
				agr->country1 = itr->second;
				agr->country2 = jtr->second;
				diplomacy->addAgreement(agr);
				itr->second->addAgreement(agr);
				jtr->second->addAgreement(agr);
			}

			// Alliances
			if (lhs->hasAllianceWith(rhs))
			{
				EU3Agreement* agr = new EU3Agreement;
				agr->type = "alliance";
				agr->startDate = date("1.1.1");
				agr->country1 = itr->second;
				agr->country2 = jtr->second;
				diplomacy->addAgreement(agr);
				itr->second->addAgreement(agr);
				jtr->second->addAgreement(agr);
			}

			// Relations (bilateral)
			int rel = lhs->getRelationsWith(rhs, version);
			(*itr).second->setRelations((*jtr).second, rel);
			(*jtr).second->setRelations((*itr).second, rel);

			// trade agreements
			if (itr->second->getGovernment() == "merchant_republic")
			{
				int coastalProvinces	= 0;
				int tradePosts			= 0;
				vector<EU3Province*> EU3SrcProvinces = jtr->second->getProvinces();
				for (vector<EU3Province*>::iterator provItr = EU3SrcProvinces.begin(); provItr != EU3SrcProvinces.end(); provItr++)
				{
					if (!(*provItr)->isCoastal())
					{
						continue;
					}
					coastalProvinces++;
					vector<CK2Province*> CK2SrcProvinces = (*provItr)->getSrcProvinces();
					for (vector<CK2Province*>::iterator prov2Itr = CK2SrcProvinces.begin(); prov2Itr != CK2SrcProvinces.end(); prov2Itr++)
					{
						if (((*prov2Itr)->hasTradePost()) && ((*prov2Itr)->getTPOwner() == itr->second->getSrcCountry()))
						{
							tradePosts++;
							break;
						}
					}
				}
				if ((coastalProvinces > 0) && (tradePosts * 2 >= coastalProvinces))
				{
					EU3Agreement* agr = new EU3Agreement;
					agr->type = "trade_agreement";
					agr->startDate = startDate;
					agr->country1 = itr->second;
					agr->country2 = jtr->second;
					diplomacy->addAgreement(agr);
					itr->second->addAgreement(agr);
					jtr->second->addAgreement(agr);
				}
			}
			if (jtr->second->getGovernment() == "merchant_republic")
			{
				int coastalProvinces	= 0;
				int tradePosts			= 0;
				vector<EU3Province*> EU3SrcProvinces = itr->second->getProvinces();
				for (vector<EU3Province*>::iterator provItr = EU3SrcProvinces.begin(); provItr != EU3SrcProvinces.end(); provItr++)
				{
					if (!(*provItr)->isCoastal())
					{
						continue;
					}
					coastalProvinces++;
					vector<CK2Province*> CK2SrcProvinces = (*provItr)->getSrcProvinces();
					for (vector<CK2Province*>::iterator prov2Itr = CK2SrcProvinces.begin(); prov2Itr != CK2SrcProvinces.end(); prov2Itr++)
					{
						if (((*prov2Itr)->hasTradePost()) && ((*prov2Itr)->getTPOwner() == jtr->second->getSrcCountry()))
						{
							tradePosts++;
							break;
						}
					}
				}
				if ((coastalProvinces > 0) && (tradePosts * 2 >= coastalProvinces))
				{
					EU3Agreement* agr = new EU3Agreement;
					agr->type = "trade_agreement";
					agr->startDate = startDate;
					agr->country1 = jtr->second;
					agr->country2 = itr->second;
					diplomacy->addAgreement(agr);
					itr->second->addAgreement(agr);
					jtr->second->addAgreement(agr);
				}
			}
		}
	}
}


void EU3World::convertArmies(const inverseProvinceMapping inverseProvinceMap)
{
	for (map<string, EU3Country*>::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
	{
		countryItr->second->convertArmiesandNavies(inverseProvinceMap, provinces);
	}
}


void EU3World::removeUnusedCountries()
{
	vector<EU3Country*> newConvertedCountries;
	for (vector<EU3Country*>::iterator itr = convertedCountries.begin(); itr != convertedCountries.end(); ++itr)
	{
		if (
				( (*itr)->getSrcCountry()->getHolder() != NULL) &&
				( (*itr)->hasProvinces() || (*itr)->hasCores() || (*itr)->hasVassals())
			)
		{
			newConvertedCountries.push_back(*itr);
		}
		else
		{
			EU3Country* liege = (*itr)->getLiege();
			if (liege != NULL)
			{
				liege->eatVassal(*itr);
			}
			else
			{
				vector<EU3Province*> cores = (*itr)->getCores();
				for (vector<EU3Province*>::iterator coreItr = cores.begin(); coreItr != cores.end(); coreItr++)
				{
					(*coreItr)->removeCore(*itr);
				}
			}
			CK2Title* src = (*itr)->getSrcCountry();
			if (src)
			{
				src->setDstCountry(NULL);
			}
			delete (*itr);
		}
	}
	convertedCountries.swap(newConvertedCountries);
}


int EU3World::matchTags(Object* rulesObj, vector<string>& blockedNations, const provinceMapping& provinceMap, vector< tuple<EU3Country*, EU3Country*, string, string, int> >& mappings)
{
	// get CK2 Titles
	map<string, CK2Title*> CK2Titles;
	for (vector<EU3Country*>::iterator countryItr = convertedCountries.begin(); countryItr != convertedCountries.end(); countryItr++)
	{
		CK2Title* srcTitle = (*countryItr)->getSrcCountry();
		CK2Titles.insert( make_pair(srcTitle->getTitleString(), srcTitle) );
	}

	// get EU3 tags
	set<string> EU3tags;
	for (map<string, EU3Country*>::iterator countryItr =	countries.begin(); countryItr != countries.end(); countryItr++)
	{
		EU3tags.insert(countryItr->first);
	}

	// get rules
	vector<IObject*> leaves = rulesObj->getLeaves();
	if (leaves.size() < 1)
	{
		log ("Error: No country mapping definitions loaded.\n");
		printf("Error: No country mapping definitions loaded.\n");
		return -1;
	}
	vector<IObject*> rules = leaves[0]->getLeaves();
	// match titles and nations
	for (unsigned int i = 0; i < rules.size(); ++i)
	{
		vector<IObject*> rule = rules[i]->getLeaves();
		string			rCK2Title;
		vector<string>	rEU3Tags;
		for (unsigned int j = 0; j < rule.size(); j++)
		{
			if (rule[j]->getKey().compare("CK2") == 0)
			{
				rCK2Title = rule[j]->getLeaf();
			}
			else if (rule[j]->getKey().compare("EU3") == 0)
			{
				rEU3Tags.push_back(rule[j]->getLeaf());
			}
			else
			{
				log("Warning: unknown data while mapping countries: %s.\n", rule[j]->getKey().c_str());
			}
		}

		//find CK2 title from the rule
		map<string, CK2Title*>::iterator CK2TitlesPos = CK2Titles.find(rCK2Title);
		if (CK2TitlesPos == CK2Titles.end())
		{
			continue;
		}
		if (rCK2Title == Configuration::getHRETitle())
		{
			CK2Titles.erase(CK2TitlesPos);
			continue;
		}

		//find EU3 tag from the rule
		set<string>::iterator EU3CountryPos;
		unsigned int distance = 0;
		for (vector<string>::reverse_iterator j = rEU3Tags.rbegin(); j != rEU3Tags.rend(); ++j)
		{
			++distance;
			EU3CountryPos = EU3tags.find(*j);
			if (EU3CountryPos != EU3tags.end())
			{
				break;
			}
		}
		if (EU3CountryPos == EU3tags.end())
		{
			continue;
		}

		//map the countries
		tuple<EU3Country*, EU3Country*, string, string, int> mapping = make_tuple(CK2TitlesPos->second->getDstCountry(), countries[*EU3CountryPos], CK2TitlesPos->first.c_str(), EU3CountryPos->c_str(), distance);
		mappings.push_back(mapping);

		//remove tags from the lists
		CK2Titles.erase(CK2TitlesPos);
		EU3tags.erase(EU3CountryPos);
	}

	for (unsigned int j = 0; j < blockedNations.size(); ++j)
	{
		set<string>::iterator i = EU3tags.find(blockedNations[j]);
		if (i != EU3tags.end())
		{
			EU3tags.erase(i);
			continue;
		}
	}

	if (Configuration::getUseConverterMod() == "no")
	{
		while ( (CK2Titles.size() > 0) && (EU3tags.size() > 0) )
		{
			map<string, CK2Title*>::iterator CK2TitlesPos = CK2Titles.begin();
			if (CK2TitlesPos->first == "e_rebels")
			{
				//mapping.insert(make_pair<string, string>(*CK2TitlesPos, "REB")); // TODO: map rebels nation
				CK2Titles.erase(CK2TitlesPos);
			}
			else if (CK2TitlesPos->first == "e_pirates")
			{
				//mapping.insert(make_pair<string, string>(*CK2TitlesPos, "PIR")); // TODO: map pirates nation
				CK2Titles.erase(CK2TitlesPos);
			}
			else if (CK2TitlesPos->first == Configuration::getHRETitle())
			{
				CK2Titles.erase(CK2TitlesPos);
			}
			else
			{
				set<string>::iterator EU3CountryPos = EU3tags.begin();
				tuple<EU3Country*, EU3Country*, string, string, int> mapping = make_tuple(CK2TitlesPos->second->getDstCountry(), countries[*EU3CountryPos], CK2TitlesPos->first.c_str(), EU3CountryPos->c_str(), 0);
				mappings.push_back(mapping);

				//remove tags from the lists
				CK2Titles.erase(CK2TitlesPos);
				EU3tags.erase(EU3CountryPos);
			}
		}
	}

	return CK2Titles.size();
}


void EU3World::determineMapSpread()
{
	vector<string> converted;
	vector<string> nomadTech;
	vector<string> westernTech;
	vector<string> easternTech;
	vector<string> ottomanTech;
	vector<string> muslimTech;
	vector<string> indianTech;
	vector<string> chineseTech;
	vector<string> subSaharanTech;
	vector<string> newWorldTech;
	for (map<string, EU3Country*>::iterator i = countries.begin(); i != countries.end(); i++)
	{
		string techGroup = i->second->getTechGroup();
		if (i->second->getSrcCountry() != NULL)
		{
			converted.push_back( i->first );
		}
		else if (techGroup == "nomad_group")
		{
			nomadTech.push_back( i->first );
		}
		else if (techGroup == "western")
		{
			westernTech.push_back( i->first );
		}
		else if (techGroup == "eastern")
		{
			easternTech.push_back( i->first );
		}
		else if (techGroup == "ottoman")
		{
			ottomanTech.push_back( i->first );
		}
		else if (techGroup == "muslim")
		{
			muslimTech.push_back( i->first );
		}
		else if (techGroup == "indian")
		{
			indianTech.push_back( i->first );
		}
		else if (techGroup == "chinese")
		{
			chineseTech.push_back( i->first );
		}
		else if (techGroup == "sub_saharan")
		{
			subSaharanTech.push_back( i->first );
		}
		else if (techGroup == "new_world")
		{
			newWorldTech.push_back( i->first );
		}

		vector<string> selfString;
		selfString.push_back( i->first );
		mapSpreadStrings.insert(  make_pair( i->first, selfString )  );
	}

	mapSpreadStrings.insert( make_pair("converted", converted) );
	mapSpreadStrings.insert( make_pair("nomad_group", nomadTech) );
	mapSpreadStrings.insert( make_pair("western", westernTech) );
	mapSpreadStrings.insert( make_pair("eastern", easternTech) );
	mapSpreadStrings.insert( make_pair("ottoman", ottomanTech) );
	mapSpreadStrings.insert( make_pair("muslim", muslimTech) );
	mapSpreadStrings.insert( make_pair("indian", indianTech) );
	mapSpreadStrings.insert( make_pair("chinese", chineseTech) );
	mapSpreadStrings.insert( make_pair("sub_saharan", subSaharanTech) );
	mapSpreadStrings.insert( make_pair("new_world", newWorldTech) );

	for (map<int, EU3Province*>::iterator provItr = provinces.begin(); provItr != provinces.end(); provItr++)
	{
		provItr->second->setDiscoverers(mapSpreadStrings);
	}
}


void EU3World::convertHRE()
{
	if (srcWorld->getHRETitle() == NULL)
	{
		return;
	}

	hreEmperor = srcWorld->getHRETitle()->getHolder()->getPrimaryTitle()->getDstCountry();
	map<string, CK2Title*> hreMembers = srcWorld->getHREMembers();
	vector<CK2Title*> potentialElectors;
	for (map<string, CK2Title*>::iterator itr = hreMembers.begin(); itr != hreMembers.end(); itr++)
	{
		if (itr->second->isIndependent())
		{
			potentialElectors.push_back(itr->second);
		}
	}
	sort(potentialElectors.begin(), potentialElectors.end(), [](CK2Title* a, CK2Title* b) { return a->getHolder()->getTotalScore() < b->getHolder()->getTotalScore(); } );

	if (potentialElectors.size() > 0)
	{
		unsigned int maxElectors = 6;
		if (potentialElectors.size() < maxElectors)
		{
			maxElectors = potentialElectors.size();
		}
		for (unsigned int electors = 0; electors < maxElectors; electors++)
		{
			potentialElectors[electors]->getDstCountry()->setElector(true);
		}
	}
	else
	{
		hreEmperor->setElector(true);
	}
}


#define RULERS 10
#define COMMONERS 30
#define MAX_SHIPNAMES 40
#define MAX_LEADERNAMES 40
#define MULTIPLIER 3
void EU3World::populateCountryFileData(EU3Country* country, cultureRuleOverrideMapping croMap, string titleString)
{
	ModCultureRule *rulingCulture = NULL, *commonCulture = NULL;
	string primaryCulture = country->getPrimaryCulture().c_str();
	string capitalCulture = provinces[country->getCapital()]->getCulture().c_str();

	if (croMap.count(titleString.c_str()) !=0 ) // check if CK2 title is in overrides
	{
		primaryCulture = croMap[titleString.c_str()]->getKey();
		if (cultureRules.count(primaryCulture) == 0)
		{
			log("\tWarning: could not find culture rule for \"%s\"\n",primaryCulture.c_str());
			country->setGraphicalCulture(DEFAULT_GFX);
			return;
		}
		else
		{
			rulingCulture = croMap[titleString.c_str()];
			commonCulture = rulingCulture;
		}
	}
	else
	{
		if (cultureRules.count(primaryCulture) == 0)
		{
			log("\tWarning: could not find culture rule for \"%s\"\n",primaryCulture.c_str());
			country->setGraphicalCulture(DEFAULT_GFX);
			return;
		}
		else
		{
			rulingCulture = cultureRules[primaryCulture];
		}
		if (cultureRules.count(capitalCulture) == 0)
		{
			log("\tWarning: could not find culture rule for \"%s\"\n",capitalCulture.c_str());
			country->setGraphicalCulture(DEFAULT_GFX);
			return;
		}
		else
		{
			commonCulture = cultureRules[capitalCulture];
		}
	}

	// Set graphical culture
	country->setGraphicalCulture(commonCulture->getGraphicalCulture().c_str());

	// Determine names
	string gender = country->getSrcCountry()->getGenderLaw().c_str();
	deque<tuple<string,int>> firstNames;
	vector<string> insertMe;
	int weight = -1;
	double maleRatio = -1;

	if (gender == "agnatic")
	{
		maleRatio = 1;
	}
	if (gender == "cognatic")
	{
		maleRatio = 0.8;
	}
	if (gender == "true_cognatic")
	{
		maleRatio = 0.5;
	}
	if (gender == "enatic_cognatic")
	{
		maleRatio = 0.2;
	}
	if (gender == "enatic")
	{
		maleRatio = 0;
	}
	double rulers = RULERS;
	double commoners = COMMONERS;
	firstNames.clear();
	if (rulingCulture->getKey() == commonCulture->getKey())
	{
		vector<string> nameListM(rulingCulture->getMaleNames());
		vector<string> nameListF(rulingCulture->getFemaleNames());
		vector<string> mixed;
		mixed.reserve(nameListM.size() + nameListF.size());
		mixed.insert(mixed.end(), nameListM.begin(), nameListM.end());
		mixed.insert(mixed.end(), nameListF.begin(), nameListF.end());
		random_shuffle(nameListM.begin(),nameListM.end());
		random_shuffle(nameListF.begin(),nameListF.end());
		random_shuffle(mixed.begin(),mixed.end());

		for (double i=0; i<rulers; i++)
		{
			// Monarch names
			if (i < 0.1*rulers) // First 10%
			{
				weight = (rand()%10+31)*MULTIPLIER;
			}
			else if (i < 0.3*rulers) // Next 20%
			{
				weight = (rand()%10+21)*MULTIPLIER;
			}
			else if (i < 0.6*rulers) // Next 30%
			{
				weight = (rand()%10+11)*MULTIPLIER;
			}
			else // Last 40%
			{
				weight = (rand()%10+1)*MULTIPLIER;
			}
			if (gender=="agnatic" || gender=="cognatic")
			{
				if(i < maleRatio*rulers)
				{
					if (nameListM.empty()) continue;
					firstNames.push_back( make_tuple(nameListM.back(),weight) );
					nameListM.pop_back();
				}
				else
				{
					if (nameListF.empty()) continue;
					firstNames.push_back( make_tuple(nameListF.back(),weight*-1) );
					nameListF.pop_back();
				}
			}
			else if (gender == "true_cognatic")
			{
				if (mixed.empty()) continue;
				string name = mixed.back();
				mixed.pop_back();
				if(find(nameListM.begin(), nameListM.end(), name) != nameListM.end())
				{
					firstNames.push_back( make_tuple(name,weight) );
				}
				else
				{
					firstNames.push_back( make_tuple(name,weight*-1) );
				}
			}
			else if (gender=="enatic" || gender=="enatic_cognatic")
			{
				if(i >= (1-maleRatio)*rulers)
				{
					if (nameListM.empty()) continue;
					firstNames.push_back( make_tuple(nameListM.back(),weight) );
					nameListM.pop_back();
				}
				else
				{
					if (nameListF.empty()) continue;
					firstNames.push_back( make_tuple(nameListF.back(),weight*-1) );
					nameListF.pop_back();
				}
			}
		}
		for (double i=0; i<commoners; i++)
		{
			if(gender=="agnatic" || gender=="cognatic")
			{
				if(i < maleRatio*commoners)
				{
					if (nameListM.empty()) continue;
					firstNames.push_back( make_tuple(nameListM.back(),0) );
					nameListM.pop_back();
				}
				else
				{
					if (nameListF.empty()) continue;
					firstNames.push_back( make_tuple(nameListF.back(),-1) );
					nameListF.pop_back();
				}
			}
			else if (gender == "true_cognatic")
			{
				if (mixed.empty()) continue;
				string name = mixed.back();
				mixed.pop_back();
				if (find(nameListM.begin(), nameListM.end(), name) != nameListM.end())
				{
					firstNames.push_back( make_tuple(name,0) );
				}
				else
				{
					firstNames.push_back( make_tuple(name,-1) );
				}
			}
			else if (gender=="enatic" || gender=="enatic_cognatic")
			{
				if(i >= (1-maleRatio)*commoners)
				{
					if (nameListM.empty()) continue;
					firstNames.push_back( make_tuple(nameListM.back(),0) );
					nameListM.pop_back();
				}
				else
				{
					if (nameListF.empty()) continue;
					firstNames.push_back( make_tuple(nameListF.back(),-1) );
					nameListF.pop_back();
				}
			}
		}
		country->setMonarchNames(firstNames);

		// Leader names
		vector<string> surnames(rulingCulture->getLeaderNames());
		random_shuffle(surnames.begin(),surnames.end());

		for(unsigned int i=0; i < MAX_LEADERNAMES; i++)
		{
			insertMe.push_back(surnames.back());
			surnames.pop_back();
			if (surnames.size() == 0) break;
		}
		country -> setLeaderNames(insertMe);

	}
	else
	{
		// Rulers of a country are foreign
		// Ex: Duchy of Athens (ATH): Italian rulers, Greek commoners

		vector<string> rulingNameListM(rulingCulture->getMaleNames());
		vector<string> rulingNameListF(rulingCulture->getFemaleNames());
		vector<string> rulingMixed;
		rulingMixed.reserve(rulingNameListM.size() + rulingNameListF.size());
		rulingMixed.insert(rulingMixed.end(), rulingNameListM.begin(), rulingNameListM.end());
		rulingMixed.insert(rulingMixed.end(), rulingNameListF.begin(), rulingNameListF.end());
		vector<string> commonNameListM(commonCulture->getMaleNames());
		vector<string> commonNameListF(commonCulture->getFemaleNames());
		vector<string> commonMixed;
		commonMixed.reserve(commonNameListM.size() + commonNameListF.size());
		commonMixed.insert(commonMixed.end(), commonNameListM.begin(), commonNameListM.end());
		commonMixed.insert(commonMixed.end(), commonNameListF.begin(), commonNameListF.end());
		random_shuffle(rulingNameListM.begin(),rulingNameListM.end());
		random_shuffle(rulingNameListF.begin(),rulingNameListF.end());
		random_shuffle(rulingMixed.begin(),rulingMixed.end());
		random_shuffle(commonNameListM.begin(),commonNameListM.end());
		random_shuffle(commonNameListF.begin(),commonNameListF.end());
		random_shuffle(commonMixed.begin(),commonMixed.end());

		for (double i=0; i<rulers; i++)
		{
			// Monarch names
			if (i < 0.1*rulers) // First 10%
			{
				weight = (rand()%10+31)*MULTIPLIER;
			}
			else if (i < 0.3*rulers) // Next 20%
			{
				weight = (rand()%10+21)*MULTIPLIER;
			}
			else if (i < 0.6*rulers) // Next 30%
			{
				weight = (rand()%10+11)*MULTIPLIER;
			}
			else // Last 40%
			{
				weight = (rand()%10+1)*MULTIPLIER;
			}
			if (gender=="agnatic" || gender=="cognatic")
			{
				if(i < maleRatio*rulers)
				{
					if (rulingNameListM.empty())
                    {

                        LOG(LogLevel::Debug) << country->getTag() << " has no male names for rulers\n";
                        continue;
                    }
					firstNames.push_back( make_tuple(rulingNameListM.back(),weight) );
					rulingNameListM.pop_back();
				}
				else
				{
					if (rulingNameListM.empty())
                    {

                        LOG(LogLevel::Debug) << country->getTag() << " has no female names for rulers\n";
                        continue;
                    }
					firstNames.push_back( make_tuple(rulingNameListF.back(),weight*-1) );
					rulingNameListF.pop_back();
				}
			}
			else if (gender == "true_cognatic")
			{
				if (rulingMixed.empty()) continue;
				string name = rulingMixed.back();
				rulingMixed.pop_back();
				if(find(rulingNameListM.begin(), rulingNameListM.end(), name) != rulingNameListM.end())
				{
					firstNames.push_back( make_tuple(name,weight) );
				}
				else
				{
					firstNames.push_back( make_tuple(name,weight*-1) );
				}
			}
			else if (gender=="enatic" || gender=="enatic_cognatic")
			{
				if(i >= (1-maleRatio)*rulers)
				{
					if (rulingNameListM.empty()) continue;
					firstNames.push_back( make_tuple(rulingNameListM.back(),weight) );
					rulingNameListM.pop_back();
				}
				else
				{
					if (rulingNameListF.empty()) continue;
					firstNames.push_back( make_tuple(rulingNameListF.back(),weight*-1) );
					rulingNameListF.pop_back();
				}
			}
		}
		for (double i=0; i<commoners; i++)
		{
			if(gender=="agnatic" || gender=="cognatic")
			{
				if(i < maleRatio*commoners)
				{
					if (commonNameListM.empty()) continue;
					firstNames.push_back( make_tuple(commonNameListM.back(),0) );
					commonNameListM.pop_back();
				}
				else
				{
					if (commonNameListF.empty()) continue;
					firstNames.push_back( make_tuple(commonNameListF.back(),-1) );
					commonNameListF.pop_back();
				}
			}
			else if (gender == "true_cognatic")
			{
				if (commonMixed.empty()) continue;
				string name = commonMixed.back();
				commonMixed.pop_back();
				if (find(commonNameListM.begin(), commonNameListM.end(), name) != commonNameListM.end())
				{
					firstNames.push_back( make_tuple(name,0) );
				}
				else
				{
					firstNames.push_back( make_tuple(name,-1) );
				}
			}
			else if (gender=="enatic" || gender=="enatic_cognatic")
			{
				if(i >= (1-maleRatio)*commoners)
				{
					if (commonNameListM.empty()) continue;
					firstNames.push_back( make_tuple(commonNameListM.back(),0) );
					commonNameListM.pop_back();
				}
				else
				{
					if (commonNameListF.empty()) continue;
					firstNames.push_back( make_tuple(commonNameListF.back(),-1) );
					commonNameListF.pop_back();
				}
			}
		}
		country->setMonarchNames(firstNames);

		// Leader names
		vector<string> rulingSurnames(rulingCulture->getLeaderNames());
		vector<string> commonSurnames(commonCulture->getLeaderNames());
		vector<string> surnames;

		for(unsigned int i=0; i < MAX_LEADERNAMES; i++)
		{
			if (i < 0.8*MAX_LEADERNAMES)
			{
				if (rulingSurnames.size() == 0) continue;
				insertMe.push_back(rulingSurnames.back());
				rulingSurnames.pop_back();
			}
			else
			{
				if (commonSurnames.size() == 0) break;
				insertMe.push_back(commonSurnames.back());
				commonSurnames.pop_back();
			}
		}
		country -> setLeaderNames(insertMe);
	}

	insertMe.clear();
	vector<string> ships(commonCulture->getShipNames());
	random_shuffle(ships.begin(),ships.end());
	for(unsigned int i=0; i < MAX_SHIPNAMES; i++)
	{
		if (ships.size() == 0) break;
		insertMe.push_back(ships.back());
		ships.pop_back();
	}
	country -> setShipNames(insertMe);

	country -> setArmyNames(commonCulture->getArmyNames());

	country -> setFleetNames(commonCulture->getFleetNames());

}


void EU3World::addModCountries(const vector<EU3Country*>& modCountries, set<string> mappedTags, vector< tuple<EU3Country*, EU3Country*, string, string, int> >& mappings, const religionMapping& religionMap, const cultureMapping& cultureMap, const inverseProvinceMapping& inverseProvinceMap)
{
    // get mod culture rules
    getCultureRules();

	// parse overrides file
	Object* obj = doParseFile( (Configuration::getModPath() + "\\config\\overrides.txt").c_str() );
	if (obj == NULL)
	{
		log("Error: Could not open %s\n",(Configuration::getModPath() + "\\config\\overrides.txt").c_str()) ;
		printf("Error: Could not open %s\n",(Configuration::getModPath() + "\\config\\overrides.txt").c_str()) ;
		exit(-1);
	}

	vector<IObject*> cultureRuleOverrideObj = obj->getValue("culture_rule_override");
	cultureRuleOverrideMapping croMap; // Culture Rule Override Mapping
	if (cultureRuleOverrideObj.size() > 0)
	{
		croMap = initCultureRuleOverrideMap(static_cast<Object*>(cultureRuleOverrideObj[0]),cultureRules);
	}

	vector<IObject*> localeOverrideObj = obj->getValue("locale_override");
	localeOverrideMapping locMap; // Locale Override Mapping
	if (localeOverrideObj.size() > 0)
	{
		locMap = initLocaleOverrideMap(static_cast<Object*>(localeOverrideObj[0]));
	}

	// Initialize RNG
	srand((unsigned int)time(NULL));

	// get CK2 localisations
	map<string, string>	localisations;
	struct _finddata_t	data;
	intptr_t					fileListing;
	if ( (fileListing = _findfirst( (Configuration::getCK2Path() + "\\localisation\\*").c_str(), &data)) != -1L)
	{
		do
		{
			if (strcmp(data.name, ".") == 0 || strcmp(data.name, "..") == 0 )
			{
				continue;
			}
			FILE* localisationsFile;
			fopen_s( &localisationsFile, (Configuration::getCK2Path() + "\\localisation\\" + data.name).c_str(), "rb");
			if (localisationsFile == NULL)
			{
				continue;
			}
			while (!feof(localisationsFile))
			{
				char line[4096];
				fgets(line, sizeof(line), localisationsFile);
				if ((line[0] == '#') || (line[0] == '\0'))
				{
					continue;
				}
				string fullLine = line;
				int pos = fullLine.find_first_of(';');
				if (pos == string::npos)
				{
					continue;
				}
				localisations.insert( make_pair(fullLine.substr(0, pos), fullLine.substr(pos, fullLine.size())) );
			}
		} while(_findnext(fileListing, &data) == 0);
		_findclose(fileListing);
	}

	// sort titles by rank
	vector<EU3Country*> sortedCountries;
	for (vector<EU3Country*>::const_iterator countryItr = modCountries.begin(); countryItr != modCountries.end(); countryItr++)
	{
		if ((*countryItr)->getSrcCountry()->getTitleString().substr(0,2) == "e_")
		{
			sortedCountries.push_back(*countryItr);
		}
	}
	for (vector<EU3Country*>::const_iterator countryItr = modCountries.begin(); countryItr != modCountries.end(); countryItr++)
	{
		if ((*countryItr)->getSrcCountry()->getTitleString().substr(0,2) == "k_")
		{
			sortedCountries.push_back(*countryItr);
		}
	}
	for (vector<EU3Country*>::const_iterator countryItr = modCountries.begin(); countryItr != modCountries.end(); countryItr++)
	{
		if ((*countryItr)->getSrcCountry()->getTitleString().substr(0,2) == "d_")
		{
			sortedCountries.push_back(*countryItr);
		}
	}
	for (vector<EU3Country*>::const_iterator countryItr = modCountries.begin(); countryItr != modCountries.end(); countryItr++)
	{
		if ((*countryItr)->getSrcCountry()->getTitleString().substr(0,2) == "c_")
		{
			sortedCountries.push_back(*countryItr);
		}
	}

	FILE* EU3Localisations;
	fopen_s(&EU3Localisations, (Configuration::getModPath() + "\\Converter\\localisation\\converter.csv").c_str(), "a");
	if (EU3Localisations == NULL)
	{
		log("\tError: Could not open %s\\Converter\\localisation\\converter.csv\n", Configuration::getModPath().c_str());
	}

	FILE* resultsFile;
	fopen_s(&resultsFile, "newCountries.txt", "w");
	if (EU3Localisations == NULL)
	{
		log("\tError: Could not open newCountries.txt\n");
	}

	// assign tags
	FILE* countriesList;
	fopen_s(&countriesList, (Configuration::getModPath() + "\\converter\\common\\countries.txt").c_str(), "a");
	fprintf(countriesList, "\n");
	char first	= 'A';
	char second	= 'A';
	char third	= 'A';
	for (vector<EU3Country*>::iterator countryItr = sortedCountries.begin(); countryItr != sortedCountries.end(); countryItr++)
	{
		string titleString = (*countryItr)->getSrcCountry()->getTitleString();

		// determine tag
		string potentialTag = boost::to_upper_copy(titleString.substr(2,3));
		while (potentialTag.size() < 3)
		{
			potentialTag += '_';
		}
		string tag;

		map<string, EU3Country*>::iterator	itr	= countries.find(potentialTag);
		set<string>::iterator					itr2	= mappedTags.find(potentialTag);
		while ((itr != countries.end()) || (itr2 != mappedTags.end()) || (potentialTag == "AUX") || (potentialTag == "CON") || (potentialTag == "NUL") || (potentialTag == "PRN"))
		{
			potentialTag = "";
			potentialTag += first;
			potentialTag += second;
			potentialTag += third;
			third++;
			if (third > 'Z')
			{
				third = 'A';
				second++;
				if (second > 'Z')
				{
					second = 'A';
					first++;
					if (first > 'Z')
					{
						log("\tError: ran out of possible tags!\n");
						exit(0);
					}
				}
			}
			itr	= countries.find(potentialTag);
			itr2	= mappedTags.find(potentialTag);
		}
		tag = potentialTag;
		mappedTags.insert(tag);

		EU3Country* newCountry = new EU3Country((*countryItr)->getSrcCountry(), religionMap, cultureMap, inverseProvinceMap);
		newCountry->setTag(tag);
		countries.insert(make_pair(tag, newCountry));
		mappings.push_back( make_tuple(*countryItr, newCountry, (*countryItr)->getSrcCountry()->getTitleString().c_str(), tag, 1) );

		// Get culture rules for new country

		populateCountryFileData((*countryItr), croMap, titleString);

		// Add localisations
		if (EU3Localisations != NULL)
		{
			map<string, string>::iterator localisation = localisations.find(titleString);
			if (locMap.count(titleString) !=0 ) // check if CK2 title is locale-overriden
			{
				string newLocalisation = tag;
				newLocalisation += locMap[titleString];
				fprintf(EU3Localisations, newLocalisation.c_str());
			}
			else if (localisation == localisations.end())
			{
				log("\tWarning: could not find CK2 localisation for %s\n", titleString.c_str());
				string genLocalisation = titleString;
				stringstream ss;
				for (unsigned int i = 0; i<genLocalisation.size(); i++) // c_country_name -> c_Country_Name
				{
					if (genLocalisation[i] == '_')
					{
						genLocalisation[i+1] = toupper(genLocalisation[i+1]);
					}
				}
				genLocalisation = genLocalisation.substr(2); // c_Country_Name -> Country_Name
				replace(genLocalisation.begin(), genLocalisation.end(), '_',' '); // Country_Name = Country Name
				ss << tag;
				ss << ";" << genLocalisation;	 // English
				ss << ";" << genLocalisation;	 // French
				ss << ";" << genLocalisation;	 // German
				ss << ";;" << genLocalisation;	 // Spanish
				ss << ";;;;;;;;;x\r\n";
				fprintf(EU3Localisations, ss.str().c_str());
			}
			else
			{
				string newLocalisation = tag;
				newLocalisation += localisation->second;
				fprintf(EU3Localisations, newLocalisation.c_str());
			}

			localisation = localisations.find(titleString + "_adj");
			if (locMap.count( (titleString + "_adj") ) !=0 ) // check if CK2 title demonym is locale-overriden
			{
				string newLocalisation = tag;
				newLocalisation += "_ADJ";
				newLocalisation += locMap[(titleString + "_adj")];
				fprintf(EU3Localisations, newLocalisation.c_str());
			}
			else if (localisation == localisations.end())
			{
				string newLocalisation = tag;
				string genLocalisation = titleString;
				stringstream ss;
				log("\tWarning: could not find CK2 localisation for %s\n", (titleString + "_adj").c_str());
				for (unsigned int i = 0; i<genLocalisation.size(); i++) // c_country_name -> c_Country_Name
				{
					if (genLocalisation[i] == '_')
					{
						genLocalisation[i+1] = toupper(genLocalisation[i+1]);
					}
				}
				genLocalisation = genLocalisation.substr(2); // c_Country_Name -> Country_Name
				replace(genLocalisation.begin(), genLocalisation.end(), '_',' '); // Country_Name = Country Name
				ss << tag << "_ADJ";
				ss << ";" << genLocalisation;	 // English
				ss << ";" << genLocalisation;	 // French
				ss << ";" << genLocalisation;	 // German
				ss << ";" << genLocalisation;	 // Spanish
				ss << ";" << genLocalisation;	 // French
				ss << ";;;;;;;;;x\r\n";
				newLocalisation += ss.str();
				fprintf(EU3Localisations, ss.str().c_str());
			}
			else
			{
				string newLocalisation = tag;
				newLocalisation += "_ADJ";
				newLocalisation += localisation->second;
				fprintf(EU3Localisations, newLocalisation.c_str());
			}
		}

		// determine filename
		string filename = Configuration::getEU3Path();
		filename += "\\common\\countries\\";
		filename	+= titleString.substr(2, titleString.size());
		filename += ".txt";
		int number = 0;
		while( boost::filesystem::exists( filename ) )
		{
			number++;
			filename	=  Configuration::getEU3Path();
			filename	+= "\\common\\countries\\";
			filename += titleString.substr(2, titleString.size());
			filename	+= boost::lexical_cast<string>(number);
			filename	+= ".txt";
		}
		filename	=  Configuration::getModPath() + "\\converter\\common\\countries\\";
		filename += titleString.substr(2, titleString.size());
		if (number > 0)
		{
			filename	+= boost::lexical_cast<string>(number);
		}
		filename	+= ".txt";

		// create country file
		FILE* countryFile;
		fopen_s(&countryFile, filename.c_str(), "wt");
		outputCountryFile(countryFile, *countryItr);
		fclose(countryFile);

		string shortFilename = "countries\\";
		shortFilename += titleString.substr(2, titleString.size());
		if (number > 0)
		{
			shortFilename	+= boost::lexical_cast<string>(number);
		}
		shortFilename	+= ".txt";
		fprintf(countriesList, "%s\t= \"%s\"\n", tag.c_str(), shortFilename.c_str());

		//copy flag
		string srcFlag	=  "\"";
		srcFlag			+= Configuration::getCK2Path();
		srcFlag			+= "\\gfx\\flags\\";
		srcFlag			+= titleString;
		srcFlag			+= ".tga\"";

		string dstFlag	=  "\"";
		dstFlag			+= Configuration::getModPath() + "\\Converter\\gfx\\flags\\";
		dstFlag			+= tag;
		dstFlag			+= ".tga\"";

		string command	=  "copy ";
		command			+= srcFlag;
		command			+= " ";
		command			+= dstFlag;

		system(command.c_str());

		// record in newCountries.txt
		fprintf(resultsFile, "# Country mapping: CK2=%s -> EU3=%s\n", titleString.c_str(), tag.c_str());
		fprintf(resultsFile, "%s=\n", tag.c_str());
		fprintf(resultsFile, "{\n");
		fprintf(resultsFile, "\tprimary_culture=%s\n", newCountry->getPrimaryCulture().c_str());
		fprintf(resultsFile, "\tcapital_culture=%s\n", provinces[newCountry->getCapital()]->getCulture().c_str());
		fprintf(resultsFile, "\tsuccession=%s\n", (*countryItr)->getSrcCountry()->getSuccessionLaw().c_str());
		fprintf(resultsFile, "\tgender=%s\n", (*countryItr)->getSrcCountry()->getGenderLaw().c_str());
		fprintf(resultsFile, "}\n");
		fprintf(resultsFile, "\n");

		log("\t%s will become %s. Filename is %s\n", titleString.c_str(), tag.c_str(), filename.c_str());
	}
	fclose(EU3Localisations);
	fclose(countriesList);
}


void EU3World::outputCountryFile(FILE* countryFile, EU3Country* country)
{
	fprintf(countryFile, "#Country Name: Please see filename.\n");
	fprintf(countryFile, "\n");
	fprintf(countryFile, "graphical_culture = %s\n",country->getGraphicalCulture().c_str());
	fprintf(countryFile, "\n");
	const int* color = country->getSrcCountry()->getColor();
	fprintf(countryFile, "Color = { %d %d %d }\n", color[0], color[1], color[2]);

	// monarch_names
	deque<tuple<string,int>> monarchNames = country->getMonarchNames();
	if (monarchNames.size() > 0)
	{
		fprintf(countryFile, "\nmonarch_names = {\n");
		for (deque<tuple<string,int>>::iterator itr = monarchNames.begin(); itr < monarchNames.end(); ++itr)
		{
			fprintf(countryFile, "\t\"%s #0\" = %d\n", get<0>((*itr)).c_str(), get<1>((*itr)) );
		}
		fprintf(countryFile, "}\n");
	}

	vector<string> leaderNames = country->getLeaderNames();
	if (leaderNames.size() > 0)
	{
		fprintf(countryFile, "\nleader_names = {\n\t");
		for (vector<string>::iterator itr = leaderNames.begin(); itr < leaderNames.end(); ++itr)
		{
			fprintf(countryFile, "%s ", (*itr).c_str() );
			if ( (distance(leaderNames.begin(),itr)+1)%10 == 0 )
			{
				fprintf(countryFile, "\n\t");
			}
		}
		fprintf(countryFile, "\n}\n");
	}

	vector<string> shipNames = country->getShipNames();
	if (shipNames.size() > 0)
	{
		fprintf(countryFile, "\nship_names = {\n\t");
		for (vector<string>::iterator itr = shipNames.begin(); itr < shipNames.end(); ++itr)
		{
			fprintf(countryFile, "%s ", (*itr).c_str() );
			if ( (distance(shipNames.begin(),itr)+1)%10 == 0 )
			{
				fprintf(countryFile, "\n\t");
			}
		}
		fprintf(countryFile, "\n}\n");
	}

	vector<string> armyNames = country->getArmyNames();
	if (armyNames.size() > 0)
	{
		fprintf(countryFile, "\narmy_names = {\n\t");
		for (vector<string>::iterator itr = armyNames.begin(); itr < armyNames.end(); ++itr)
		{
			fprintf(countryFile, "%s ", (*itr).c_str() );
			if ( (distance(armyNames.begin(),itr)+1)%10 == 0 )
			{
				fprintf(countryFile, "\n\t");
			}
		}
		fprintf(countryFile, "\n}\n");
	}

	vector<string> fleetNames = country->getFleetNames();
	if (fleetNames.size() > 0)
	{
		fprintf(countryFile, "\nfleet_names = {\n\t");
		for (vector<string>::iterator itr = fleetNames.begin(); itr < fleetNames.end(); ++itr)
		{
			fprintf(countryFile, "%s ", (*itr).c_str() );
			if ( (distance(fleetNames.begin(),itr)+1)%10 == 0 )
			{
				fprintf(countryFile, "\n\t");
			}
		}
		fprintf(countryFile, "\n}\n");
	}
}


void EU3World::convertCoTs()
{
	for (map<int, EU3Province*>::iterator provItr = provinces.begin(); provItr != provinces.end(); provItr++)
	{
		if (provItr->second->hasCOT())
		{
			centersOfTrade.push_back(provItr->first);
		}
	}

	for (map<string, EU3Country*>	::iterator countryItr = countries.begin(); countryItr != countries.end(); countryItr++)
	{
		if (countryItr->second->getGovernment() == "merchant_republic")
		{
			bool addCot = true;
			vector<EU3Province*> republicProvs = countryItr->second->getProvinces();
			if (republicProvs.size() == 0)
			{
				addCot = false;
			}
			for (vector<EU3Province*>::iterator provItr = republicProvs.begin(); provItr != republicProvs.end(); provItr++)
			{
				if ((*provItr)->hasCOT())
				{
					addCot = false;
				}
			}
			if (addCot)
			{
				int capital = countryItr->second->getCapital();
				map<int, EU3Province*>::iterator provItr = provinces.find(capital);
				if (provItr != provinces.end())
				{
					provItr->second->setCot(true);
					centersOfTrade.push_back(provItr->first);
				}
			}
		}
	}
}


void EU3World::convertSliders()
{
	// get EU3 tags
	log("\t;tag;government;centralization/decentraliztion;aristocracy/plutocracy;serfdom/freesubjects;innovative/narrowminded;mercantilism/freetrade;offensive/defensive;land/naval;quality/quantity\n");
	for (vector<EU3Country*>::iterator countryItr =	convertedCountries.begin(); countryItr != convertedCountries.end(); countryItr++)
	{
		(*countryItr)->convertSliders();
	}
}
