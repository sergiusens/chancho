/*
 * Copyright (c) 2015 Manuel de la Peña <mandel@themacaque.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <QDebug>
#include "public_recurrence.h"
#include "test_recurrence.h"

namespace chancho = com::chancho;
void
TestRecurrence::init() {
    BaseTestCase::init();
}

void
TestRecurrence::cleanup() {
    BaseTestCase::cleanup();
}

void
TestRecurrence::testRecurrenceDailySameDay() {
    auto today = QDate::currentDate();
    PublicRecurrence recurrence(chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, today);
    auto result = recurrence.generateMissingDates();
    QCOMPARE(result.count(), 0);
}

void
TestRecurrence::testRecurrenceDailyFromStart() {
    int numberDays = 10;
    auto startDate = QDate::currentDate().addDays(-1 * numberDays);
    PublicRecurrence recurrence(chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, startDate);
    auto result = recurrence.generateMissingDates();
    QCOMPARE(result.count(), numberDays);
    QCOMPARE(result.first(), startDate.addDays(1));
    QCOMPARE(result.last(), QDate::currentDate());
}

void
TestRecurrence::testRecurrenceDailyNotFromStart() {
    int numberDays = 20;
    auto startDate = QDate::currentDate().addDays(-1 * numberDays);
    auto lastGenerated = startDate.addDays(numberDays/2);
    PublicRecurrence recurrence(chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, startDate);
    recurrence.lastGenerated = lastGenerated;
    auto result = recurrence.generateMissingDates();
    QCOMPARE(result.count(), numberDays/2);
    QCOMPARE(result.first(), lastGenerated.addDays(1));
    QCOMPARE(result.last(), QDate::currentDate());
}

void
TestRecurrence::testRecurrenceDailyOcurrenceLeft() {
    int numberDays = 10;
    int performed = 4;
    auto startDate = QDate::currentDate().addDays(-1 * numberDays);
    PublicRecurrence recurrence(chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, startDate, 11);
    recurrence.lastGenerated = startDate.addDays(performed);
    auto result = recurrence.generateMissingDates();
    QCOMPARE(result.count(), numberDays - performed);
    QCOMPARE(result.first(), recurrence.lastGenerated.addDays(1));
    QCOMPARE(result.last(), QDate::currentDate());
}

void
TestRecurrence::testRecurrenceDailyNoMore() {
    int numberDays = 10;
    int performed = 4;
    auto startDate = QDate::currentDate().addDays(-1 * numberDays);
    PublicRecurrence recurrence(chancho::RecurrentTransaction::Recurrence::Defaults::DAILY, startDate, performed);
    recurrence.lastGenerated = startDate.addDays(performed);
    auto result = recurrence.generateMissingDates();
    QCOMPARE(result.count(), 0);
}

void
TestRecurrence::testRecurrenceWeeklySameDay() {
    auto today = QDate::currentDate();
    PublicRecurrence recurrence(chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY, today);
    auto result = recurrence.generateMissingDates();
    QCOMPARE(result.count(), 0);
}

void
TestRecurrence::testRecurrenceWeeklyFromStart() {
    int numberDays = 21;
    auto startDate = QDate::currentDate().addDays(-1 * numberDays);
    PublicRecurrence recurrence(chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY, startDate);
    auto result = recurrence.generateMissingDates();
    QCOMPARE(result.count(), numberDays/7);
    QCOMPARE(result.first(), startDate.addDays(7));
    QCOMPARE(result.last(), QDate::currentDate());
}

void
TestRecurrence::testRecurrenceWeeklyFromStartNotLast() {
    int numberDays = 24;
    auto startDate = QDate::currentDate().addDays(-1 * numberDays);
    PublicRecurrence recurrence(chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY, startDate);
    auto result = recurrence.generateMissingDates();
    QCOMPARE(result.count(), numberDays/7);
    QCOMPARE(result.first(), startDate.addDays(7));
    QVERIFY(result.last() != QDate::currentDate());
}

void
TestRecurrence::testRecurrenceWeeklyNotFromStart() {
    int numberDays = 7 * 8;
    auto startDate = QDate::currentDate().addDays(-1 * numberDays);
    auto lastGenerated = startDate.addDays(14);
    PublicRecurrence recurrence(chancho::RecurrentTransaction::Recurrence::Defaults::WEEKLY, startDate);
    recurrence.lastGenerated = lastGenerated;
    auto result = recurrence.generateMissingDates();
    QCOMPARE(result.count(), (numberDays - 14)/7);
    QCOMPARE(result.first(), lastGenerated.addDays(7));
    QCOMPARE(result.last(), QDate::currentDate());
}

void
TestRecurrence::testRecurrenceMonthlySameDay() {
    auto today = QDate::currentDate();
    PublicRecurrence recurrence(chancho::RecurrentTransaction::Recurrence::Defaults::MONTHLY, today);
    auto result = recurrence.generateMissingDates();
    QCOMPARE(result.count(), 0);
}

void
TestRecurrence::testRecurrenceMonthlyFromStart() {
    int numberMonths = 5;
    auto startDate = QDate::currentDate().addMonths(-1 * numberMonths);
    PublicRecurrence recurrence(chancho::RecurrentTransaction::Recurrence::Defaults::MONTHLY, startDate);
    auto result = recurrence.generateMissingDates();
    QCOMPARE(result.count(), numberMonths);
    QCOMPARE(result.first(), startDate.addMonths(1));
    QCOMPARE(result.last(), QDate::currentDate());
}

void
TestRecurrence::testRecurrenceMonthlyNotFromStart() {
    int numberMonths = 20;
    auto startDate = QDate::currentDate().addMonths(-1 * numberMonths);
    auto lastGenerated = startDate.addMonths(numberMonths/2);
    PublicRecurrence recurrence(chancho::RecurrentTransaction::Recurrence::Defaults::MONTHLY, startDate);
    recurrence.lastGenerated = lastGenerated;
    auto result = recurrence.generateMissingDates();
    QCOMPARE(result.count(), numberMonths/2);
    QCOMPARE(result.first(), lastGenerated.addMonths(1));
    QCOMPARE(result.last(), QDate::currentDate());
}

QTEST_MAIN(TestRecurrence)
