import GaussianElimination as ge

class PiecewiseRegression:
    def __init__(self, power, min_domain, max_domain):
        self._power = power
        self._min_domain = min_domain
        self._max_domain = max_domain

        self._functions = {}

    def populate_data(self, xpoints, ypoints):
        pass
        sections = {}
        for i in range(len(xpoints)):
            if (xpoints[i] // 1) in sections:
                sections[xpoints[i] // 1][0].append(xpoints[i])
                sections[xpoints[i] // 1][1].append(ypoints[i])
            else:
                sections[xpoints[i] // 1] = [[],[]]
                sections[xpoints[i] // 1][0].append(xpoints[i])
                sections[xpoints[i] // 1][1].append(ypoints[i])

        for key in sections:
            self.add_section(sections[key][0], sections[key][1])            

    def add_section(self, xpoints, ypoints):
        """
            Adds a regression to the piecewise function from the min of this
            mini-function's range to the max. Any x value called in the piecewise
            regression will be called in this regression.
        """
        matrix = [0] * self._power
        for row in range(self._power):
            matrix[row] = []
            for element in range(self._power):
                matrix[row].append(sum(pow(x, row + element) for x in xpoints))
            matrix[row].append(sum(pow(xpoints[i], row) * ypoints[i] for i in range(len(xpoints)) ))
        
        matrix[0][0] = len(xpoints) # This part's just kinda part of the formula
        
        self._functions[int(xpoints[0] // 1)] = ge.solve_matrix(matrix)

    def get_value_at(self, x):
        coeffecients = self._functions[int(x // 1)]
        val = 0
        for i in range(len(coeffecients)):
            val += coeffecients[i] * pow(x, i)
        return val



    def get_string_equation(self, coeffecients):
        equation = ""
        for i in range(len(coeffecients) - 1, -1, -1):
            equation += str(coeffecients[i]) + "x" + "^" + str(i) + " + "
        return equation