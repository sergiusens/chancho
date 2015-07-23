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
 * Function that returns the number of selected items in a model.
 * @param model The model that is under test.
 * @returns {number} The number of items selected in the model.
 */
function numberOfSelectedItems(model) {
    var count = 0;
    for(var index=0; index < model.count; index++) {
        var acc = model.get(index);
        if (acc.selected) {
            count += 1;
        }
    }
    return count;
}

/**
 * Function that returns if the model has a selected item.
 * @param model The model to look for selected items.
 * @returns {boolean} If there are any selected items.
 */
function modelHasSelectedItem(model) {
    for(var index=0; index < model.count; index++) {
        var acc = model.get(index);
        if (acc.selected) {
            return true;
        }
    }
    return false;
}

/**
 * Function that returns if there is a category selected either in the income model or
 * expense model.
 *
 * @param incomeModel The model used to hold the income categories.
 * @param expenseModel The model used to hold the expense categories.
 * @returns {boolean} If there are any categories selected.
 */
function areCategoriesSelected(incomeModel, expenseModel) {
    return modelHasSelectedItem(incomeModel) || modelHasSelectedItem(expenseModel);
}

/**
 * Removes all selected categories from the given model.
 *
 * @param model The model that contains the categories.
 */
function deleteCategoriesFromModel(model) {
    var catIndexes = [];
    for(var index=0; index < model.count; index++) {
        var acc = model.get(index);
        if (acc.selected) {
            catIndexes.push(index);
        }
    }
    catIndexes.reverse();
    for(var index=0; index < catIndexes.length; index++) {
        model.remove(catIndexes[index]);
    }
}

/**
 * Removes all selected categories from the income and expense models.
 *
 * @param incomeModel The model that contains the income categories.
 * @param expenseModel The model that contains the expense categories.
 */
function deleteCategoriesCallback(incomeModel, expenseModel) {
    deleteCategoriesFromModel(incomeModel);
    deleteCategoriesFromModel(expenseModel);
}
