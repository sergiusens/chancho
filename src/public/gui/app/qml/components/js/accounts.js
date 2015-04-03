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
 * This functions returns the data to be displayed in a line graph for the given accounts.
 *
 * @param {Book} book The object that is used to access the information from the database.
 * @param {[Account]} accounts The accounts whose data we are going to calculate.
 * @param {Date} date The date for which the graph is calculated.
 */
function calculateGraphData(book, accounts, date) {
    var accountsData = [];
    var chartData = {
        labels: [
            i18n.tr("January"),
            i18n.tr("February"),
            i18n.tr("March"),
            i18n.tr("April"),
            i18n.tr("May"),
            i18n.tr("June"),
            i18n.tr("July"),
            i18n.tr("August"),
            i18n.tr("September"),
            i18n.tr("October"),
            i18n.tr("November"),
            i18n.tr("December")],
        datasets: []
    }

    for (var index=0; index < accounts.length; index++) {
        var account = accounts[index];
        // the result is the total amounts per month without taking into account the accounts
        // total, that is to allow updates in the diff transactions. To show it in the ui
        // we need to remove those amounts from the total in reverse order
        var monthsAmounts = book.monthsTotalForAccount(account, date.getFullYear()).reverse();
        var currentAccountTotal = account.amount;
        var graphData = [];
        for(var dataIndex=0; dataIndex < monthsAmounts.length; dataIndex++) {
            currentAccountTotal -= monthsAmounts[dataIndex];
            graphData[dataIndex] = currentAccountTotal;
        }

        graphData = graphData.reverse();
        // we need to remove the info about all those months we yet have not lived, so, is usually the last x months
        var month = date.getMonth();
        graphData = graphData.slice(0, month + 1);

        var color = account.color;
        var darkColor = Qt.darker(color, 1.2);
        var lightColor = Qt.lighter(color, 1.6);
        lightColor.a = 0.5;

        var item = {
            label: account.name,
            fillColor: lightColor,
            strokeColor: color,
            pointColor: darkColor,
            pointStrokeColor: "#fff",
            pointHighlightFill: "#fff",
            pointHighlightStroke: "rgba(220,220,220,1)",
            data: graphData
        };
        accountsData.push(item);
    }
    chartData.datasets = accountsData;
    return chartData;
}

/**
 * This function takes care of redrawing an accounts chart when ever is needed.
 *
 * @param {Book} book The object that can be used to get the information out of the database.
 * @param {QChart} chart The chart to redraw.
 * @param {[Account]} accounts The accounts that are displayed in the chart.
 * @param {Date} date The date when the graph data is calculated
 */
function redrawGraph(book, chart, accounts, date) {
    var data = calculateGraphData(book, accounts, date);
    chart.chartData = data;
    chart.repaint();
}
