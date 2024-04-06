import json
import matplotlib.pyplot as plt
import numpy

def convertData(dataArray, fileName):
    testFile = open(fileName, "r")
    
    # converts json data to a readable python data type
    incommingData = json.load(testFile)
    
    #strips json data from data-packet, converts to string
    dataString = incommingData["data-packet"]
    dataString = dataString.split(", ")
    
    # adds data string to end of data array
    dataArray += dataString

    testFile.close()
    
    # returns concatenated dataArray
    return dataArray

def graphData(xData, yData, xAxisTitle = "", yAxisTitle = "", dataTitle = ""):
    # creates subplot of xData and yData
    fig, ax = plt.subplots()
    ax.plot(xData, yData)
    
    # sets axis titles
    ax.set(xlabel = xAxisTitle, ylabel = yAxisTitle, title = dataTitle)
    
    # shows grid lines and plots data
    ax.grid
    plt.show()



testFileName = ["examples\json_data\data-packet-one.json",
                "examples\json_data\data-packet-two.json",
                "examples\json_data\data-packet-three.json"]

dataArray = []

for i in testFileName:
    dataArray = convertData(dataArray, i)

# x-data arbitrary numbering system for total number of data plots
x = numpy.arange(0.0, len(dataArray), 1)

# function call for graphing data
graphData(x, dataArray, "Numbers", "Data Values", "Ben Has a Large Cock")