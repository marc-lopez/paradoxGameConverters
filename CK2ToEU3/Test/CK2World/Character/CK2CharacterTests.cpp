/*Copyright (c) 2015 The Paradox Game Converters Project

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

#include <map>
#include <memory>
#include "Mocks/ObjectMock.h"
#include "Configuration.h"
#include "Parsers/Object.h"
#include "CK2World/CK2Religion.h"
#include "CK2World/CK2Title.h"
#include "CK2World/Character/CK2Character.h"

using namespace testing;

namespace ck2
{
namespace unittests
{
namespace character
{

using namespace mocks;

class CK2CharacterShould : public Test
{
protected:
	CK2CharacterShould() : SAMPLE_TITLE_NAME("e_sample"), DEMESNE_KEY("demesne")
	{
	}

	std::vector<IObject*> getSampleDemsneData()
	{
	    constexpr auto TITLE_INNER_KEY = "title";
	    constexpr auto PRIMARY_TITLE_KEY = "primary";

		Object *primaryTitleInnerObj = new Object(TITLE_INNER_KEY);
		primaryTitleInnerObj->setValue(SAMPLE_TITLE_NAME);
		Object *primaryTitleObj = new Object(PRIMARY_TITLE_KEY);
		primaryTitleObj->setValue(primaryTitleInnerObj);
		Object *demesneObj = new Object(DEMESNE_KEY);
		demesneObj->setValue(primaryTitleObj);
		std::vector<IObject*> demesneCollection;
		demesneCollection.push_back(demesneObj);

		return demesneCollection;
	}

	void setDefaultExpectations()
	{
        EXPECT_CALL(saveDataMock, getKey()).WillRepeatedly(Return(std::string()));
        EXPECT_CALL(saveDataMock, getLeaf(_)).WillRepeatedly(Return(std::string()));
        EXPECT_CALL(saveDataMock, getLeaf("birth_date")).WillRepeatedly(Return(std::string("1.1.1")));
        EXPECT_CALL(saveDataMock, getValue(_)).WillRepeatedly(Return(std::vector<IObject*>()));
	}

	const std::string SAMPLE_TITLE_NAME;
	const std::string DEMESNE_KEY;
    map<int, CK2Dynasty*> dynasties;
	map<int, CK2Trait*> traits;
	ObjectMock saveDataMock;
};

TEST_F(CK2CharacterShould, SetVersion2Point2SaveFormatPrimaryTitle)
{
	int color[3];
	CK2Title sampleTitle(SAMPLE_TITLE_NAME, color);

	map<string, CK2Title*> titleMap;
	titleMap.insert(std::pair<string, CK2Title*>(SAMPLE_TITLE_NAME, &sampleTitle));

	std::vector<IObject*> demesneData = getSampleDemsneData();

	ObjectMock configurationMock;

    setDefaultExpectations();
	EXPECT_CALL(configurationMock, getLeaf(_)).WillRepeatedly(Return(std::string()));
	EXPECT_CALL(saveDataMock, getValue(DEMESNE_KEY)).WillRepeatedly(Return(demesneData));

	Configuration::setConfiguration(&configurationMock);
	CK2Character sampleCharacter(&saveDataMock, dynasties, traits, common::date());
	sampleTitle.setHolder(&sampleCharacter);
	sampleCharacter.setPrimaryTitle(titleMap);
	CK2Title* calculatedPrimaryTitle = sampleCharacter.getPrimaryTitle();

	ASSERT_THAT(calculatedPrimaryTitle, NotNull());
	ASSERT_EQ(SAMPLE_TITLE_NAME, calculatedPrimaryTitle->getTitleString());

	delete demesneData[0];
}

TEST_F(CK2CharacterShould, BeSunniIfBektashiHeresyNotAvailableInGame)
{
    constexpr auto BEKTASHI_KEY = "bektashi";
    constexpr auto SUNNI_KEY = "sunni";
    constexpr auto MUSLIM_KEY = "muslim";
    constexpr auto RELIGION_KEY = "religion";

    IObject* sunniObj = new Object(SUNNI_KEY);
    std::vector<IObject*> muslimReligions = { sunniObj };
    Object muslimGroupObj(MUSLIM_KEY);
    muslimGroupObj.setValue(static_cast<Object *>(sunniObj));
    std::vector<IObject*> religionGroups = { &muslimGroupObj };
    ObjectMock religionsMock;

    setDefaultExpectations();
    EXPECT_CALL(religionsMock, getLeaves()).WillRepeatedly(Return(religionGroups));
	EXPECT_CALL(saveDataMock, getLeaf(RELIGION_KEY)).WillRepeatedly(Return(BEKTASHI_KEY));

    CK2Religion::parseReligions(&religionsMock);
    CK2Character sampleCharacter(&saveDataMock, dynasties, traits, common::date());

    ASSERT_EQ(CK2Religion::getReligion(SUNNI_KEY), sampleCharacter.getReligion());
}

TEST_F(CK2CharacterShould, GetItsMappedPrimaryTitleStringIfDemesnePrimaryTitleStringEmpty)
{
    int color[3];
	CK2Title sampleTitle(SAMPLE_TITLE_NAME, color);

	map<string, CK2Title*> titleMap;
	titleMap.insert(std::pair<string, CK2Title*>(SAMPLE_TITLE_NAME, &sampleTitle));

    std::vector<IObject*> demesneData;
    auto demesneMock = std::make_shared<ObjectMock>();
    EXPECT_CALL(*demesneMock, getValue(_)).WillRepeatedly(Return(std::vector<IObject*>()));
    EXPECT_CALL(*demesneMock, getTitle(_)).WillRepeatedly(Return(std::string()));
    demesneData.push_back(&*demesneMock);

    setDefaultExpectations();
	EXPECT_CALL(saveDataMock, getValue(DEMESNE_KEY)).WillRepeatedly(Return(demesneData));

    CK2Character sampleCharacter(&saveDataMock, dynasties, traits, common::date());
    sampleTitle.setHolder(&sampleCharacter);
    sampleCharacter.setPrimaryTitle(titleMap);

    ASSERT_EQ(SAMPLE_TITLE_NAME, sampleCharacter.getPrimaryTitleString());
}

} // namespace character
} // namespace unittests
} // namespace ck2
