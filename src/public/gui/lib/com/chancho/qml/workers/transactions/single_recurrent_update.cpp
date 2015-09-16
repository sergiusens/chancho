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

#include "single_recurrent_update.h"

namespace com {

namespace chancho {

namespace qml {

namespace workers {

namespace transactions {

SingleRecurrentUpdate::SingleRecurrentUpdate(BookPtr book, chancho::RecurrentTransactionPtr trans, chancho::AccountPtr acc,
                                             chancho::CategoryPtr cat, QDate date, QString contents, QString memo,
                                             double amount, bool updateAll)
        : workers::Worker(),
          _book(book),
          _trans(trans),
          _acc(acc),
          _cat(cat),
          _date(date),
          _contents(contents),
          _memo(memo),
          _amount(amount),
          _updateAll(updateAll){
}

void
SingleRecurrentUpdate::run() {
    // decide if we need to perform the update
    auto requiresUpdate = _trans->transaction->account != _acc || _trans->transaction->category != _cat
                          || _trans->recurrence->startDate != _date || _trans->transaction->contents != _contents
                          || _trans->transaction->memo != _memo || _trans->transaction->amount != _amount;

    if (requiresUpdate) {
        _trans->transaction->account = _acc;
        _trans->transaction->category = _cat;
        _trans->recurrence->startDate = _date;
        _trans->transaction->contents = _contents;
        _trans->transaction->memo = _memo;
        _trans->transaction->amount = _amount;

        _book->store(_trans, _updateAll);
        if (_book->isError()) {
            emit failure();
            return;
        }
    }
    emit success();
}

}
}
}
}
}
