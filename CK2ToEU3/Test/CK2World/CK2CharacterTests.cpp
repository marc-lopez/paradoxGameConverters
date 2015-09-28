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
#include "CppUnitTest.h"
#include "Configuration.h"
#include "Parsers\Object.h"
#include "Mocks\ObjectMock.h"
#include "CK2World\Character\CK2Character.h"
#include "CK2World\CK2Title.h"

using namespace testing;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{		
	TEST_CLASS(CK2CharacterTests)
	{
	public:

		CK2CharacterTests()
		{
			SAMPLE_TITLE_NAME = "e_sample";
		}

		TEST_METHOD_INITIALIZE(Initialize)
		{
		}

		TEST_METHOD_CLEANUP(Cleanup)
		{
		}

		TEST_METHOD(CK2Character_ShouldSetVersion2Point2SaveFormatPrimaryTitle)
		{
			map<int, CK2Dynasty*> dynasties;
			map<int, CK2Trait*> traits;

			int color[3];
			CK2Title* sampleTitle = new CK2Title(SAMPLE_TITLE_NAME, color);

			map<string, CK2Title*> titleMap;
			titleMap.insert(std::pair<string, CK2Title*>(SAMPLE_TITLE_NAME, sampleTitle));

			std::vector<IObject*> demesneData = getSampleDemsneData();

			ObjectMock *configurationMock = new ObjectMock();
			ObjectMock *saveDataMock = new ObjectMock();
			
			ON_CALL(*configurationMock, getLeaf(_)).WillByDefault(Return(std::string()));
			ON_CALL(*saveDataMock, getLeaf(_)).WillByDefault(Return(std::string()));
			ON_CALL(*saveDataMock, getValue(_)).WillByDefault(Return(std::vector<IObject*>()));
			EXPECT_CALL(*saveDataMock, getValue("demesne")).WillOnce(Return(demesneData));
			
			Configuration::setConfiguration(configurationMock);
			CK2Character* emperor = new CK2Character(saveDataMock, dynasties, traits, date());
			sampleTitle->setHolder(emperor);
			emperor->setPrimaryTitle(titleMap);
			CK2Title* calculatedPrimaryTitle = emperor->getPrimaryTitle(); 
			Assert::IsNotNull(calculatedPrimaryTitle);
			Assert::AreEqual(calculatedPrimaryTitle->getTitleString(), std::string(SAMPLE_TITLE_NAME));

			delete sampleTitle;
			delete emperor;
			delete demesneData[0];
			delete configurationMock;
			delete saveDataMock;
		}

		std::vector<IObject*> getSampleDemsneData()
		{
			Object *primaryTitleInnerObj = new Object("title");
			primaryTitleInnerObj->setValue("e_abyssinia");
			Object *primaryTitleObj = new Object("primary");
			primaryTitleObj->setValue(primaryTitleInnerObj);
			Object *demesneObj = new Object("demesne");
			demesneObj->setValue(primaryTitleObj);
			std::vector<IObject*> demesneCollection;
			demesneCollection.push_back(demesneObj);

			return demesneCollection;
		}

	protected:
		
		std::string SAMPLE_TITLE_NAME;
	};
}