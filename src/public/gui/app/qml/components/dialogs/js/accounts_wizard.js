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
 * Callback to be performed when the ok button from the WizardNewAccountDialog is clicked. The callback will
 * check the input from the different fields and will add a new model to the passed list model.
 *
 * @param nameField The field where the user types the name of the account.
 * @param initialAmountField The field where the user types the initial amount of the account.
 * @param memoField The field where the user types the memo information.
 * @param colorChooser A component that is used to show the color to be used with the account.
 * @param model A model that will receive new accounts if all the information is correct. The new added object will
 *              have ne of the form {name, color, memo, amount}
 */
function onAddAccountOkClicked(nameField, initialAmountField, memoField, colorChooser, model) {
    var amount = 0;
    if (initialAmountField.text != "") {
        amount = initialAmountField.text;
        amount = amount.replace(",", ".");
        amount = parseFloat(amount);
    }

    // a little hacky, but we need a string
    var color = "" + colorChooser.color;
    console.log("Adding new account to wizard "
        + "{'name': " + nameField.text + ", 'color': " + color
        + ", 'memo': " + memoField.text + ", 'amount': " + amount);

    model.append({
        "name": nameField.text,
        "color":color,
        "memo": memoField.text,
        "amount": amount
    });
}
