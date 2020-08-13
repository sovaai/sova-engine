#include "NumberParser.hpp"
#include "NanoLib/ReturnCode.hpp"
#include "NanoLib/aTextString.hpp"
#include <NanoLib/NameIndex.hpp>
#include <NanoLib/TextFuncs.hpp>

namespace NumberParser
{

	struct StringToString
	{
		const char * vFrom;
		const char * vTo;
	};


	static const char * STR_NORM_UNIT[] = { "ноль", "один", "два", "три", "четыре", "пять", "шесть", "семь", "восемь", "девять", "десять",
											"одиннадцать", "двенадцать", "тринадцать", "четырнадцать", "пятнадцать", "шестнадцать", "семнадцать", "восемнадцать", "девятнадцать" };

	static const char * STR_NORM_DECADE[] = { "двадцать", "тридцать", "сорок", "пятьдесят", "шестьдесят", "семьдесят", "восемьдесят", "девяносто" };

	static const char * STR_NORM_HUNDRED[] = { "сто", "двести", "триста", "четыреста", "пятьсот", "шестьсот", "семьсот", "восемьсот", "девятьсот" };

	static const char * STR_NORM_BASE[] = { "тысяча", "миллион", "миллиард", "триллион", "квадриллион", "квинтиллион", "секстиллион", "септиллион" };


	typedef const char * SYNONYMS[];

	static const SYNONYMS SYNONYMS_SIGN_MINUS = { "-",
												  "минус", "минуса", "минус", "минусу", "минусом", "минусе", nullptr };

	static const SYNONYMS SYNONYMS_SIGN_PLUS = { "+",
												 "плюс", "плюса", "плюс", "плюсу", "плюсом", "плюсе", nullptr };

	static const SYNONYMS SYNONYMS_INTEGRAL = { "целых",
												"целая", "целые"
														 "целой", "целых",
												"целую", "целые",
												"целой", "целым",
												"целой", "целыми",
												"целой", "целых", nullptr };

	static const SYNONYMS SYNONYMS_0 = { "ноль",
										 "ноль", "нуль",
										 "ноля", "нуля",
										 "ноль", "нуль",
										 "нолю", "нулю",
										 "нолём", "нулём",
										 "ноле", "нуле", nullptr };

	static const SYNONYMS SYNONYMS_1 = { "один",
										 "один", "одна",
										 "одного", "одной",
										 "один", "одну",
										 "одному", "одной",
										 "одним", "одной",
										 "одином", "одной", nullptr };

	static const SYNONYMS SYNONYMS_2 = { "два",
										 "два", "две", "двух", "двух", "двум", "двумя", "двух", nullptr };

	static const SYNONYMS SYNONYMS_3 = { "три",
										 "три", "трёх", "три", "трём", "тремя", "трёх", nullptr };

	static const SYNONYMS SYNONYMS_4 = { "четыре",
										 "четыре", "четырёх", "четырёх", "четырём", "четырьмя", "четырёх", nullptr };

	static const SYNONYMS SYNONYMS_5 = { "пять",
										 "пять", "пяти", "пять", "пяти", "пятью", "пяти", nullptr };

	static const SYNONYMS SYNONYMS_6 = { "шесть",
										 "шесть", "шести", "шесть", "шести", "шестью", "шести", nullptr };

	static const SYNONYMS SYNONYMS_7 = { "семь",
										 "семь", "семи", "семь", "семи", "семью", "семи", nullptr };

	static const SYNONYMS SYNONYMS_8 = { "восемь",
										 "восемь", "восьми", "восемь", "восьми", "восемью", "восьми", nullptr };

	static const SYNONYMS SYNONYMS_9 = { "девять",
										 "девять", "девяти", "девять", "девяти", "девятью", "девяти", nullptr };

	static const SYNONYMS SYNONYMS_10 = { "десять",
										  "десять", "десяти", "десять", "десяти", "десятью", "десяти", nullptr };

	static const SYNONYMS SYNONYMS_11 = { "одиннадцать",
										  "одиннадцать", "одиннадцати", "одиннадцать", "одиннадцати", "одиннадцатью", "одиннадцати", nullptr };

	static const SYNONYMS SYNONYMS_12 = { "двенадцать",
										  "двенадцать", "двенадцати", "двенадцать", "двенадцати", "двенадцатью", "двенадцати", nullptr };

	static const SYNONYMS SYNONYMS_13 = { "тринадцать",
										  "тринадцать", "тринадцати", "тринадцать", "тринадцати", "тринадцатью", "тринадцати", nullptr };

	static const SYNONYMS SYNONYMS_14 = { "четырнадцать",
										  "четырнадцать", "четырнадцати", "четырнадцать", "четырнадцати", "четырнадцатью", "четырнадцати", nullptr };

	static const SYNONYMS SYNONYMS_15 = { "пятнадцать",
										  "пятнадцать", "пятнадцати", "пятнадцать", "пятнадцати", "пятнадцатью", "пятнадцати", nullptr };

	static const SYNONYMS SYNONYMS_16 = { "шестнадцать",
										  "шестнадцать", "шестнадцати", "шестнадцать", "шестнадцати", "шестнадцатью", "шестнадцати", nullptr };

	static const SYNONYMS SYNONYMS_17 = { "семнадцать",
										  "семнадцать", "семнадцати", "семнадцать", "семнадцати", "семнадцатью", "семнадцати", nullptr };

	static const SYNONYMS SYNONYMS_18 = { "восемнадцать",
										  "восемнадцать", "восемнадцати", "восемнадцать", "восемнадцати", "восемнадцать,", "восемнадцати", nullptr };

	static const SYNONYMS SYNONYMS_19 = { "девятнадцать",
										  "девятнадцать", "девятнадцати", "девятнадцать", "девятнадцати", "девятнадцатью", "девятнадцати", nullptr };



	static const SYNONYMS SYNONYMS_20 = { "двацать",
										  "двацать", "двацати", "двацать", "двацати", "двацатью", "двацати", nullptr };

	static const SYNONYMS SYNONYMS_30 = { "тридцать",
										  "тридцать", "тридцати", "тридцать", "тридцати", "тридцатью", "тридцати", nullptr };

	static const SYNONYMS SYNONYMS_40 = { "сорок",
										  "сорок", "сорока", "сорок", "сорока", "сорока", "сорока", nullptr };

	static const SYNONYMS SYNONYMS_50 = { "пятьдесят",
										  "пятьдесят", "пятидесяти", "пятьдесят", "пятидесяти", "пятидесятью", "пятидесяти", nullptr };

	static const SYNONYMS SYNONYMS_60 = { "шестьдесят",
										  "шестьдесят", "шестидесяти", "шестьдесят", "шестидесяти", "дестидесятью", "шестидесяти", nullptr };

	static const SYNONYMS SYNONYMS_70 = { "семьдесят",
										  "семьдесят", "семидесяти", "семьдесят", "семидесяти", "семидесятью", "семидесяти", nullptr };

	static const SYNONYMS SYNONYMS_80 = { "восемьдесят",
										  "восемьдесят", "восьмидесяти", "восемьдесят", "восьмидесяти", "восьмидесятью", "восьмидесяти", nullptr };

	static const SYNONYMS SYNONYMS_90 = { "девяносто",
										  "девяносто", "девяноста", "девяносто", "девяноста", "девяноста", "девяноста", nullptr };



	static const SYNONYMS SYNONYMS_100 = { "сто",
										   "сто", "ста", "сто", "стам", "стами", "ста", nullptr };

	static const SYNONYMS SYNONYMS_200 = { "двести",
										   "двести", "двухсот", "двести", "двумстам", "двумястами", "двухстах", nullptr };

	static const SYNONYMS SYNONYMS_300 = { "триста",
										   "триста", "трёхсот", "триста", "трёмстам", "тремястами", "трёхстах", nullptr };

	static const SYNONYMS SYNONYMS_400 = { "четыреста",
										   "четыреста", "четырёхсот", "четыреста", "четырёмстам", "четырьмястами", "четырёхстах", nullptr };

	static const SYNONYMS SYNONYMS_500 = { "пятьсот",
										   "пятьсот", "пятисот", "пятьсот", "пятистам", "пятьюстами", "пятистах", nullptr };

	static const SYNONYMS SYNONYMS_600 = { "шестьсот",
										   "шестьсот", "шестисот", "шестьсот", "шестистам", "шестьюстами", "шестистах", nullptr };

	static const SYNONYMS SYNONYMS_700 = { "семьсот",
										   "семьсот", "семисот", "семьсот", "семистам", "семьюстами", "семистах", nullptr };

	static const SYNONYMS SYNONYMS_800 = { "восемьсот",
										   "восемьсот", "восьмисот", "восемьсот", "восьмистам", "восемьюстами", "восьмистах", nullptr };

	static const SYNONYMS SYNONYMS_900 = { "девятьсот",
										   "девятьсот", "девятисот", "девятьсот", "девятистам", "девятьюстами", "девятистах", nullptr };



	static const SYNONYMS SYNONYMS_10_3 = { "тысяча",
											"тысяча",  "тысячи",
											"тысячи",  "тысяч",
											"тысячу",  "тысячи",
											"тысяче",  "тысячам",
											"тысячей", "тысячами",
											"тысяче",  "тысячах", nullptr };

	static const SYNONYMS SYNONYMS_10_6 = { "миллион",
											"миллион", "миллионы",
											"миллиона", "миллионов",
											"миллион", "миллионы",
											"миллиону", "миллионам",
											"миллионом", "миллионами",
											"миллионе", "миллионах", nullptr };

	static const SYNONYMS SYNONYMS_10_9 = { "миллиард",
											"миллиард", "миллиарды",
											"миллиарда", "миллиардов",
											"миллиард", "миллиарды",
											"миллиарду", "миллиардам",
											"миллиардом", "миллиардами",
											"миллиарде", "миллиардах", nullptr };

	static const SYNONYMS SYNONYMS_10_12 = { "триллион",
											 "триллион", "триллионы",
											 "триллиона", "триллионов",
											 "триллион", "триллионы",
											 "триллиону", "триллионам",
											 "триллионом", "триллионами",
											 "триллионе", "триллионах", nullptr };

	static const SYNONYMS SYNONYMS_10_15 = { "квадриллион",
											 "квадриллион", "квадриллионы",
											 "квадриллиона", "квадриллионов",
											 "квадриллион", "квадриллионы",
											 "квадриллиону", "квадриллионам",
											 "квадриллионом", "квадриллионами",
											 "квадриллионе", "квадриллионах", nullptr };

	static const SYNONYMS SYNONYMS_10_18 = { "квинтиллион",
											 "квинтиллион", "квинтиллионы",
											 "квинтиллиона", "квинтиллионов",
											 "квинтиллион", "квинтиллионы",
											 "квинтиллиону", "квинтиллионам",
											 "квинтиллионом", "квинтиллионами",
											 "квинтиллионе", "квинтиллионах", nullptr };

	static const SYNONYMS SYNONYMS_10_21 = { "секстиллион",
											 "секстиллион", "секстиллионы",
											 "секстиллиона", "секстиллионов",
											 "секстиллион", "секстиллионы",
											 "секстиллиону", "секстиллионам",
											 "секстиллионом", "секстиллионами",
											 "секстиллионе", "секстиллионах", nullptr };

	static const SYNONYMS SYNONYMS_10_24 = { "септиллион",
											 "септиллион", "септиллионы",
											 "септиллиона", "септиллионов",
											 "септиллион", "септиллионы",
											 "септиллиону", "септиллионам",
											 "септиллионом", "септиллионами",
											 "септиллионе", "септиллионах", nullptr };



	static const SYNONYMS SYNONYMS_0_1 = { "десятая",
										   "десятая", "десятые",
										   "десятой", "десятых",
										   "десятую", "десятые",
										   "десятой", "десятым",
										   "десятой", "десятыми",
										   "десятой", "десятых", nullptr };

	static const SYNONYMS SYNONYMS_0_2 = { "сотая",
										   "сотая", "сотые",
										   "сотой", "сотых",
										   "сотую", "сотые",
										   "сотой", "сотым",
										   "сотой", "сотыми",
										   "сотой", "сотых", nullptr };

	static const SYNONYMS SYNONYMS_0_3 = { "тысячная",
										   "тысячная", "тысячные",
										   "тысячной", "тысячных",
										   "тысячную", "тысячные",
										   "тысячной", "тысячным",
										   "тысячной", "тысячными",
										   "тысячной", "тысячных", nullptr };

	static const SYNONYMS SYNONYMS_0_4 = { "десятитысячная",
										   "десятитысячная", "десятитысячные",
										   "десятитысячной", "десятитысячных",
										   "десятитысячную", "десятитысячные",
										   "десятитысячной", "десятитысячным",
										   "десятитысячной", "десятитысячными",
										   "десятитысячной", "десятитысячных", nullptr };

	static const SYNONYMS SYNONYMS_0_5 = { "стотысячная",
										   "стотысячная", "стотысячные",
										   "стотысячной", "стотысячных",
										   "стотысячную", "стотысячные",
										   "стотысячной", "стотысячным",
										   "стотысячной", "стотысячными",
										   "стотысячной", "стотысячных", nullptr };

	static const SYNONYMS SYNONYMS_0_6 = { "миллионная",
										   "миллионная", "миллионные",
										   "миллионной", "миллионных",
										   "миллионную", "миллионные",
										   "миллионной", "миллионным",
										   "миллионной", "миллионными",
										   "миллионной", "миллионных", nullptr };

	static const SYNONYMS SYNONYMS_0_7 = { "десятимиллионная",
										   "десятимиллионная", "десятимиллионные",
										   "десятимиллионной", "десятимиллионных",
										   "десятимиллионную", "десятимиллионные",
										   "десятимиллионной", "десятимиллионным",
										   "десятимиллионной", "десятимиллионными",
										   "десятимиллионной", "десятимиллионных", nullptr };

	static const SYNONYMS SYNONYMS_0_8 = { "стомиллионная",
										   "стомиллионная", "стомиллионные",
										   "стомиллионной", "стомиллионных",
										   "стомиллионную", "стомиллионные",
										   "стомиллионной", "стомиллионным",
										   "стомиллионной", "стомиллионными",
										   "стомиллионной", "стомиллионных", nullptr };

	static const SYNONYMS SYNONYMS_0_9 = { "миллиардная",
										   "миллиардная", "миллиардные",
										   "миллиардной", "миллиардных",
										   "миллиардную", "миллиардные",
										   "миллиардной", "миллиардным",
										   "миллиардной", "миллиардными",
										   "миллиардной", "миллиардных", nullptr };

	static const SYNONYMS SYNONYMS_0_10 = { "десятимиллиардная",
											"десятимиллиардная", "десятимиллиардные",
											"десятимиллиардной", "десятимиллиардных",
											"десятимиллиардную", "десятимиллиардные",
											"десятимиллиардной", "десятимиллиардным",
											"десятимиллиардной", "десятимиллиардными",
											"десятимиллиардной", "десятимиллиардных", nullptr };

	static const SYNONYMS SYNONYMS_0_11 = { "стомиллиардная",
											"стомиллиардная", "стомиллиардные",
											"стомиллиардной", "стомиллиардных",
											"стомиллиардную", "стомиллиардные",
											"стомиллиардной", "стомиллиардным",
											"стомиллиардной", "стомиллиардными",
											"стомиллиардной", "стомиллиардных", nullptr };





	static const char * ZERO = "ноль";

	static StringToString FractionalBaseTable[] = {
		{ "десятая",           "10" },
		{ "сотая",             "100" },
		{ "тысячная",          "1000" },
		{ "десятитысячная",    "10000" },
		{ "стотысячная",       "100000" },
		{ "миллионная",        "1000000" },
		{ "десятимиллионная",  "10000000" },
		{ "стомиллионная",     "100000000" },
		{ "миллиардная",       "1000000000" },
		{ "десятимиллиардная", "10000000000" },
		{ "стомиллиардная",    "100000000000" },
	};

	static unsigned int FractionalBaseTableSize = sizeof(FractionalBaseTable) / sizeof(StringToString);


	static StringToString BaseTable[] = {
		{ "тысяча",      "1000" },
		{ "миллион",     "1000000" },
		{ "миллиард",    "1000000000" },
		{ "триллион",    "1000000000000" },
		{ "квадриллион", "1000000000000000" },
		{ "квинтиллион", "1000000000000000000" },
		{ "секстиллион", "1000000000000000000000" },
		{ "септиллион",  "1000000000000000000000000" },
	};

	static unsigned int BaseTableSize = sizeof(BaseTable) / sizeof(StringToString);


	static StringToString HundredTable[] = {
		{ "сто", "100" },
		{ "двести", "200" },
		{ "триста", "300" },
		{ "четыреста", "400" },
		{ "пятьсот", "500" },
		{ "шестьсот", "600" },
		{ "семьсот", "700" },
		{ "восемьсот", "800" },
		{ "девятьсот", "900" },
	};

	static unsigned int HundredTableSize = sizeof(HundredTable) / sizeof(StringToString);


	static StringToString DecadeTable[] = {
		{ "двадцать", "20" },
		{ "тридцать", "30" },
		{ "сорок", "40" },
		{ "пятьдесят", "50" },
		{ "шестьдесят", "60" },
		{ "семьдесят", "70" },
		{ "восемьдесят", "80" },
		{ "девяносто", "90" },
	};

	static unsigned int DecadeTableSize = sizeof(DecadeTable) / sizeof(StringToString);


	static StringToString UnitTable[] = {
		{ "один", "1" },
		{ "два", "2" },
		{ "три", "3" },
		{ "четыре", "4" },
		{ "пять", "5" },
		{ "шесть", "6" },
		{ "семь", "7" },
		{ "восемь", "8" },
		{ "девять", "9" },
		{ "десять", "10" },
		{ "одиннадцать", "11" },
		{ "двенадцать", "12" },
		{ "тринадцать", "13" },
		{ "четырнадцать", "14" },
		{ "пятнадцать", "15" },
		{ "шестнадцать", "16" },
		{ "семнадцать", "17" },
		{ "восемнадцать", "18" },
		{ "девятнадцать", "19" },
	};

	static unsigned int UnitTableSize = sizeof(UnitTable) / sizeof(StringToString);


	static const char * FractionalBase2Forms[] = {
		"десятая", "десятых",
		"сотая", "сотых",
		"тысячная", "тысячных",
		"десятитысячная", "десятитысячных",
		"стотысячная", "стотысячных",
		"миллионная", "миллионных",
		"десятимиллионная", "десятимиллионных",
		"стомиллионная", "стомиллионных",
		"миллиардная", "миллиардных",
		"десятимиллиардная", "десятимиллиардных",
		"стомиллиардная", "стомиллиардных"
	};

	static unsigned int FractionalBase2FormsSize = sizeof(FractionalBase2Forms) / sizeof(const char *);
	static unsigned int MaxFractionalDigitsCount = 11;

	static const char * Base3Forms[] = {
		"тысяча", "тысячи", "тысяч",
		"миллион", "миллиона", "миллионов",
		"миллиард", "миллиарда", "миллиардов",
		"триллион", "триллиона", "триллионов",
		"квадриллион", "квадриллиона", "квадриллионов",
		"квинтиллион", "квинтиллиона", "квинтиллионов",
		"секстиллион", "секстиллиона", "секстиллионов",
		"септиллион", "септиллиона", "септиллионов"
	};

	static unsigned int Base3FormsSize = sizeof(Base3Forms) / sizeof(const char *);
	static unsigned int MaxIntegralDigitsCount = 27;

	static const char * Hundred1Form[] = {
		"сто",
		"двести",
		"триста",
		"четыреста",
		"пятьсот",
		"шестьсот",
		"семьсот",
		"восемьсот",
		"девятьсот"
	};

	static unsigned int Hundred1FormSize = sizeof(Hundred1Form) / sizeof(const char *);

	static const char * Decade1Form[] = {
		"двадцать",
		"тридцать",
		"сорок",
		"пятьдесят",
		"шестьдесят",
		"семьдесят",
		"восемьдесят",
		"девяносто"
	};

	static unsigned int Decade1FormSize = sizeof(Decade1Form) / sizeof(const char *);

	static const char * Unit2Form[] = {
		"один", "одна",
		"два", "две",
		"три", "три",
		"четыре", "четыре",
		"пять", "пять",
		"шесть", "шесть",
		"семь", "семь",
		"восемь", "восемь",
		"девять", "девять",
		"десять", "десять",
		"одиннадцать", "одиннадцать",
		"двенадцать", "двенадцать",
		"тринадцать", "тринадцать",
		"четырнадцать", "четырнадцать",
		"пятнадцать", "пятнадцать",
		"шестнадцать", "шестнадцать",
		"семнадцать", "семнадцать",
		"восемнадцать", "восемнадцать",
		"девятнадцать", "девятнадцать"
	};

	static unsigned int Unit2FormSize = sizeof(Unit2Form) / sizeof(const char *);

	static const char * DelimiterString2Forms[] = { " целая и", " целых и" };

	static const char MINUS_STR[] = "минус";
	static const unsigned int MINUS_STR_LEN = sizeof( MINUS_STR ) - 1;

	static const char ZERO_STR[] = "ноль";
	static const unsigned int ZERO_STR_LEN = sizeof( ZERO_STR ) - 1;



	// Таблица синонимов.
	static NanoLib::NameIndex Synonyms;
	static avector<const char *> MainSynonyms;


	/**
	 *  Добавление группы синонимов в таблицу синонимов.
	 */
	static nlReturnCode AddSynonyms( const char * const* aSynonyms )
	{
		// Добавление группы синонимов в таблицу.
		for( const char * const * syn = aSynonyms + 1; *syn; ++syn )
		{
			// Добавление синонима.
			unsigned int id = 0;
			NanoLib::NameIndex::ReturnCode rc = Synonyms.AddName( *syn, id );
			if( NanoLib::NameIndex::rcSuccess != rc && NanoLib::NameIndex::rcElementExists != rc )
				return nlrcErrorInternal;

			// Если такой синоним уже есть в таблице, переходим к следующему.
			if( id < MainSynonyms.size() )
				continue;

			// Добавление ссылки на главный синоним.
			if( MainSynonyms.size() == id )
			{
				MainSynonyms.push_back( aSynonyms[0] );
				if( MainSynonyms.no_memory() )
					return nlrcErrorNoFreeMemory;
			}
			else
				return nlrcErrorInternal;
		}

		return nlrcSuccess;
	}

	/**
	 *  Инициализация объектов для разбора числительных.
	 */
	static nlReturnCode Initialize()
	{
		// Создание и заполнение таблицы синонимов.
		nlReturnCode nlrc = nlrcSuccess;
		Synonyms.Create();

		if( nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_SIGN_MINUS ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_SIGN_PLUS ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_INTEGRAL ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_0 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_1 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_2 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_3 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_4 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_5 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_6 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_7 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_8 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_9 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_10 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_11 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_12 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_13 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_14 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_15 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_16 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_17 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_18 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_19 ) ) ||

				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_20 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_30 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_40 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_50 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_60 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_70 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_80 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_90 ) ) ||

				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_100 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_200 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_300 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_400 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_500 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_600 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_700 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_800 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_900 ) ) ||

				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_10_3 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_10_6 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_10_9 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_10_12 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_10_15 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_10_18 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_10_21 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_10_24 ) ) ||

				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_0_1 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_0_2 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_0_3 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_0_4 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_0_5 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_0_6 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_0_7 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_0_8 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_0_9 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_0_10 ) ) ||
				nlrcSuccess != ( nlrc = AddSynonyms( SYNONYMS_0_11 ) ) )
			return nlrc;

		return nlrcSuccess;
	}


	bool CommonParser::vInitialized = false;

	nlReturnCode CommonParser::vStatusCode = nlrcUnsuccess;

	CommonParser::CommonParser()
	{
		if( CommonParser::vInitialized )
			return;

		vStatusCode = Initialize();

		if( nlrcSuccess == vStatusCode )
			CommonParser::vInitialized = true;
	}

	nlReturnCode CommonParser::Init()
	{
		return vStatusCode;
	}


	static inline nlReturnCode String2LongFloat( const char * aString, unsigned int aLength, StringToString * aTable, unsigned int aTableSize,  LongFloat & aResult )
	{
		if( !aLength )
			return nlrcUnsuccess;

		for( unsigned int i = 0; i < aTableSize; ++i )
		{
			if( !strncasecmp( aString, aTable[i].vFrom, aLength ) && aTable[i].vFrom[aLength] == '\0' )
			{
				if( !aResult.sscanf( aTable[i].vTo ) )
					return nlrcErrorInternal;
				return nlrcSuccess;
			}
		}

		return nlrcUnsuccess;

	}

	nlReturnCode FractionalBase2LongFloat( const char * aFractionalBase, unsigned int aLength, LongFloat & aResult )
	{
		return String2LongFloat( aFractionalBase, aLength, FractionalBaseTable, FractionalBaseTableSize, aResult );
	}

	nlReturnCode Base2LongFloat( const char * aBase, unsigned int aLength, LongFloat & aResult )
	{
		return String2LongFloat( aBase, aLength, BaseTable, BaseTableSize, aResult );
	}

	nlReturnCode Hundred2LongFloat( const char * aHundred, unsigned int aLength, LongFloat & aResult )
	{
		return String2LongFloat( aHundred, aLength, HundredTable, HundredTableSize, aResult );
	}

	nlReturnCode Decade2LongFloat( const char * aDecade, unsigned int aLength, LongFloat & aResult )
	{
		return String2LongFloat( aDecade, aLength, DecadeTable, DecadeTableSize, aResult );
	}

	nlReturnCode Units2LongFloat( const char * aUnit, unsigned int aLength, LongFloat & aResult )
	{
		return String2LongFloat( aUnit, aLength, UnitTable, UnitTableSize, aResult );
	}

	bool IsZero( const char * aWord, unsigned int aLength )
	{
		return ( !strncasecmp( ZERO, aWord, aLength ) && ZERO[aLength] == '\0' ) || !strncasecmp( "0", aWord, aLength );
	}


	nlReturnCode NormalizeNumeral( const char *& aNormalized, const char * aNumeral, unsigned int aNumeralLen )
	{
		// Поиск слова в таблице синонимов.
		const unsigned int * id = Synonyms.Search( aNumeral, aNumeralLen );
		if( !id )
		{
			aNormalized = nullptr;
			return nlrcSuccess;
		}

		// Выбор нормальной формы для найденного слова.
		if( *id >= MainSynonyms.size() )
			return nlrcErrorInternal;
		aNormalized = MainSynonyms[*id];
		return nlrcSuccess;
	}


	nlReturnCode NormalizeNumerals( aTextString & aResult, const char * aStr, unsigned int aStrLen )
	{
		if( !aStrLen )
			aStrLen = strlen( aStr );

		aResult.clear();
		unsigned int begin = 0;
		unsigned int end = 0;
		const char * norm = nullptr;
		nlReturnCode nlrc = nlrcSuccess;

		// Замена числительных на их нормальные формы.
		while( begin < aStrLen )
		{
			// Выделение слова.
			begin = TextFuncs::SkipSpaces( aStr, begin, aStrLen );
			if( begin >= aStrLen )
				break;
			end = begin;
			while( end < aStrLen && !TextFuncs::IsSpace( aStr[end] ) )
				++end;

			// Пропуск знака, написанного слитно со словом.
			if( end - begin > 1 && ( aStr[begin] == '-' || aStr[begin]=='+' ) )
			{
				if( nlrcSuccess != ( nlrc = aResult.append( aStr + begin, 1 ) ) )
					return nlrc;
				++begin;
			}

			// Поиск в таблице синонимов.
			if( nlrcSuccess != ( nlrc = NormalizeNumeral( norm, aStr + begin, end - begin ) ) )
				return nlrc;

			// Добавление пробела перед очередным словом в результате.
			if( aResult.size() )
			{
				if( nlrcSuccess != ( nlrc = aResult.append( " ", 1 ) ) )
					return nlrc;
			}

			// Замена слова на нормальную форму.
			if( norm )
			{
				if( nlrcSuccess != ( nlrc = aResult.append( norm ) ) )
					return nlrc;
			}
			else
			// Копирование слова.
			{
				if( nlrcSuccess != ( nlrc = aResult.append( aStr + begin, end - begin ) ) )
					return nlrc;
			}

			begin = end + 1;
		}

		return nlrcSuccess;
	}

	nlReturnCode DeleteSpaces( aTextString & aResult, const char * aStr, unsigned int aStrLen )
	{
		if( !aStrLen )
			aStrLen = strlen( aStr );

		// Проверка строки на пустоту.
		if( !aStrLen )
		{
			aResult.clear();
			return nlrcSuccess;
		}

		// Резервирование места под результат.
		nlReturnCode nlrc = aResult.resize( aStrLen );
		if( nlrcSuccess != nlrc )
			return nlrc;

		// Удаление лишних пробелов, склеивание соседних цифр.
		unsigned int pos = 0;
		for( unsigned int n = TextFuncs::SkipSpaces( aStr, 0, aStrLen ); n < aStrLen; ++n )
		{
			if( n > 0 && n < aStrLen - 1 )
			{
				if( TextFuncs::IsSpace( aStr[n] ) )
				{
					if( !TextFuncs::IsSpace( aStr[n + 1] ) && !( TextFuncs::IsDigit( aStr[n - 1] ) && TextFuncs::IsDigit( aStr[n + 1] ) ) )
						aResult[pos++] = aStr[n];
				}
				else
				{

					aResult[pos++] = aStr[n];
				}
			}
			else if( !TextFuncs::IsSpace( aStr[n] ) )
				aResult[pos++] = aStr[n];
		}

		// Установка корректного размера результата.
		if( nlrcSuccess != ( nlrc = aResult.resize( pos ) ) )
			return nlrc;

		return nlrcSuccess;
	}


	bool NoNumerals( const aTextString & aString )
	{
		// Поиск символов отличных от цифр, пробельных и десятичного разделителя.
		for( unsigned int n = 0; n < aString.size(); ++n )
			if( !TextFuncs::IsSpace( aString[n] ) && !TextFuncs::IsDigit( aString[n] ) && aString[n] != '.' )
				return false;

		return true;
	}

	nlReturnCode VoiceNumberToNormString( const LongFloat & aUnsignedInteger, aTextString & aResult )
	{
		//    printf( "\nLOG:\t\t\t Number2String   \"%s\"\n", aResult.ToConstChar() );

		// Очистка строки-результата.
		aResult.clear();
		LongFloat cur( aUnsignedInteger );
		nlReturnCode nlrc = nlrcSuccess;

		if( cur == 0 )
		{
			// Ноль.
			//        printf( "\nLOG:\t\t\t Number2String  SUCESS2\n" );
			return aResult.assign( STR_NORM_UNIT[0] );
		}
		else
		{
			// Количество разрядов, которые могут быть преобразованы в запись числа прописью.
			static const unsigned int base_count = sizeof(STR_NORM_BASE) / sizeof(const char *);
			struct triplet
			{
				unsigned int units;
				unsigned int decades;
				unsigned int hundreds;
			} base[sizeof(STR_NORM_BASE) / sizeof(const char *)];

			// Заполнение значений разрядов.
			int base_n = 0;

			char buf[1000];
			for(; base_n < base_count && cur >= 1; ++base_n )
			{
				cur.snprintf( buf, 1000, 10 );
				//            printf( "\nLOG:\t\t\t    cur=\"%s\"\n", buf );

				// Единицы
				base[base_n].units = cur % 100;
				base[base_n].units = base[base_n].units < 20 ? base[base_n].units : base[base_n].units % 10;

				// Десятки
				base[base_n].decades = ( cur % 100 ) / 10;
				base[base_n].decades = base[base_n].decades < 2 ? 0 : base[base_n].decades;

				// Сотни.
				base[base_n].hundreds = ( cur % 1000 ) / 100;

				cur /= 1000;
			}


			//        printf( "\nLOG:\t\t\t max_base_n=%i\n", base_n );

			// Число слишком большое, чтобы быть записанным прописью.
			if( cur >= 1 )
			{
				//            printf( "\nLOG:\t\t\t Number2String  FAIL1\n" );
				return nlrcUnsuccess;
			}

			// Формирование записи числа прописью.
			for( base_n = base_n - 1; base_n >= 0; --base_n )
			{
				// Сотни.
				if( base[base_n].hundreds )
				{
					if( nlrcSuccess != ( nlrc = aResult.append( STR_NORM_HUNDRED[base[base_n].hundreds - 1] ) ) ||
							nlrcSuccess != ( nlrc = aResult.append( " ", 1 ) ) )
					{
						//                    printf( "\nLOG:\t\t\t Number2String  FAIL2\n" );
						return nlrc;
					}
				}

				// Десятки.
				if( base[base_n].decades )
				{
					if( nlrcSuccess != ( nlrc = aResult.append( STR_NORM_DECADE[base[base_n].decades - 2] ) ) ||
							nlrcSuccess != ( nlrc = aResult.append( " ", 1 ) ) )
					{
						//                    printf( "\nLOG:\t\t\t Number2String  FAIL3\n" );
						return nlrc;
					}
				}

				// Единицы.
				if( base[base_n].units && !( base[base_n].units == 1 && base_n > 0 )  )
				{
					if( nlrcSuccess != ( nlrc = aResult.append( STR_NORM_UNIT[base[base_n].units] ) ) ||
							nlrcSuccess != ( nlrc = aResult.append( " ", 1 ) ) )
					{
						//                    printf( "\nLOG:\t\t\t Number2String  FAIL4\n" );
						return nlrc;
					}
				}

				// Разряд.
				if( base_n && ( base[base_n].units || base[base_n].decades || base[base_n].hundreds ) )
				{
					if( nlrcSuccess != ( nlrc = aResult.append( STR_NORM_BASE[base_n - 1] ) ) ||
							nlrcSuccess != ( nlrc = aResult.append( " ", 1 ) ) )
					{
						//                    printf( "\nLOG:\t\t\t Number2String  FAIL5\n" );
						return nlrc;
					}
				}
			}
		}

		//    printf( "\nLOG:\t\t\t Number2String  SUCCESS1\n" );

		return nlrcSuccess;
	}

	nlReturnCode PrepareVoiceNumber( aTextString & aResult, const char * aVoiceNumber, unsigned int aLen )
	{
		if( !aLen )
			aLen = strlen( aVoiceNumber );

		//    printf( "\nLOG:\t\t Prepare voice number   \"%s\"\n", aVoiceNumber );

		// Очистка результата.
		aResult.clear();

		nlReturnCode nlrc = nlrcSuccess;
		unsigned int begin = 0;
		unsigned int end = 0;

		// Замена чисел, записанных цифрами, на запись прописью.
		while( begin < aLen )
		{
			// Выделение слова.
			begin = TextFuncs::SkipSpaces( aVoiceNumber, begin, aLen );
			if( begin < aLen )
			{
				bool digits_only = true;
				end = begin;

				while( end < aLen && !TextFuncs::IsSpace( aVoiceNumber[end] ) )
				{
					digits_only &= TextFuncs::IsDigit( aVoiceNumber[end] );
					++end;
				}

				// Добавление к результату пробела.
				if( aResult.size() )
					if( nlrcSuccess != ( nlrc = aResult.append( " ", 1 ) ) )
						return nlrc;

				// Замена циферной записи числа на прописную.
				if( digits_only )
				{
					LongFloat tmp = 0;
					aTextString number;

					// Копирование циферной записи числа.
					if( nlrcSuccess != ( nlrc =  number.assign( aVoiceNumber + begin, end - begin ) ) )
						return nlrc;

					// Преобразование циферной записи числа в тип LongFloat.
					if( !tmp.sscanf( number.ToConstChar() ) )
						return nlrcUnsuccess;

					// Преобразование типа LongFloat в запись числа прописью.
					if( nlrcSuccess != ( nlrc = VoiceNumberToNormString( tmp, number ) ) )
						return nlrc;

					//                printf( "\nLOG:\t\t Change number to  \"%s\"\n", number.ToConstChar() );

					// Добавление полученной записи числа к результату.
					if( nlrcSuccess != ( nlrc = aResult.append( number ) ) )
						return nlrc;

				}
				else
				{
					// Замена десятичной точки на "и".
					if( end - begin == 1 && ( aVoiceNumber[begin] == '.' || aVoiceNumber[begin] == ',' ) )
					{
						if( nlrcSuccess != ( nlrc = aResult.append( "и", 1 ) ) )
							return nlrc;
					}
					// Копирование слова без изменений.
					else
					{
						if( nlrcSuccess != ( nlrc = aResult.append( aVoiceNumber + begin, end - begin ) ) )
							return nlrc;
					}
				}

				begin = end;
			}
		}

		return nlrcSuccess;
	}

	static nlReturnCode IntegralToString( const char * aIntegral, unsigned int aIntegralLen, aTextString & aNumeral, bool aIsPartOfFloatNumber )
	{
		// Проверка аргументов.
		if( !aIntegralLen )
			return nlrcSuccess;

		nlReturnCode nlrc = nlrcSuccess;

		// Если заданное число - ноль.
		bool is_zero = true;
		for( unsigned int pos = 0; pos < aIntegralLen && is_zero; ++pos )
			if( aIntegral[pos] != '0' )
				is_zero = false;
		if( is_zero )
		{
			if( aNumeral.size() && nlrcSuccess != ( nlrc = aNumeral.append( " ", 1 ) ) )
				return nlrc;
			return aNumeral.append( ZERO_STR, ZERO_STR_LEN );
		}

		// Текущий разряд.
		unsigned int base = (aIntegralLen - 1) / 3;
		// Текущая позиция.
		unsigned int pos = 0;
		unsigned int n = 0;


		// Преобразование числа из записи цифрами в запись прописью.
		while( pos < aIntegralLen )
		{
			// Флаг, показывающий, что сотни, десятки и единицы в текущем разряде отсутсвуют.
			bool is_zero = true;

			// Преобразование сотен.
			if( ( aIntegralLen - pos ) % 3 == 0 )
			{
				if( (n = aIntegral[pos] - '0') != 0 )
				{
					if( n > 9 )
						return nlrcUnsuccess;
					if( aNumeral.size() && nlrcSuccess != ( nlrc = aNumeral.append( " ", 1 ) ) )
						return nlrc;
					if( nlrcSuccess != ( nlrc = aNumeral.append( Hundred1Form[n - 1] ) ) )
						return nlrc;
					is_zero = false;
				}
				++pos;
			}

			// Преобразование десятков.
			if( ( 1 + aIntegralLen - pos ) % 3 == 0 )
			{

				if( ( n = aIntegral[pos] - '0' ) > 1 )
				{
					if( n > 9 )
						return nlrcUnsuccess;
					if( aNumeral.size() && nlrcSuccess != ( nlrc = aNumeral.append( " ", 1 ) ) )
						return nlrc;
					if( nlrcSuccess != ( nlrc = aNumeral.append( Decade1Form[n - 2] ) ) )
						return nlrc;
					is_zero = false;
				}
				++pos;
			}

			// Преобразование единиц.
			if( ( n = aIntegral[pos] - '0' + ( pos && aIntegral[pos - 1] == '1' ? 10 : 0 ) ) )
			{
				if( n > 19 )
					return nlrcUnsuccess;
				if( aNumeral.size() && nlrcSuccess != ( nlrc = aNumeral.append( " ", 1 ) ) )
					return nlrc;

				// Выбор падежа.
				if( base == 1 || ( base == 0 && aIsPartOfFloatNumber ) )
				{
					if( nlrcSuccess != ( nlrc = aNumeral.append( Unit2Form[(n - 1) * 2 + 1] ) ) )
						return nlrc;
				}
				else
				{
					if( nlrcSuccess != ( nlrc = aNumeral.append( Unit2Form[(n - 1) * 2] ) ) )
						return nlrc;
				}
				is_zero = false;

				//            printf( "\nLOG: IntegralToString units - Ok\n");
			}
			//        else
			//            printf( "\nLOG: IntegralToString no units\n");

			// Добавление обозначения разряда.
			if( !is_zero && base )
			{
				if( aNumeral.size() && nlrcSuccess != ( nlrc = aNumeral.append( " ", 1 ) ) )
					return nlrc;
				// Выбор падежа по количеству единиц.
				if( n == 1 )
				{
					if( nlrcSuccess != ( nlrc = aNumeral.append( Base3Forms[(base - 1) * 3] ) ) )
						return nlrc;
				}
				else if( n >=2 && n <= 4 )
				{
					if( nlrcSuccess != ( nlrc = aNumeral.append( Base3Forms[(base - 1) * 3 + 1] ) ) )
						return nlrc;
				}
				else
				{
					if( nlrcSuccess != ( nlrc = aNumeral.append( Base3Forms[(base - 1) * 3 + 2] ) ) )
						return nlrc;
				}

				//            printf( "\nLOG: IntegralToString base - Ok\n");
			}
			//        else
			//            printf( "\nLOG: IntegralToString no base\n");

			++pos;
			--base;
		}

		//    printf( "\nLOG: IntegralToString FINISH\n");

		return nlrcSuccess;
	}

	nlReturnCode NumberToString( const aTextString & aNumber, aTextString & aNumeral )
	{
		// Проверка аргумента на корректность.
		unsigned int pos = 0;
		for(; pos < aNumber.size(); ++pos )
		{
			if( aNumber[pos] == '.' || aNumber[pos] == ',' )
			{
				++pos;
				break;
			}
			if( !TextFuncs::IsDigit( aNumber[pos] ) )
				return nlrcUnsuccess;
		}
		for(; pos < aNumber.size(); ++pos )
			if( !TextFuncs::IsDigit( aNumber[pos] ) )
				return nlrcUnsuccess;

		nlReturnCode nlrc = nlrcSuccess;
		aNumeral.clear();
		// Проверка входной строки на пустоту.
		if( !aNumber.size() )
			return nlrcSuccess;

		// Определение местоположения десятичного разделителя.
		unsigned int dot_pos = 0;
		while( dot_pos < aNumber.size() && aNumber[dot_pos] != '.' && aNumber[dot_pos] != ',' )
			++dot_pos;

		// Учёт знака.
		unsigned int sign_len = aNumber[0] == '-' || aNumber[0] == '+' ? 1 : 0;
		bool is_negated = aNumber[0] == '-';

		// Проверка возможности представления числа такого данного числительным.
		if( dot_pos - sign_len > MaxIntegralDigitsCount || aNumber.size() - dot_pos > MaxFractionalDigitsCount + 1 )
		{
			return aNumeral.assign( aNumber );
		}
		else
		{
			// Добавоение к результату знака числа.
			if( is_negated )
				if( nlrcSuccess != ( nlrc = aNumeral.append( MINUS_STR, MINUS_STR_LEN ) ) )
					return nlrc;

			// Добавление к результату целой части числа.
			if( nlrcSuccess != ( nlrc = IntegralToString( aNumber.ToConstChar() + sign_len, dot_pos - sign_len, aNumeral, dot_pos < aNumber.size() ) ) )
				return nlrc;

			// Провка наличия дробной части.
			if( dot_pos < aNumber.size() )
			{
				// Добавление к результату разделителя целой и дробной части числа.
				if( aNumber[dot_pos - 1] == '1' && (dot_pos < 2 || aNumber[dot_pos - 2] != '1' ) )
				{
					if( nlrcSuccess != ( nlrc = aNumeral.append( DelimiterString2Forms[0] ) ) )
						return nlrc;
				}
				else
				{
					if( nlrcSuccess != ( nlrc = aNumeral.append( DelimiterString2Forms[1] ) ) )
						return nlrc;
				}

				// Добавление к результату дробной части.
				if( nlrcSuccess != ( nlrc = IntegralToString( aNumber.ToConstChar() + dot_pos + 1, aNumber.size() - dot_pos - 1, aNumeral, true ) ) )
					return nlrc;

				// Добавление к результату основания дробной части, с учётом падежа.
				unsigned int n = aNumber[aNumber.size() - 1] - '0';
				if( dot_pos <= aNumber.size() - 2 && aNumber[aNumber.size() - 2] == '1' )
					n += 10;
				if( nlrcSuccess != ( nlrc = aNumeral.append( " ", 1 ) ) )
					return nlrc;
				if( n == 1 )
				{
					if( nlrcSuccess != ( nlrc = aNumeral.append( FractionalBase2Forms[(aNumber.size() - dot_pos - 2) * 2] ) ) )
						return nlrc;
				}
				else
				{
					if( nlrcSuccess != ( nlrc = aNumeral.append( FractionalBase2Forms[(aNumber.size() - dot_pos - 2) * 2 + 1] ) ) )
						return nlrc;
				}
			}
		}

		return nlrcSuccess;
	}

}
