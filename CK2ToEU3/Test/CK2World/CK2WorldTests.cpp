#include <map>
#include "CppUnitTest.h"
#include "LogBase.h"
#include "Parsers\Object.h"
#include "CK2World\CK2Character.h"
#include "CK2World\CK2Title.h"
#include "CK2World\CK2World.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{		
	TEST_CLASS(CK2WorldTests)
	{
	public:

		CK2WorldTests() : world(LogBase()), TITLE_NAME("k_sample"), DE_JURE_LIEGE_TITLE_NAME("e_sample")
		{
		}

		TEST_METHOD_INITIALIZE(Initialize)
		{
			sampleDeJureLiege = new CK2Title(DE_JURE_LIEGE_TITLE_NAME, COLOR);
			sampleDeJureLieges.insert(make_pair(DE_JURE_LIEGE_TITLE_NAME, sampleDeJureLiege));
			newTitle = new CK2Title(TITLE_NAME, COLOR);
			newTitle->setDeJureLiege(sampleDeJureLieges);

			world.addTitle(make_pair(TITLE_NAME, newTitle));
		}

		TEST_METHOD_CLEANUP(Cleanup)
		{
			delete newTitle;
			delete sampleDeJureLiege;
		}

		TEST_METHOD(TitleFilterShouldRemoveTitlesWithoutCurrentHolderAndHistory)
		{
			TitleFilter(&world).removeDeadTitles();

			Assert::IsNull(world.getAllTitles()[TITLE_NAME]);
		}

		TEST_METHOD(TitleFilterShouldNotFailWhilePassingUsedTitleToWorld)
		{
			CK2Character* holder = new CK2Character();
			newTitle->setHolder(holder);

			TitleFilter(&world).removeDeadTitles();

			Assert::IsNotNull(world.getAllTitles()[TITLE_NAME]);
			delete holder;
		}

	protected:
		int COLOR[3];
		string TITLE_NAME;
		string DE_JURE_LIEGE_TITLE_NAME;
		title_map_t sampleDeJureLieges;
		CK2Title* sampleDeJureLiege;
		CK2Title* newTitle;
		CK2World world;
	};
}