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



#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <sys/stat.h>
#include <io.h>
#include <functional>
#include <ShlObj.h>
#include "Log.h"
#include "Configuration.h"
#include "Parsers/Parser.h"
#include "Parsers/Object.h"
#include "Parsers/LandedTitleMigrationsParser.h"
#include "EU3World\EU3World.h"
#include "EU3World\EU3Country.h"
#include "EU3World\EU3Tech.h"
#include "CK2World\CK2World.h"
#include "CK2World\CK2Opinion.h"
#include "CK2World\CK2Religion.h"
#include "Mappers.h"
using namespace std;

void inform(std::string msg)
{
	log((msg + "\n").c_str());
	std::cout << msg <<std::endl;
}

bool doParseDirectoryContents(const std::string& directory, std::function<void(Object* obj)> predicate)
{
	Object*				obj;
	struct _finddata_t	data;
	intptr_t			fileListing;
	if ( (fileListing = _findfirst( (directory + "*").c_str(), &data)) == -1L)
	{
		return false;
	}
	do
	{
		if (strcmp(data.name, ".") == 0 || strcmp(data.name, "..") == 0 )
		{
			continue;
		}
		obj = doParseFile((directory + data.name).c_str());
		if (obj == NULL)
		{
			inform("Error: Could not open " + directory + data.name);
			exit(-1);
		}
		predicate(obj);
	} while(_findnext(fileListing, &data) == 0);
	_findclose(fileListing);

	return true;
}

void parseLandedTitleMigrations(CK2World& world)
{
    LOG(LogLevel::Info) << "\tParsing landed title migrations\n";
    auto fileData = doParseFile("landed_title_migrations.txt");
    parsers::LandedTitleMigrationsParser landedTitleMigrationsParser;
    auto landedTitleMigrations = landedTitleMigrationsParser.parse(std::shared_ptr<Object>(fileData));
    world.addTitleMigrations(landedTitleMigrations);
}

int main(int argc, char * argv[])
{
	Object*	obj;				// generic object

	//Get CK2 install location
	string CK2Loc = Configuration::getCK2Path();
	if (CK2Loc[CK2Loc.length() - 1] == '\\')
	{
		CK2Loc = CK2Loc.substr(0, CK2Loc.length() - 1);
	}
	struct stat st;
	if (CK2Loc.empty() || (stat(CK2Loc.c_str(), &st) != 0))
	{
		inform("No Crusader King 2 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.");
		return (-2);
	}

	//Verify EU3 install location
	string EU3Loc = Configuration::getEU3Path();
	if (EU3Loc[EU3Loc.length() - 1] == '\\')
	{
		EU3Loc = EU3Loc.substr(0, EU3Loc.length() - 1);
	}
	if (EU3Loc.empty() || (stat(EU3Loc.c_str(), &st) != 0))
	{
		inform("No Europa Universalis 3 path was specified in configuration.txt, or the path was invalid.  A valid path must be specified.");
		return (-2);
	}


	//Get Input CK2 save
	string inputFilename("input.ck2");
	if (argc >= 2)
	{
		inputFilename = argv[1];
		inform("Using input file " + inputFilename);
	}
	else
	{
		inform("No input file given, defaulting to input.ck2");
	}


	//Copy mod folder
	string modFolderName;
	if (Configuration::getUseConverterMod() == "yes")
	{
		if (argc >= 2)
		{
			modFolderName = inputFilename.substr(0, inputFilename.find_last_of('.'));
			modFolderName = modFolderName.substr(modFolderName.find_last_of('\\') + 1, modFolderName.length());
		}
		else
		{
			modFolderName = "output";
		}
		Configuration::setModPath(modFolderName);
		string copyCommand = "xcopy mod \"" + modFolderName + "\" /E /C /I /Y";
		system(copyCommand.c_str());
	}
	inform("Getting CK2 data.");

	inform("\tGetting opinion modifiers");

	auto srcOpinionRepository = std::make_shared<CK2OpinionRepository>();

	if (Configuration::getCK2Mod() != "")
	{
		obj = doParseFile((Configuration::getCK2ModPath() + "\\" + Configuration::getCK2Mod() + "/common/opinion_modifiers.txt").c_str()); // for pre-1.06 installs
		if (obj != NULL)
		{
			srcOpinionRepository->initOpinions(obj);
		}
		doParseDirectoryContents((Configuration::getCK2ModPath() + "\\" + Configuration::getCK2Mod() + "\\common\\opinion_modifiers\\"), [&](Object* eachobj) { srcOpinionRepository->initOpinions(eachobj); });
	}
	obj = doParseFile((Configuration::getCK2Path() + "/common/opinion_modifiers.txt").c_str()); // for pre-1.06 installs
	srcOpinionRepository->initOpinions(obj);
	if (!doParseDirectoryContents((CK2Loc + "\\common\\opinion_modifiers\\"), [&](Object* eachobj) { srcOpinionRepository->initOpinions(eachobj); }))
	{
		inform("\t\tError: Could not open opinion_modifiers directory (ok for pre-1.06).");
		if (obj == NULL)
		{
			inform("Error: Could not open " + Configuration::getCK2Path() + "/common/opinion_modifiers.txt");
			exit(-1);
		}
	}

	// Input CK2 Data
	auto srcWorld = std::make_shared<CK2World>(std::make_shared<Log>(LogLevel::Info), srcOpinionRepository);

	inform("\tGetting building types.");
	if (Configuration::getCK2Mod() != "")
	{
		obj = doParseFile((Configuration::getCK2ModPath() + "\\" + Configuration::getCK2Mod() + "/common/buildings.txt").c_str()); // for pre-1.06 installs
		srcWorld->addBuildingTypes(obj);
		doParseDirectoryContents((Configuration::getCK2ModPath() + "\\" + Configuration::getCK2Mod() + "\\common\\buildings\\"), [&](Object* eachobj) { srcWorld->addBuildingTypes(eachobj); });
	}
	obj = doParseFile((Configuration::getCK2Path() + "/common/buildings.txt").c_str()); // for pre-1.06 installs
	srcWorld->addBuildingTypes(obj);
	if (!doParseDirectoryContents((CK2Loc + "\\common\\buildings\\"), [&](Object* eachobj) { srcWorld->addBuildingTypes(eachobj); }))
	{
		inform("\t\tError: Could not open buildings directory (ok for pre-1.06).\n");

		if (obj == NULL)
		{
			inform("Error: Could not open " + Configuration::getCK2Path() + "/common/buildings.txt");
			exit(-1);
		}
	}

	inform("\tGetting CK2 religions");
	if (Configuration::getCK2Mod() != "")
	{
		obj = doParseFile((Configuration::getCK2ModPath() + "\\" + Configuration::getCK2Mod() + "/common/religion.txt").c_str()); // for pre-1.06 installs
		CK2Religion::parseReligions(obj);
		doParseDirectoryContents((Configuration::getCK2ModPath() + "\\" + Configuration::getCK2Mod() + "\\common\\religions\\"), [&](Object* eachobj) { CK2Religion::parseReligions(eachobj); });
	}
	obj = doParseFile((Configuration::getCK2Path() + "/common/religion.txt").c_str()); // for pre-1.06 installs
	CK2Religion::parseReligions(obj);
	if (!doParseDirectoryContents((CK2Loc + "\\common\\religions\\"), [&](Object* eachobj) { CK2Religion::parseReligions(eachobj); }))
	{
		inform("\t\tError: Could not open religions directory (ok for pre-1.06).");

		if (obj == NULL)
		{
			inform("Error: Could not open " + Configuration::getCK2Path() + "/common/religion.txt");
			exit(-1);
		}
	}

	inform("\tGetting CK2 cultures");
	auto CK2CultureGroupMap = std::make_shared<cultureGroupMapping>();
	if (Configuration::getCK2Mod() != "")
	{
		obj = doParseFile((Configuration::getCK2ModPath() + "\\" + Configuration::getCK2Mod() + "/common/cultures.txt").c_str()); // for pre-1.06 installs
		addCultureGroupMappings(obj, *CK2CultureGroupMap);
		doParseDirectoryContents((Configuration::getCK2ModPath() + "\\" + Configuration::getCK2Mod() + "\\common\\cultures\\"), [&](Object* eachobj) { addCultureGroupMappings(eachobj, *CK2CultureGroupMap); });
	}
	obj = doParseFile((Configuration::getCK2Path() + "/common/cultures.txt").c_str()); // for pre-1.06 installs
	addCultureGroupMappings(obj, *CK2CultureGroupMap);
	if (!doParseDirectoryContents((CK2Loc + "\\common\\cultures\\"), [&](Object* eachobj) { addCultureGroupMappings(eachobj, *CK2CultureGroupMap); }))
	{
		inform("\t\tError: Could not open cultures directory (ok for pre-1.06).");
		if (obj == NULL)
		{
			inform("Error: Could not open " + Configuration::getCK2Path() + "/common/cultures.txt");
			exit(-1);
		}
	}

	inform("\tParsing landed titles.");
	bool modHasFiles = false;
	if (Configuration::getCK2Mod() != "")
	{
		obj = doParseFile((Configuration::getCK2ModPath() + "\\" + Configuration::getCK2Mod() + "/common/landed_titles.txt").c_str()); // for pre-1.06 installs
		if (obj != NULL)
		{
			srcWorld->addPotentialTitles(obj);
			modHasFiles = true;
		}
		DWORD dwAttrib = GetFileAttributes((Configuration::getCK2ModPath() + "\\" + Configuration::getCK2Mod() + "\\common\\landed_titles\\").c_str());
		if (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
		{
			doParseDirectoryContents((Configuration::getCK2ModPath() + "\\" + Configuration::getCK2Mod() + "\\common\\landed_titles\\"), [&](Object* eachobj) { srcWorld->addPotentialTitles(eachobj); });
			modHasFiles = true;
		}
	}
	if (!modHasFiles)
	{
		obj = doParseFile((Configuration::getCK2Path() + "/common/landed_titles.txt").c_str()); // for pre-1.06 installs
		srcWorld->addPotentialTitles(obj);
		if (!doParseDirectoryContents((CK2Loc + "\\common\\landed_titles\\"), [&](Object* eachobj) { srcWorld->addPotentialTitles(eachobj); }))
		{
			inform("\t\tError: Could not open landed_titles directory (ok for pre-1.06).");
			if (obj == NULL)
			{
				inform("Error: Could not open " + Configuration::getCK2Path() + "/common/landed_titles.txt");
				exit(-1);
			}
		}
	}

	parseLandedTitleMigrations(*srcWorld);

	inform("\tGetting traits");
	if (Configuration::getCK2Mod() != "")
	{
		obj = doParseFile((Configuration::getCK2ModPath() + "\\" + Configuration::getCK2Mod() + "/common/traits.txt").c_str()); // for pre-1.06 installs
		if (obj != NULL)
		{
			srcWorld->addTraits(obj);
		}
		doParseDirectoryContents((Configuration::getCK2ModPath() + "\\" + Configuration::getCK2Mod() + "\\common\\traits\\"), [&](Object* eachobj) { srcWorld->addTraits(eachobj); });
	}
	obj = doParseFile((Configuration::getCK2Path() + "/common/traits.txt").c_str()); // for pre-1.06 installs
	srcWorld->addTraits(obj);
	if (!doParseDirectoryContents((CK2Loc + "\\common\\traits\\"), [&](Object* eachobj) { srcWorld->addTraits(eachobj); }))
	{
		log("\t\tError: Could not open traits directory (ok for pre-1.06).\n");
		printf("\t\tError: Could not open traits directory (ok for pre-1.06).\n");
		if (obj == NULL)
		{
			log("Error: Could not open %s\n", (Configuration::getCK2Path() + "/common/traits.txt").c_str());
			printf("Error: Could not open %s\n", (Configuration::getCK2Path() + "/common/traits.txt").c_str());
			exit(-1);
		}
	}

	inform("\tAdding dynasties from CK2 Install");
	if (Configuration::getCK2Mod() != "")
	{
		obj = doParseFile((Configuration::getCK2ModPath() + "\\" + Configuration::getCK2Mod() + "/common/dynasties.txt").c_str()); // for pre-1.06 installs
		if (obj != NULL)
		{
			srcWorld->addDynasties(obj);
		}
		doParseDirectoryContents((Configuration::getCK2ModPath() + "\\" + Configuration::getCK2Mod() + "\\common\\dynasties\\"), [&](Object* eachobj) { srcWorld->addDynasties(eachobj); });
	}
	obj = doParseFile((Configuration::getCK2Path() + "/common/dynasties.txt").c_str()); // for pre-1.06 installs
	srcWorld->addDynasties(obj);
	if (!doParseDirectoryContents((CK2Loc + "\\common\\dynasties\\"), [&](Object* eachobj) { srcWorld->addDynasties(eachobj); }))
	{
		log("\t\tError: Could not open dynasties directory (ok for pre-1.06).\n");
		printf("\t\tError: Could not open dynasties directory (ok for pre-1.06).\n");
		if (obj == NULL)
		{
			inform("Error: Could not open " + Configuration::getCK2Path() + "/common/dynasties.txt");
			exit(-1);
		}
	}

	log("Parsing CK2 save.\n");
	printf("Parsing CK2 save.\n");
	obj = doParseFile(inputFilename.c_str());
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", inputFilename.c_str());
		printf("Error: Could not open %s\n", inputFilename.c_str());
		exit(-1);
	}

	log("Importing parsed data.\n");
	printf("Importing parsed data.\n");
	srcWorld->init(obj, CK2CultureGroupMap);

	log("Merging top-level titles.\n");
	printf("Merging top-level titles.\n");
	srcWorld->mergeTitles();


	// Parse province mappings
	log("Parsing province mappings.\n");
	printf("Parsing province mappings.\n");
	const char* mappingFile = "province_mappings.txt";
	obj = doParseFile(mappingFile);
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", mappingFile);
		printf("Error: Could not open %s\n", mappingFile);
		exit(-1);
	}
	provinceMapping			provinceMap				= initProvinceMap(obj, srcWorld->getVersion().get());
	inverseProvinceMapping	inverseProvinceMap	= invertProvinceMap(provinceMap);
	//map<int, CK2Province*> srcProvinces				= srcWorld->getProvinces();
	//for (map<int, CK2Province*>::iterator i = srcProvinces.begin(); i != srcProvinces.end(); i++)
	//{
	//	inverseProvinceMapping::iterator p = inverseProvinceMap.find(i->first);
	//	if ( p == inverseProvinceMap.end() )
	//	{
	//		log("\tError: CK2 province %d has no mapping specified!\n", i->first);
	//	}
	//	else if ( p->second.size() == 0 )
	//	{
	//		log("\tWarning: CK2 province %d is not mapped to any EU3 provinces!\n", i->first);
	//	}
	//}

	// Parse techs
	log("Setting up tech groups.\n");
	printf("Setting up tech groups.\n");
	obj					= doParseFile( (Configuration::getEU3Path() + "/common/technology.txt").c_str() );
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technology.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technology.txt").c_str());
		exit(-1);
	}
	Object* govObj		= doParseFile( (Configuration::getEU3Path() + "/common/technologies/government.txt").c_str() );
	if (govObj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/government.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/government.txt").c_str());
		exit(-1);
	}
	Object* prodObj	= doParseFile( (Configuration::getEU3Path() + "/common/technologies/production.txt").c_str() );
	if (prodObj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/production.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/production.txt").c_str());
		exit(-1);
	}
	Object* tradeObj	= doParseFile( (Configuration::getEU3Path() + "/common/technologies/trade.txt").c_str() );
	if (tradeObj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/trade.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/trade.txt").c_str());
		exit(-1);
	}
	Object* navalObj	= doParseFile( (Configuration::getEU3Path() + "/common/technologies/naval.txt").c_str() );
	if (navalObj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/naval.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/naval.txt").c_str());
		exit(-1);
	}
	Object* landObj	= doParseFile( (Configuration::getEU3Path() + "/common/technologies/land.txt").c_str() );
	if (landObj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/land.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "/common/technologies/land.txt").c_str());
		exit(-1);
	}
	EU3Tech* techData = new EU3Tech(srcWorld->getEndDate(), obj, govObj, prodObj, tradeObj, navalObj, landObj);

	EU3World destWorld(srcWorld.get(), techData);

	// Add historical EU3 countries
	log("Adding historical EU3 nations.\n");
	printf("Adding historical EU3 nations.\n");
	destWorld.addHistoricalCountries();

	// Get list of blocked nations
	log("Getting blocked EU3 nations.\n");
	printf("Getting blocked EU3 nations.\n");
	obj = doParseFile("blocked_nations.txt");
	if (obj == NULL)
	{
		log("Error: Could not open blocked_nations.txt\n");
		printf("Error: Could not open blocked_nations.txt\n");
		exit(-1);
	}
	vector<string> blockedNations = processBlockedNations(obj);

	// Get EU3 Culture Groups
	log("Getting EU3 cultures\n");
	printf("Getting EU3 cultures\n");
	string cultureFile;
	if (Configuration::getUseConverterMod() == "yes")
	{
		cultureFile = Configuration::getModPath() + "\\converter\\common\\cultures.txt";
	}
	else
	{
		cultureFile = Configuration::getEU3Path() + "\\common\\cultures.txt";
	}
	obj = doParseFile(cultureFile.c_str());
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", cultureFile.c_str());
		printf("Error: Could not open %s\n", cultureFile.c_str());
		exit(-1);
	}
	cultureGroupMapping EU3CultureGroupMap;
	addCultureGroupMappings(obj, EU3CultureGroupMap);

	// Get EU3 Religion Groups
	log("Getting EU3 religions\n");
	printf("Getting EU3 religions\n");
	string religionFile;
	if (Configuration::getUseConverterMod() == "yes")
	{
		religionFile = Configuration::getModPath() + "\\converter\\common\\religion.txt";
	}
	else
	{
		religionFile = Configuration::getEU3Path() + "\\common\\religion.txt";
	}
	obj = doParseFile(religionFile.c_str());
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", religionFile.c_str());
		printf("Error: Could not open %s\n", religionFile.c_str());
		exit(-1);
	}
	religionGroupMapping EU3ReligionGroupMap;
	addReligionGroupMappings(obj, EU3ReligionGroupMap);

	// Get culture mappings
	log("Parsing culture mappings.\n");
	printf("Parsing culture mappings.\n");
	string filename;
	if (Configuration::getUseConverterMod() == "yes")
	{
		filename = "culture_mappings_mod.txt";
	}
	else
	{
		filename = "culture_mappings.txt";
	}
	obj = doParseFile(filename.c_str());
	if (obj == NULL)
	{
		log("Error: Could not open culture_mappings.txt\n");
		printf("Error: Could not open culture_mappings.txt\n");
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		log("Error: Failed to parse culture_mappings.txt.\n");
		printf("Error: Failed to parse culture_mappings.txt.\n");
		return 1;
	}
	cultureMapping cultureMap;
	cultureMap = initCultureMap(static_cast<Object*>(obj->getLeaves()[0]));

	// Get religion mappings
	log("Parsing religion mappings.\n");
	printf("Parsing religion mappings.\n");
	if (Configuration::getUseConverterMod() == "yes")
	{
		filename = "religion_mappings_mod.txt";
	}
	else
	{
		filename = "religion_mappings.txt";
	}
	obj = doParseFile(filename.c_str());
	if (obj == NULL)
	{
		log("Error: Could not open religion_mappings.txt\n");
		printf("Error: Could not open religion_mappings.txt\n");
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		log("Error: Failed to parse religion_mappings.txt.\n");
		printf("Error: Failed to parse religion_mappings.txt.\n");
		return 1;
	}
	religionMapping religionMap;
	religionMap = initReligionMap(static_cast<Object*>(obj->getLeaves()[0]));

	// Get continents
	log("Parsing continents.\n");
	printf("Parsing continents.\n");
	obj = doParseFile((Configuration::getEU3Path() + "\\map\\continent.txt").c_str());
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "\\map\\continent.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "\\map\\continent.txt").c_str());
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		log("Error: Failed to parse continent.txt.\n");
		printf("Error: Failed to parse continent.txt.\n");
		return 1;
	}
	continentMapping continentMap;
	continentMap = initContinentMap(obj);

	// Get adjacencies
	log("Importing adjacencies\n");
	printf("Importing adjacencies\n");
	adjacencyMapping adjacencyMap = initAdjacencyMap();

	// Get trade good data
	log("Importing trade good data.\n");
	printf("Importing trade good data.\n");
	obj = doParseFile((Configuration::getEU3Path() + "\\common\\Prices.txt").c_str());
	if (obj == NULL)
	{
		log("Error: Could not open %s\n", (Configuration::getEU3Path() + "\\common\\Prices.txt").c_str());
		printf("Error: Could not open %s\n", (Configuration::getEU3Path() + "\\common\\Prices.txt").c_str());
		exit(-1);
	}
	if (obj->getLeaves().size() < 1)
	{
		log("Error: Failed to parse Prices.txt.\n");
		printf("Error: Failed to parse Prices.txt.\n");
		return 1;
	}
	tradeGoodMapping tradeGoodMap = initTradeGoodMapping(obj);


	// Convert
	log("Converting countries.\n");
	printf("Converting countries.\n");
	destWorld.convertCountries(srcWorld->getAllTitles(), religionMap, cultureMap, inverseProvinceMap);

	log("Setting up provinces.\n");
	printf("Setting up provinces.\n");
	destWorld.setupProvinces(provinceMap);

	log("Converting provinces.\n");
	printf("Converting provinces.\n");
	Object* positionsObj = doParseFile((Configuration::getEU3Path() + "\\map\\positions.txt").c_str());
	auto provinces = srcWorld->getProvinces();
	destWorld.convertProvinces(provinceMap, provinces, cultureMap, religionMap, continentMap, adjacencyMap, tradeGoodMap, EU3ReligionGroupMap, positionsObj);

	// Map CK2 nations to EU3 nations
	LOG(LogLevel::Info) << "Parsing country mappings.\n";
	if (Configuration::getUseConverterMod() == "yes")
	{
		filename = "country_mappings_mod.txt";
	}
	else
	{
		filename = "country_mappings.txt";
	}
	obj = doParseFile(filename.c_str());
	if (obj == NULL)
	{
	    LOG(LogLevel::Error) << "Could not open country_mappings.txt\n";
		exit(-1);
	}
	LOG(LogLevel::Info) << "Mapping CK2 nations to EU3 nations.\n";
	destWorld.assignTags(obj, blockedNations, provinceMap, religionMap, cultureMap, inverseProvinceMap, *(srcWorld->getVersion()));

    LOG(LogLevel::Info) << "Adding accepted cultures.\n";
	destWorld.addAcceptedCultures();

    LOG(LogLevel::Info) << "Converting tech.\n";
	destWorld.convertTech(*srcWorld);

	LOG(LogLevel::Info) << "Converting governments.\n";
	destWorld.convertGovernments();

	LOG(LogLevel::Info) << "Converting centers of trade\n";
	destWorld.convertCoTs();

	log("Converting sliders\n");
	printf("Converting sliders\n");
	destWorld.convertSliders();

	log("Converting economies.\n");
	printf("Converting economies.\n");
	destWorld.convertEconomies(EU3CultureGroupMap, tradeGoodMap);

	log("Converting advisors.\n");
	printf("Converting advisors.\n");
	destWorld.convertAdvisors(inverseProvinceMap, provinceMap, *srcWorld);

	log("Converting diplomatic relations.\n");
	printf("Converting diplomatic relations.\n");
	destWorld.convertDiplomacy(*(srcWorld->getVersion()));

	log("Converting armies and navies.\n");
	printf("Converting armies and navies.\n");
	destWorld.convertArmies(inverseProvinceMap);

	// Output results
	printf("Outputting save.\n");
	log("Outputting save.\n");
	string outputFilename = "";
	if (Configuration::getUseConverterMod() == "yes")
	{
		outputFilename = modFolderName + "\\Converter\\save games\\";
		if (argc >= 2)
		{
			string filename = inputFilename.substr(0, inputFilename.find_last_of('.'));
			filename = filename.substr(filename.find_last_of('\\') + 1, filename.length());
			outputFilename += filename + ".eu3";
		}
		else
		{
			outputFilename += "output.eu3";
		}
	}
	else
	{
		if (argc >= 2)
		{
			string filename = inputFilename.substr(0, inputFilename.find_last_of('.'));
			filename = filename.substr(filename.find_last_of('\\') + 1, filename.length());
			outputFilename += filename + ".eu3";
		}
		else
		{
			outputFilename += "output.eu3";
		}
	}
	log("\tFilename: %s\n", outputFilename.c_str());
	FILE* output;
	if (fopen_s(&output, outputFilename.c_str(), "w") != 0)
	{
		log("Error: could not open %s.\n", outputFilename.c_str());
		printf("Error: could not open %s.\n", outputFilename.c_str());
	}
	destWorld.output(output);
	fclose(output);


	log("Complete.\n");
	printf("Complete.\n");
	return 0;
}
