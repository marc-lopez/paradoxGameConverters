#include <map>
#include "CppUnitTest.h"
#include "LogBase.h"
#include "CK2World\CK2Title.h"
#include "CK2World\CK2World.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{		
	TEST_CLASS(CK2WorldTests)
	{
	public:

		TEST_METHOD(TitleFilterShouldRemoveTitlesWithoutCurrentHolderAndHistory)
		{
			int COLOR[3] = {0, 0, 0};
			const string TITLE_NAME = "k_sample";
			const string DE_JURE_LIEGE_TITLE_NAME = "e_sample";

			LogBase loggerDummy;
			CK2World world(loggerDummy);
			map<string, CK2Title*> sampleDeJureLieges;
			CK2Title* sampleDeJureLiege = new CK2Title(DE_JURE_LIEGE_TITLE_NAME, COLOR);
			sampleDeJureLieges.insert(make_pair(DE_JURE_LIEGE_TITLE_NAME, sampleDeJureLiege));

			CK2Title* newTitle = new CK2Title(TITLE_NAME, COLOR);
			newTitle->setDeJureLiege(sampleDeJureLieges);

			world.addTitle(make_pair(TITLE_NAME, newTitle));

			TitleFilter(&world).removeDeadTitles();

			Assert::IsNull(world.getAllTitles()[TITLE_NAME]);
		}

	};
}