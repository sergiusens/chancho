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

function canBeInit(accountsModel, incomeModel, expenseModel) {
    console.log("accounts " + accountsModel.count + " income " + incomeModel.count + " expense " + expenseModel.count);
    return accountsModel.count > 0 && incomeModel.count > 0 && expenseModel.count > 0;
}

function initDatabase(book, accountsModel, incomeModel, expenseModel) {
    var accounts = [];
    for(var index=0; index < accountsModel.count; index++) {
        var account = accountsModel.get(index);
        var obj = {
            "name": account.name,
            "memo": account.memo,
            "color": account.color,
            "amount": account.amount
        };
        accounts.push(obj);

        if (!success) {
            console.log("Error storing new account!");
        }
    }
    var success = book.storeAccounts(accounts);

    var cats = [];
    for(var index=0; index < incomeModel.count; index++) {
        var cat = incomeModel.get(index);
        var obj = {
            "name": cat.name,
            "color": cat.color,
            "type": book.INCOME
        };
        cats.push(obj);
    }

    for(var index=0; index < expenseModel.count; index++) {
        var cat = expenseModel.get(index);
        var obj = {
            "name": cat.name,
            "color": cat.color,
            "type": book.EXPENSE
        };
        cats.push(obj);
    }
    success = book.storeCategories(cats);
}
