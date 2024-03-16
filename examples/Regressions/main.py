import PiecewiseRegression as pr
import matplotlib.pyplot as plt
import ParametricPlot as pp

xpoints = []
ypoints = []

# populate the xpoints and ypoints arrays
with open("data.txt") as file:
    file_lines = file.readlines()
    for line in file_lines:
        line = line.split()
        xpoints.append(float(line[0]) / 1000) # divide by 1000 to convert to seconds <- this part's really important
        ypoints.append(float(line[1]))

function = pr.PiecewiseRegression(9, xpoints[0], xpoints[-1])
function.populate_data(xpoints, ypoints)

newypoints = []
for point in xpoints:
    newypoints.append(function.get_value_at(point))

parametric_plot = pp.ParametricPlot(function, function)
parametric_x = parametric_plot.get_x_points()
parametric_y = parametric_plot.get_y_points()

plt.plot(parametric_x, parametric_y)

# plt.plot(xpoints, ypoints, 'o')
# plt.plot(xpoints, newypoints)

plt.show()