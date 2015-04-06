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
 * Callback to be executed when the user clicks the add button in the categories page in the wizard. The callback
 * reads all the information and updates the correct model according to the user selection.
 *
 * @param nameField The field where the user types the name of the category.
 * @param typeOptionPicker The option picker that states the type of category.
 * @param colorChooser The components that is used to display the color that the category is going to use.
 * @param incomeModel The model used to show the income categories.
 * @param expenseModel The model used to show the expense categories.
 */
function onAddCategoryOkClicked(nameField, typeOptionPicker, colorChooser, incomeModel, expenseModel) {
    var model = expenseModel;
    if (typeOptionPicker.selectedIndex == 1) {
        model = incomeModel;
    }

    var color = "" + colorChooser.color;
    console.log("Adding new category to wizard "
        + "{'name': " + nameField.text + ", 'color': " + color);

    model.append({
        "name": nameField.text,
        "color": color
    });
}
