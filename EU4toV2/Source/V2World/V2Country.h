/*Copyright (c) 2014 The Paradox Game Converters Project

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



#ifndef V2COUNTRY_H_
#define V2COUNTRY_H_


#include "../CountryMapping.h"
#include "../Mapper.h"
#include "../Color.h"
#include "../Date.h"
#include "../Eu4World/EU4Army.h"
#include "V2Inventions.h"
#include "V2Localisation.h"
#include "V2TechSchools.h"
#include <vector>
#include <set>
using namespace std;

class EU4World;
class EU4Country;
class V2World;
class V2State;
class V2Province;
class V2Relations;
class V2Army;
class V2Reforms;
class V2UncivReforms;
class V2Factory;
class V2Creditor;
class V2Leader;
class V2LeaderTraits;
struct V2Party;



class V2Country
{
	public:
		V2Country(string _tag, string _commonCountryFile, vector<V2Party*> _parties, V2World* _theWorld, bool _newCountry = false, bool _dynamicCountry = false);
		void								output() const;
		void								outputToCommonCountriesFile(FILE*) const;
		void								outputLocalisation(FILE*) const;
		void								outputOOB() const;
		void								initFromEU4Country(EU4Country* _srcCountry, const CountryMapping& countryMap, cultureMapping cultureMap, religionMapping religionMap, unionCulturesMap unionCultures, governmentMapping governmentMap, inverseProvinceMapping inverseProvinceMap, vector<V2TechSchool> techSchools, map<int, int>& leaderMap, const V2LeaderTraits& lt, const map<string, double>& UHLiberalIdeas, const map<string, double>& UHReactionaryIdeas, const vector< pair<string, int> >& literacyIdeas, const EU4RegionsMapping& regionsMap);
		void								initFromHistory();
		void								addProvince(V2Province* _province);
		void								addState(V2State* newState);
		void								convertArmies(const map<int,int>& leaderIDMap, double cost_per_regiment[num_reg_categories], const inverseProvinceMapping& inverseProvinceMap, map<int, V2Province*> allProvinces, vector<int> port_whitelist, adjacencyMapping adjacencyMap);
		bool								addFactory(V2Factory* factory);
		void								addRailroadtoCapitalState();
		void								convertUncivReforms();
		void								setupPops(double popWeightRatio);
		void								setArmyTech(double normalizedScore);
		void								setNavyTech(double normalizedScore);
		void								setCommerceTech(double normalizedScore);
		void								setIndustryTech(double normalizedScore);
		void								setCultureTech(double normalizedScore);
		void								addRelation(V2Relations* newRelation);
		void								absorbVassal(V2Country* vassal);
		void								setColonyOverlord(V2Country* colony);
		V2Country*							getColonyOverlord();
		string								getColonialRegion();

		string								getLocalName();
		V2Relations*						getRelations(string withWhom) const;
		void								getNationalValueScores(int& liberty, int& equality, int& order, const map<string, int>& orderIdeas, const map<string, int>& libertyIdeas, const map<string, int>& equalityIdeas);
		
		void								addPrestige(double additionalPrestige) { prestige += additionalPrestige; }
		void								addResearchPoints(double newPoints)		{ researchPoints += newPoints; }
		void								addTech(string newTech)						{ techs.push_back(newTech); }
		void								setNationalValue(string NV)				{ nationalValue = NV; }
		void								isANewCountry(void)							{ newCountry = true; }

		map<int, V2Province*>		getProvinces() const { return provinces; }
		string							getTag() const { return tag; }
		bool								isCivilized() const { return civilized; }
		string							getPrimaryCulture() const { return primaryCulture; }
		set<string>						getAcceptedCultures() const { return acceptedCultures; }
		EU4Country*						getSourceCountry() const { return srcCountry; }
		inventionStatus				getInventionState(vanillaInventionType invention) const { return vanillaInventions[invention]; }
		inventionStatus				getInventionState(HODInventionType invention) const { return HODInventions[invention]; }
		inventionStatus				getInventionState(HODNNMInventionType invention) const { return HODNNMInventions[invention]; }
		double							getReactionary() const { return upperHouseReactionary; }
		double							getConservative() const { return upperHouseConservative; }
		double							getLiberal() const { return upperHouseLiberal; }
		string							getGovernment() const { return government; }
		vector< pair<int, int> >	getReactionaryIssues() const { return reactionaryIssues; }
		vector< pair<int, int> >	getConservativeIssues() const { return conservativeIssues; }
		vector< pair<int, int> >	getLiberalIssues() const { return liberalIssues; }
		double							getLiteracy() const { return literacy; }
		int								getCapital() const { return capital; }
		bool								isNewCountry() const { return newCountry; }
		int								getNumFactories() const { return numFactories; }

		string								getReligion() const { return religion; }

	private:
		void			outputTech(FILE*) const ;
		void			outputElection(FILE*) const;
		void			addLoan(string creditor, double size, double interest);
		int			addRegimentToArmy(V2Army* army, RegimentCategory rc, const inverseProvinceMapping& inverseProvinceMap, map<int, V2Province*> allProvinces, adjacencyMapping adjacencyMap);
		vector<int>	getPortProvinces(vector<int> locationCandidates, map<int, V2Province*> allProvinces);
		V2Army*		getArmyForRemainder(RegimentCategory rc);
		V2Province*	getProvinceForExpeditionaryArmy();
		string		getRegimentName(RegimentCategory rc);

		V2World*							theWorld;
		EU4Country*						srcCountry;
		string							filename;
		bool								newCountry;			// true if this country is being added by the converter, i.e. doesn't already exist in Vic2
		bool								dynamicCountry;	// true if this country is a Vic2 dynamic country
		V2Country*						colonyOverlord;
		string							colonialRegion;

		string							tag;
		vector<V2State*>				states;
		map<int, V2Province*>		provinces;
		int								capital;
		bool								civilized;
		bool							isReleasableVassal;
		string							primaryCulture;
		set<string>						acceptedCultures;
		string							religion;
		vector<V2Party*>				parties;
		string							rulingParty;
		string							commonCountryFile;
		double							prestige;
		double							leadership;
		double							plurality;
		vector<string>					techs;
		inventionStatus				vanillaInventions[VANILLA_naval_exercises];
		inventionStatus				HODInventions[HOD_naval_exercises];
		inventionStatus				HODNNMInventions[HOD_NNM_naval_exercises];
		V2UncivReforms*				uncivReforms;
		double							researchPoints;
		string							techSchool;
		string							government;
		int								upperHouseReactionary;
		int								upperHouseConservative;
		int								upperHouseLiberal;
		vector< pair<int, int> >	reactionaryIssues;
		vector< pair<int, int> >	conservativeIssues;
		vector< pair<int, int> >	liberalIssues;
		map<string,V2Relations*>	relations;
		vector<V2Army*>				armies;
		V2Reforms*						reforms;
		string							nationalValue;
		double							money;
		date								lastBankrupt;
		map<string, V2Creditor*>	creditors;
		double							bankReserves;
		double							diploPoints;
		double							badboy;
		vector<V2Leader*>				leaders;
		double							literacy;
		V2Localisation					localisation;
		Color								color;
		int								unitNameCount[num_reg_categories];
		int								numFactories;
};

bool ProvinceRegimentCapacityPredicate(V2Province* prov1, V2Province* prov2);


#endif	// V2COUNTRY_H_