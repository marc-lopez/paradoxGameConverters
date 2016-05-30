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



#ifndef EU3COUNTRY_H_
#define EU3COUNTRY_H_



#include <string>
#include <vector>
#include <queue>
#include <tuple>
#include "Common\Date.h"
#include "..\Mappers.h"
#include "..\CK2World\CK2Title.h"
using namespace std;


class CK2Title;
class EU3Ruler;
class EU3Advisor;
class EU3History;
class EU3Province;
class EU3World;
class EU3Tech;
class EU3Diplomacy;
struct EU3Agreement;
class CK2Province;
class CK2Version;
class EU3Army;
class EU3Navy;

class EU3Country
{
	public:
		EU3Country(EU3World* world, string tag, string countryFile, common::date startDate, const EU3Tech* techData);
		EU3Country(CK2Title*, const religionMapping& religionMap, const cultureMapping& cultureMap, const inverseProvinceMapping& inverseProvinceMap);

		void						output(FILE*);
		void						addProvince(EU3Province* province);
		void						determineLearningScore();
		void						determineTechScore();
		void						addAcceptedCultures();
		void						determineGovernment(double prestigeFactor);
		void						determineEconomy(const cultureGroupMapping& cultureGroups, const map<string, double>& unitPrices);
		double					getTradeEffeciency();
		double					getProductionEffeciency();
		void						setPreferredUnitType();
		void						determineTechLevels(const vector<double>& avgTechLevels, const EU3Tech* techData, CK2Version& version);
		void						determineTechInvestment(const EU3Tech* techData, common::date startDate);
		void						determineStartingAgents();
		vector<EU3Country*>	convertVassals(int initialScore, EU3Diplomacy* diplomacy, CK2Version& version);
		vector<EU3Country*>	eatVassals();
		void						eatVassal(EU3Country*);
		void						replaceWith(EU3Country* convertedCountry, const provinceMapping& provinceMappings);
		void						convertArmiesandNavies(inverseProvinceMapping inverseProvinceMap, map<int, EU3Province*> provinces);
		void						convertSliders();

		void		addLiege(EU3Country* _liege)			{ liege = _liege; if (liege != NULL) _liege->addVassal(this); };
		void		addVassal(EU3Country* _vassal)		{ vassals.push_back(_vassal); };
		void		addCore(EU3Province* core)				{ cores.push_back(core); };
		void		setAbsorbScore(int _score)				{ absorbScore = _score; };
		void		setTechGroup(string _techGroup)		{ techGroup = _techGroup; };
		void		setTag(string _tag)						{ tag = _tag; };
		void		addAdvisor(EU3Advisor* _advisor)		{ advisors.push_back(_advisor); };
		void		addAgreement(EU3Agreement* _agr)		{ agreements.push_back(_agr); };
		void		setRelations(EU3Country* other, int value)	{ relations.insert(make_pair(other, value)); };
		void		setElector(bool _elector)				{ elector = _elector; };
		void		setGraphicalCulture(string gfx)			{ graphicalCulture = gfx; };
		void		setMonarchNames(deque<tuple<string,int> > list)	{ monarchNames = list; };
		void		setLeaderNames(vector<string> list)		{ leaderNames = list; };
		void		setShipNames(vector<string> list)		{ shipNames = list; };
		void		setArmyNames(vector<string> list)		{ armyNames = list; };
		void		setFleetNames(vector<string> list)		{ fleetNames = list; };

		CK2Title*				getSrcCountry()			const { return src; };
		EU3Country*				getLiege()					const { return liege; };
		vector<EU3Country*>	getVassals()				const { return vassals; };
		vector<EU3Province*>	getProvinces()				const { return provinces; };
		vector<EU3Province*>	getCores()					const { return cores; };
		vector<EU3Advisor*>	getAdvisors()				const { return advisors; };
		double					getLearningScore()		const { return learningScore; };
		double					getTechScore()				const { return techScore; };
		int						getAbsorbScore()			const	{ return absorbScore; };
		int                     getCrownAuthorityVassalScore() const;
		string					getTag()						const { return tag; };
		string					getGovernment()			const { return government; };
		string					getPrimaryCulture()		const { return primaryCulture; };
		vector<string>			getAcceptedCultures()	const { return acceptedCultures; };
		string					getReligion()				const { return religion; };
		string					getTechGroup()				const { return techGroup; };
		double					getLandTech()				const { return landTech; };
		int						getCapital()				const { return capital; };
		double					getPrestige()				const { return prestige; };
		double					getIncome()					const { return estimatedIncome; };
		double					getGoldIncome()			const { return estimatedGold; };
		int						getStability()				const { return stability; };
		bool						hasProvinces()				const { return !provinces.empty(); };
		bool						hasCores()					const { return !cores.empty(); };
		bool						hasVassals()				const { return !vassals.empty(); };
		int						getInfantry() const;
		int						getNumPorts() const;
		string					getGraphicalCulture()	const { return graphicalCulture; };
		deque<tuple<string,int>>	getMonarchNames()	const { return monarchNames; }
		vector<string>			getLeaderNames()		const { return leaderNames; }
		vector<string>			getShipNames()			const { return shipNames; };
		vector<string>			getArmyNames()			const { return armyNames; };
		vector<string>			getFleetNames()			const { return fleetNames; };
	private:
		void	addBuildings();

		CK2Title*				src;
		EU3Country*				liege;
		vector<EU3Country*>	vassals;
		vector<EU3Province*>	provinces;
		vector<EU3Province*> cores;
		vector<EU3Advisor*>	advisors;
		double					learningScore;
		double					techScore;
		int						absorbScore;

		string						tag;
		string						historyFile;
		string						government;
		string						primaryCulture;
		vector<string>				acceptedCultures;
		string						religion;
		EU3Ruler*					monarch;
		EU3Ruler*					heir;
		EU3Ruler*					regent;
		vector<EU3History*>		history;
		vector<EU3Ruler*>			previousMonarchs;
		string						techGroup;
		double						governmentTech;
		double						productionTech;
		double						tradeTech;
		double						navalTech;
		double						landTech;
		double						governmentTechInvestment;
		double						productionTechInvestment;
		double						tradeTechInvestment;
		double						navalTechInvestment;
		double						landTechInvestment;
		vector<EU3Agreement*>	agreements;
		map<EU3Country*, int>	relations;

		vector<string>			flags;
		int						capital;
		int						stability;
		double					stabilityInvestment;
		double					prestige;

		double					estimatedIncome;
		double					estimatedTax;
		double					estimatedManu;
		double					estimatedGold;
		double					estimatedProduction;
		double					estimatedTolls;

		bool						daimyo;
		bool						japaneseEmperor;
		bool						elector;
		vector<string>			factions;
		string					mainFaction;
		int						mainFactionScore;

		vector<EU3Army*>		armies;
		vector<EU3Navy*>		navies;
		double					manpower;
		string					infantry;
		string					cavalry;
		string					bigShip;
		string					galley;
		string					transport;

		double					merchants;
		double					colonists;
		double					diplomats;
		double					missionaries;
		double					spies;
		double					magistrates;

		int						centralization;
		int						aristocracy;
		int						serfdom;
		int						innovative;
		int						mercantilism;
		int						offensive;
		int						land;
		int						quality;

		string					graphicalCulture;
		deque<tuple<string,int>>	monarchNames;
		vector<string>			leaderNames;
		vector<string>			shipNames;
		vector<string>			armyNames;
		vector<string>			fleetNames;
};



#endif	// EU3COUNTRY_H_
