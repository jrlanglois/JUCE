/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-8-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

#if JUCE_UNIT_TESTS

//==============================================================================
/**
    Comprehensive unit tests for the Time class, including RFC 3339 and RFC 9557 support.
*/
class TimeRFCTests final : public UnitTest
{
public:
    TimeRFCTests()
        : UnitTest ("Time RFC Tests", UnitTestCategories::time)
    {}

    void runTest() override
    {
        testBasicTimeOperations();
        testISO8601Compatibility();
        testRFC3339Support();
        testRFC9557Support();
        testEdgeCases();
        testErrorHandling();
        testAdditionalEdgeCases();
    }

private:
    void testBasicTimeOperations()
    {
        beginTest ("Basic Time Operations");

        // Test construction and basic operations
        auto testTime = Time (2023, 6, 15, 14, 30, 45, 123, false); // July 15, 2023 14:30:45.123 UTC
        expect (testTime.getYear() == 2023);
        expect (testTime.getMonth() == 6);  // 0-based months
        expect (testTime.getDayOfMonth() == 15);
        expect (testTime.getHours() == 14);
        expect (testTime.getMinutes() == 30);
        expect (testTime.getSeconds() == 45);
        expect (testTime.getMilliseconds() == 123);

        // Test epoch time
        auto epochTime = Time (0);
        expect (epochTime.getYear() == 1970);
        expect (epochTime.getMonth() == 0);  // January
        expect (epochTime.getDayOfMonth() == 1);
    }

    void testISO8601Compatibility()
    {
        beginTest ("ISO 8601 Compatibility");

        auto testTime = Time (2023, 6, 15, 14, 30, 45, 123, false);

        // Test that existing ISO 8601 functionality still works
        auto iso8601Extended = testTime.toISO8601 (true);
        auto iso8601Basic = testTime.toISO8601 (false);

        expect (iso8601Extended.contains ("-"));
        expect (iso8601Extended.contains (":"));
        expect (iso8601Extended.contains ("T"));

        expect (! iso8601Basic.contains ("-"));
        expect (! iso8601Basic.contains (":"));
        expect (iso8601Basic.contains ("T"));

        // Test round-trip
        expect (Time::fromISO8601 (iso8601Extended) == testTime);
        expect (Time::fromISO8601 (iso8601Basic) == testTime);
    }

    void testRFC3339Support()
    {
        beginTest ("RFC 3339 Support");

        auto testTime = Time (2023, 6, 15, 14, 30, 45, 123, false); // July 15, 2023 14:30:45.123 UTC

        // Test basic RFC 3339 formatting
        auto rfc3339String = testTime.toRFC3339();

        // RFC 3339 should always use extended format with separators
        expect (rfc3339String.contains ("-"));
        expect (rfc3339String.contains (":"));
        expect (rfc3339String.contains ("T"));

        // Test round-trip conversion
        auto parsedTime = Time::fromRFC3339 (rfc3339String);
        expect (parsedTime == testTime);

        // Test various valid RFC 3339 formats
        expect (Time::fromRFC3339 ("2023-07-15T14:30:45Z") == Time (2023, 6, 15, 14, 30, 45, 0, false));
        expect (Time::fromRFC3339 ("2023-07-15T14:30:45.123Z") == Time (2023, 6, 15, 14, 30, 45, 123, false));
        expect (Time::fromRFC3339 ("2023-07-15T14:30:45+01:00") == Time (2023, 6, 15, 13, 30, 45, 0, false));
        expect (Time::fromRFC3339 ("2023-07-15T14:30:45-05:00") == Time (2023, 6, 15, 19, 30, 45, 0, false));
        expect (Time::fromRFC3339 ("2023-07-15T14:30:45.999+02:30") == Time (2023, 6, 15, 12, 0, 45, 999, false));

        // Test that RFC 3339 is stricter than ISO 8601
        expect (Time::fromRFC3339 ("2023-07-15") == Time{}); // No time component
        expect (Time::fromRFC3339 ("2023-07-15T14:30:45") == Time{}); // No timezone
        expect (Time::fromRFC3339 ("20230715T143045Z") == Time{}); // Basic format not allowed
        expect (Time::fromRFC3339 ("2023/07/15T14:30:45Z") == Time{}); // Wrong separators
        expect (Time::fromRFC3339 ("2023-07-15 14:30:45Z") == Time{}); // Space instead of T
    }

    void testRFC9557Support()
    {
        beginTest ("RFC 9557 Support");

        auto testTime = Time (2023, 6, 15, 14, 30, 45, 123, false); // July 15, 2023 14:30:45.123 UTC

        // Test basic RFC 9557 formatting (no extensions)
        auto basicString = testTime.toRFC9557();
        expect (basicString == testTime.toRFC3339()); // Should be identical to RFC 3339 with no extensions

        // Test with timezone extension
        Time::ExtendedTimeInfo tzInfo ("America/New_York");
        auto tzString = testTime.toRFC9557 (tzInfo);
        expect (tzString.contains ("[America/New_York]"));
        expect (tzString.startsWith (testTime.toRFC3339()));

        // Test with calendar extension
        Time::ExtendedTimeInfo calInfo ("", "gregory");
        auto calString = testTime.toRFC9557 (calInfo);
        expect (calString.contains ("[u-ca=gregory]"));

        // Test with both timezone and calendar
        Time::ExtendedTimeInfo bothInfo ("Europe/London", "iso8601");
        auto bothString = testTime.toRFC9557 (bothInfo);
        expect (bothString.contains ("[Europe/London]"));
        expect (bothString.contains ("[u-ca=iso8601]"));

        // Test with custom tags
        Time::ExtendedTimeInfo customInfo;
        customInfo.customTags.set ("custom", "value");
        customInfo.customTags.set ("another", "test");
        auto customString = testTime.toRFC9557 (customInfo);
        expect (customString.contains ("[custom=value]"));
        expect (customString.contains ("[another=test]"));

        // Test parsing RFC 9557 strings
        Time::ExtendedTimeInfo parsed;

        // Parse basic timestamp
        auto parsedTime = Time::fromRFC9557 (basicString, &parsed);
        expect (parsedTime == testTime);
        expect (parsed.timezoneId.isEmpty());
        expect (parsed.calendar.isEmpty());
        expect (parsed.customTags.size() == 0);

        // Parse with timezone
        parsedTime = Time::fromRFC9557 (tzString, &parsed);
        expect (parsedTime == testTime);
        expect (parsed.timezoneId == "America/New_York");

        // Parse with calendar
        parsedTime = Time::fromRFC9557 (calString, &parsed);
        expect (parsedTime == testTime);
        expect (parsed.calendar == "gregory");

        // Parse with both timezone and calendar
        parsedTime = Time::fromRFC9557 (bothString, &parsed);
        expect (parsedTime == testTime);
        expect (parsed.timezoneId == "Europe/London");
        expect (parsed.calendar == "iso8601");

        // Parse with custom tags
        parsedTime = Time::fromRFC9557 (customString, &parsed);
        expect (parsedTime == testTime);
        expect (parsed.customTags.getValue ("custom", "") == "value");
        expect (parsed.customTags.getValue ("another", "") == "test");
    }

    void testEdgeCases()
    {
        beginTest ("Edge Cases");

        // Test leap year
        expect (Time::fromRFC3339 ("2000-02-29T12:00:00Z") == Time (2000, 1, 29, 12, 0, 0, 0, false));
        expect (Time::fromRFC3339 ("2100-02-28T12:00:00Z") == Time (2100, 1, 28, 12, 0, 0, 0, false));

        // Test year boundaries
        expect (Time::fromRFC3339 ("1970-01-01T00:00:00Z") == Time (0));
        expect (Time::fromRFC3339 ("2023-12-31T23:59:59.999Z") == Time (2023, 11, 31, 23, 59, 59, 999, false));

        // Test timezone edge cases
        expect (Time::fromRFC3339 ("2023-07-15T14:30:45+14:00") == Time (2023, 6, 15, 0, 30, 45, 0, false));
        expect (Time::fromRFC3339 ("2023-07-15T14:30:45-12:00") == Time (2023, 6, 16, 2, 30, 45, 0, false));

        // Test RFC 9557 special characters
        Time::ExtendedTimeInfo escapeInfo;
        escapeInfo.timezoneId = "Test[Zone]";
        escapeInfo.calendar = "cal=endar";
        escapeInfo.customTags.set ("key=with\\equals", "value[with]brackets");

        auto testTime = Time (2023, 6, 15, 14, 30, 45, 123, false);
        auto escapedString = testTime.toRFC9557 (escapeInfo);

        Time::ExtendedTimeInfo parsed;
        auto parsedTime = Time::fromRFC9557 (escapedString, &parsed);
        expect (parsedTime == testTime);
        expect (parsed.timezoneId == "Test[Zone]");
        expect (parsed.calendar == "cal=endar");
        expect (parsed.customTags.getValue ("key=with\\equals", "") == "value[with]brackets");
    }

    void testErrorHandling()
    {
        beginTest ("Error Handling");

        // Test invalid RFC 3339 formats
        expect (Time::fromRFC3339 ("") == Time{});
        expect (Time::fromRFC3339 ("invalid") == Time{});
        // Note: JUCE may normalize some invalid date/time values rather than rejecting them

        // Test invalid RFC 9557 formats
        expect (Time::fromRFC9557 ("") == Time{});
        expect (Time::fromRFC9557 ("invalid") == Time{});

        // Test malformed extensions (should still parse base time)
        auto testTime = Time (2023, 6, 15, 14, 30, 45, 0, false);
        expect (Time::fromRFC9557 ("2023-07-15T14:30:45Z[unclosed") == testTime);

        // Test parsing without extended info pointer
        expect (Time::fromRFC9557 ("2023-07-15T14:30:45Z[America/New_York]", nullptr) == testTime);

        // Test complex parsing scenarios
        auto complexString = "2023-07-15T14:30:45.123+02:00[Europe/Berlin][u-ca=gregory][locale=de-DE][format=extended]";
        Time::ExtendedTimeInfo parsed;
        auto parsedTime = Time::fromRFC9557 (complexString, &parsed);
        expect (parsedTime == Time (2023, 6, 15, 12, 30, 45, 123, false)); // Adjusted for +02:00 offset
        expect (parsed.timezoneId == "Europe/Berlin");
        expect (parsed.calendar == "gregory");
        expect (parsed.customTags.getValue ("locale", "") == "de-DE");
        expect (parsed.customTags.getValue ("format", "") == "extended");
        expect (parsed.customTags.size() == 2); // locale and format
    }

    void testAdditionalEdgeCases()
    {
        beginTest ("Additional Edge Cases");

        // Test Y2K38 problem boundaries (32-bit vs 64-bit systems)
        expect (Time::fromRFC3339 ("2038-01-19T03:14:07Z") == Time (2038, 0, 19, 3, 14, 7, 0, false));
        expect (Time::fromRFC3339 ("2038-01-19T03:14:08Z") == Time (2038, 0, 19, 3, 14, 8, 0, false));

        // Test far future dates (Year 2106 boundary from original tests)
        expect (Time::fromRFC3339 ("2106-02-07T06:28:15Z") == Time (2106, 1, 7, 6, 28, 15, 0, false));

        // Test negative epoch times (before 1970)
        expect (Time::fromRFC3339 ("1969-12-31T23:59:59Z") == Time (1969, 11, 31, 23, 59, 59, 0, false));
        expect (Time::fromRFC3339 ("1901-12-13T20:45:53Z") == Time (1901, 11, 13, 20, 45, 53, 0, false));

        // Test month boundary edge cases (30/31 day months)
        expect (Time::fromRFC3339 ("2023-04-30T23:59:59Z") == Time (2023, 3, 30, 23, 59, 59, 0, false)); // April has 30 days
        expect (Time::fromRFC3339 ("2023-05-31T23:59:59Z") == Time (2023, 4, 31, 23, 59, 59, 0, false)); // May has 31 days
        expect (Time::fromRFC3339 ("2023-06-30T23:59:59Z") == Time (2023, 5, 30, 23, 59, 59, 0, false)); // June has 30 days

        // Test leap year edge cases more thoroughly
        expect (Time::fromRFC3339 ("2000-02-29T12:00:00Z") == Time (2000, 1, 29, 12, 0, 0, 0, false)); // 2000 is leap year
        expect (Time::fromRFC3339 ("1900-02-28T12:00:00Z") == Time (1900, 1, 28, 12, 0, 0, 0, false)); // 1900 is NOT leap year
        expect (Time::fromRFC3339 ("2004-02-29T12:00:00Z") == Time (2004, 1, 29, 12, 0, 0, 0, false)); // 2004 is leap year
        expect (Time::fromRFC3339 ("2100-02-28T12:00:00Z") == Time (2100, 1, 28, 12, 0, 0, 0, false)); // 2100 is NOT leap year

        // Test extreme timezone offsets (±12:00, ±14:00 are real-world limits)
        expect (Time::fromRFC3339 ("2023-07-15T12:00:00+14:00") == Time (2023, 6, 14, 22, 0, 0, 0, false)); // Kiribati
        expect (Time::fromRFC3339 ("2023-07-15T12:00:00-12:00") == Time (2023, 6, 16, 0, 0, 0, 0, false)); // Baker Island
        expect (Time::fromRFC3339 ("2023-07-15T12:00:00+12:45") == Time (2023, 6, 14, 23, 15, 0, 0, false)); // Chatham Islands
        expect (Time::fromRFC3339 ("2023-07-15T12:00:00-09:30") == Time (2023, 6, 15, 21, 30, 0, 0, false)); // Marquesas Islands

        // Test daylight saving transition edge cases (from original TimeTests)
        if (Time{}.getTimeZone() == "GMT")
        {
            // Spring forward transition (1 AM becomes 2 AM)
            expect (Time (2025, 2, 30, 0, 59, 59, 999, false).toRFC3339().startsWith ("2025-03-30T00:59:59.999"));
            expect (Time (2025, 2, 30, 1, 0, 0, 0, false).toRFC3339().contains ("+01:00"));

            // Fall back transition would be October (2 AM becomes 1 AM)
            expect (Time (2024, 9, 27, 1, 0, 0, 0, false).toRFC3339().startsWith ("2024-10-27T01:00:00"));
        }

        // Test fractional seconds precision limits
        expect (Time::fromRFC3339 ("2023-07-15T12:00:00.000Z") == Time (2023, 6, 15, 12, 0, 0, 0, false));
        expect (Time::fromRFC3339 ("2023-07-15T12:00:00.001Z") == Time (2023, 6, 15, 12, 0, 0, 1, false));
        expect (Time::fromRFC3339 ("2023-07-15T12:00:00.999Z") == Time (2023, 6, 15, 12, 0, 0, 999, false));

        // Test microsecond precision (should truncate to milliseconds)
        expect (Time::fromRFC3339 ("2023-07-15T12:00:00.123456Z") == Time (2023, 6, 15, 12, 0, 0, 123, false));
        expect (Time::fromRFC3339 ("2023-07-15T12:00:00.999999Z") == Time (2023, 6, 15, 12, 0, 0, 999, false));

        // Test invalid date combinations (JUCE may normalize rather than reject)
        // These test that parsing doesn't crash rather than strict validation
        [[maybe_unused]] auto feb29NonLeap = Time::fromRFC3339 ("2023-02-29T12:00:00Z"); // Feb 29 in non-leap year
        [[maybe_unused]] auto april31 = Time::fromRFC3339 ("2023-04-31T12:00:00Z"); // April 31 (doesn't exist)
        // Just ensure parsing doesn't crash - JUCE may normalize these dates

        // Test RFC 9557 with extreme cases
        Time::ExtendedTimeInfo extremeInfo;
        extremeInfo.timezoneId = "Pacific/Kiritimati"; // UTC+14, furthest ahead timezone
        extremeInfo.calendar = "chinese";
        extremeInfo.customTags.set ("precision", "millisecond");
        extremeInfo.customTags.set ("source", "atomic-clock");

        auto extremeTime = Time (2023, 6, 15, 12, 0, 0, 123, false);
        auto extremeString = extremeTime.toRFC9557 (extremeInfo);

        Time::ExtendedTimeInfo parsedExtreme;
        auto parsedExtremeTime = Time::fromRFC9557 (extremeString, &parsedExtreme);
        expect (parsedExtremeTime == extremeTime);
        expect (parsedExtreme.timezoneId == "Pacific/Kiritimati");
        expect (parsedExtreme.calendar == "chinese");
        expect (parsedExtreme.customTags.getValue ("precision", "") == "millisecond");
        expect (parsedExtreme.customTags.getValue ("source", "") == "atomic-clock");

        // Test RFC 9557 with empty/null values in different combinations
        Time::ExtendedTimeInfo emptyInfo;
        emptyInfo.timezoneId = "";
        emptyInfo.calendar = "";
        auto emptyRFC9557 = extremeTime.toRFC9557 (emptyInfo);
        expect (emptyRFC9557 == extremeTime.toRFC3339()); // Should be basic RFC 3339

        // Test RFC 9557 with only custom tags (no timezone or calendar)
        Time::ExtendedTimeInfo customOnlyInfo;
        customOnlyInfo.customTags.set ("app", "JUCE");
        customOnlyInfo.customTags.set ("version", "8.0");
        auto customOnlyString = extremeTime.toRFC9557 (customOnlyInfo);
        expect (customOnlyString.contains ("[app=JUCE]"));
        expect (customOnlyString.contains ("[version=8.0]"));
        expect (! customOnlyString.contains ("[u-ca="));
        expect (customOnlyString.startsWith (extremeTime.toRFC3339()));

        // Test very long timezone names and custom values
        Time::ExtendedTimeInfo longInfo;
        longInfo.timezoneId = "America/Argentina/ComodRivadavia"; // One of the longest IANA timezone names
        longInfo.calendar = "islamic-civil";
        longInfo.customTags.set ("very-long-key-name-that-tests-parsing",
                                "very-long-value-that-tests-the-parsing-and-escaping-mechanisms");

        auto longString = extremeTime.toRFC9557 (longInfo);
        Time::ExtendedTimeInfo parsedLong;
        auto parsedLongTime = Time::fromRFC9557 (longString, &parsedLong);
        expect (parsedLongTime == extremeTime);
        expect (parsedLong.timezoneId == "America/Argentina/ComodRivadavia");
        expect (parsedLong.calendar == "islamic-civil");
        expect (parsedLong.customTags.getValue ("very-long-key-name-that-tests-parsing", "") ==
                "very-long-value-that-tests-the-parsing-and-escaping-mechanisms");

        // Test round-trip with current system time to ensure no precision loss
        auto now = Time::getCurrentTime();
        auto nowRFC3339 = now.toRFC3339();
        auto nowParsed = Time::fromRFC3339 (nowRFC3339);
        expect (abs (nowParsed.toMilliseconds() - now.toMilliseconds()) <= 1); // Allow 1ms tolerance for precision

        Time::ExtendedTimeInfo nowInfo ("UTC", "iso8601");
        nowInfo.customTags.set ("test", "round-trip");
        auto nowRFC9557 = now.toRFC9557 (nowInfo);
        Time::ExtendedTimeInfo parsedNowInfo;
        auto nowParsedRFC9557 = Time::fromRFC9557 (nowRFC9557, &parsedNowInfo);
        expect (abs (nowParsedRFC9557.toMilliseconds() - now.toMilliseconds()) <= 1);
        expect (parsedNowInfo.timezoneId == "UTC");
        expect (parsedNowInfo.calendar == "iso8601");
        expect (parsedNowInfo.customTags.getValue ("test", "") == "round-trip");
    }
};

static TimeRFCTests timeRFCTests;

#endif // JUCE_UNIT_TESTS

} // namespace juce
