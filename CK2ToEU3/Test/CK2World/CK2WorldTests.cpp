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
#include "LogBase.h"
#include "Parsers\Object.h"
#include "CK2World\CK2Character.h"
#include "CK2World\CK2Title.h"
#include "CK2World\CK2World.h"
#include "Mocks\LoggerMock.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{		
	TEST_CLASS(CK2WorldTests)
	{
	public:

		CK2WorldTests() : TITLE_NAME("k_sample"), DE_JURE_LIEGE_TITLE_NAME("e_sample")
		{
			world = boost::make_shared<CK2World>(boost::make_shared<LoggerMock>());
		}

		TEST_METHOD_INITIALIZE(Initialize)
		{
			sampleDeJureLiege = new CK2Title(DE_JURE_LIEGE_TITLE_NAME, COLOR);
			sampleDeJureLieges.insert(make_pair(DE_JURE_LIEGE_TITLE_NAME, sampleDeJureLiege));
			newTitle = new CK2Title(TITLE_NAME, COLOR);
			newTitle->setDeJureLiege(sampleDeJureLieges);

			world->addTitle(std::make_pair(TITLE_NAME, newTitle));
		}

		TEST_METHOD_CLEANUP(Cleanup)
		{
			delete newTitle;
			delete sampleDeJureLiege;
		}

		TEST_METHOD(TitleFilterShouldRemoveTitlesWithoutCurrentHolderAndHistory)
		{
			TitleFilter(&(*world)).removeDeadTitles();

			Assert::IsNull(world->getAllTitles()[TITLE_NAME]);
		}

		TEST_METHOD(TitleFilterShouldNotFailWhilePassingUsedTitleToWorld)
		{
			CK2Character* holder = new CK2Character();
			newTitle->setHolder(holder);

			TitleFilter(&(*world)).removeDeadTitles();

			Assert::IsNotNull(world->getAllTitles()[TITLE_NAME]);
			delete holder;
		}

	protected:
		int COLOR[3];
		string TITLE_NAME;
		string DE_JURE_LIEGE_TITLE_NAME;
		title_map_t sampleDeJureLieges;
		CK2Title* sampleDeJureLiege;
		CK2Title* newTitle;
		boost::shared_ptr<CK2World> world;
	};
}