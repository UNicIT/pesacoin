#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-PSA");
    BOOST_CHECK(GetBoolArg("-PSA"));
    BOOST_CHECK(GetBoolArg("-PSA", false));
    BOOST_CHECK(GetBoolArg("-PSA", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-PSAo"));
    BOOST_CHECK(!GetBoolArg("-PSAo", false));
    BOOST_CHECK(GetBoolArg("-PSAo", true));

    ResetArgs("-PSA=0");
    BOOST_CHECK(!GetBoolArg("-PSA"));
    BOOST_CHECK(!GetBoolArg("-PSA", false));
    BOOST_CHECK(!GetBoolArg("-PSA", true));

    ResetArgs("-PSA=1");
    BOOST_CHECK(GetBoolArg("-PSA"));
    BOOST_CHECK(GetBoolArg("-PSA", false));
    BOOST_CHECK(GetBoolArg("-PSA", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noPSA");
    BOOST_CHECK(!GetBoolArg("-PSA"));
    BOOST_CHECK(!GetBoolArg("-PSA", false));
    BOOST_CHECK(!GetBoolArg("-PSA", true));

    ResetArgs("-noPSA=1");
    BOOST_CHECK(!GetBoolArg("-PSA"));
    BOOST_CHECK(!GetBoolArg("-PSA", false));
    BOOST_CHECK(!GetBoolArg("-PSA", true));

    ResetArgs("-PSA -noPSA");  // -PSA should win
    BOOST_CHECK(GetBoolArg("-PSA"));
    BOOST_CHECK(GetBoolArg("-PSA", false));
    BOOST_CHECK(GetBoolArg("-PSA", true));

    ResetArgs("-PSA=1 -noPSA=1");  // -PSA should win
    BOOST_CHECK(GetBoolArg("-PSA"));
    BOOST_CHECK(GetBoolArg("-PSA", false));
    BOOST_CHECK(GetBoolArg("-PSA", true));

    ResetArgs("-PSA=0 -noPSA=0");  // -PSA should win
    BOOST_CHECK(!GetBoolArg("-PSA"));
    BOOST_CHECK(!GetBoolArg("-PSA", false));
    BOOST_CHECK(!GetBoolArg("-PSA", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--PSA=1");
    BOOST_CHECK(GetBoolArg("-PSA"));
    BOOST_CHECK(GetBoolArg("-PSA", false));
    BOOST_CHECK(GetBoolArg("-PSA", true));

    ResetArgs("--noPSA=1");
    BOOST_CHECK(!GetBoolArg("-PSA"));
    BOOST_CHECK(!GetBoolArg("-PSA", false));
    BOOST_CHECK(!GetBoolArg("-PSA", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-PSA", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-PSA", "eleven"), "eleven");

    ResetArgs("-PSA -psa");
    BOOST_CHECK_EQUAL(GetArg("-PSA", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-PSA", "eleven"), "");

    ResetArgs("-PSA=");
    BOOST_CHECK_EQUAL(GetArg("-PSA", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-PSA", "eleven"), "");

    ResetArgs("-PSA=11");
    BOOST_CHECK_EQUAL(GetArg("-PSA", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-PSA", "eleven"), "11");

    ResetArgs("-PSA=eleven");
    BOOST_CHECK_EQUAL(GetArg("-PSA", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-PSA", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-PSA", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-PSA", 0), 0);

    ResetArgs("-PSA -psa");
    BOOST_CHECK_EQUAL(GetArg("-PSA", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-psa", 11), 0);

    ResetArgs("-PSA=11 -psa=12");
    BOOST_CHECK_EQUAL(GetArg("-PSA", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-psa", 11), 12);

    ResetArgs("-PSA=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-PSA", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--PSA");
    BOOST_CHECK_EQUAL(GetBoolArg("-PSA"), true);

    ResetArgs("--PSA=verbose --psa=1");
    BOOST_CHECK_EQUAL(GetArg("-PSA", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-psa", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noPSA");
    BOOST_CHECK(!GetBoolArg("-PSA"));
    BOOST_CHECK(!GetBoolArg("-PSA", true));
    BOOST_CHECK(!GetBoolArg("-PSA", false));

    ResetArgs("-noPSA=1");
    BOOST_CHECK(!GetBoolArg("-PSA"));
    BOOST_CHECK(!GetBoolArg("-PSA", true));
    BOOST_CHECK(!GetBoolArg("-PSA", false));

    ResetArgs("-noPSA=0");
    BOOST_CHECK(GetBoolArg("-PSA"));
    BOOST_CHECK(GetBoolArg("-PSA", true));
    BOOST_CHECK(GetBoolArg("-PSA", false));

    ResetArgs("-PSA --noPSA");
    BOOST_CHECK(GetBoolArg("-PSA"));

    ResetArgs("-noPSA -PSA"); // PSA always wins:
    BOOST_CHECK(GetBoolArg("-PSA"));
}

BOOST_AUTO_TEST_SUITE_END()
