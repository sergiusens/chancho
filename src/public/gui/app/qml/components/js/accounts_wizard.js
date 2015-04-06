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

/**
 * Returns if there are any accounts selected in the given model.
 *
 * @param model The model that contains all the accounts added so far.
 * @returns {boolean} If there are any accounts with the selected flag set to true.
 *
 */
function areAccountsSelected(model) {
    for(var index=0; index < model.count; index++) {
        var acc = model.get(index);
        if (acc.selected) {
            return true;
        }
    }
    return false;
}

/**
 * Deletes all those selected accounts from the passed model.
 *
 * @param model The model that contains al the idfferent categories.
 */
function deleteAccountsCallback(model) {
    var accIndexes = [];
    for(var index=0; index < model.count; index++) {
        var acc = model.get(index);
        if (acc.selected) {
            accIndexes.push(index);
        }
    }
    accIndexes.reverse();
    for(var index=0; index < accIndexes.length; index++) {
        model.remove(accIndexes[index]);
    }
}

