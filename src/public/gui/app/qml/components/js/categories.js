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

function calculateGraphData(book, date) {
    var year = date.getFullYear();
    var month = date.getMonth() + 1;  // months start at 0 because javascript
    var categoryStats = book.categoryPercentagesForMonth(month, year);
    var graphData = [];
    var categories = [];

    for(var index=0; index < categoryStats.length; index++) {
        var category = categoryStats[index]["category"];
        var amount = categoryStats[index]["amount"];
        console.log("Amount for category " + category.name + " is " + amount);
        var currentData = {
            value: amount,
            color: category.color
        };
        graphData.push(currentData);
        categories.push(category);
    }
    return {
        legend: categories,
        data: graphData
    };
}

function redrawGraph(book, chart, legendModel, date) {
    var percentages = calculateGraphData(book, date);
    chart.chartData = percentages.data;

    // we need to update the legend too
    var categories = percentages.legend;
    legendModel.clear();
    for(var index=0; index < categories.length; index++) {
        var category = categories[index];
        legendModel.append({"name":category.name, "color":category.color});
    }

    chart.repaint();
}
