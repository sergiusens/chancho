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

#pragma once

#include <com/chancho/recurrent_transaction.h>

class PublicRecurrence : public com::chancho::RecurrentTransaction::Recurrence {
 public:
    PublicRecurrence(int n, QDate s, QDate e=QDate())
            : com::chancho::RecurrentTransaction::Recurrence(n, s, e) {}

    PublicRecurrence(int n, QDate s, boost::optional<int> o)
            : com::chancho::RecurrentTransaction::Recurrence(n, s, o) {}

    PublicRecurrence(com::chancho::RecurrentTransaction::Recurrence::Defaults n, QDate s, QDate e=QDate())
            : com::chancho::RecurrentTransaction::Recurrence(n, s, e) {}

    PublicRecurrence(com::chancho::RecurrentTransaction::Recurrence::Defaults n, QDate s, boost::optional<int> o)
            : com::chancho::RecurrentTransaction::Recurrence(n, s, o) {}

    using com::chancho::RecurrentTransaction::Recurrence::generateMissingDates;
};
