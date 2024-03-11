# Program that will solve a systems of equations in matrix form by using a gaussian
# And then a backwards substitution
# All the functions are basically static functions. They don't need to belong as part of a class

def gaussian_elimination(matrix):
    # Go through each row first
    for row in range(len(matrix)):
        # f for factor
        ftoOne = matrix[row][row]
        for element in range(len(matrix[row])):
            matrix[row][element] /= ftoOne

        # r for the remaining rows
        for r in range(1 + row, len(matrix), 1):
            ftoZero = matrix[row][row] * -matrix[r][row]
            for element in range(len(matrix[row])):
                matrix[r][element] += matrix[row][element] * ftoZero
    return matrix

def backwards_substitution(matrix):
    # Find first solution
    solutions = [1] * len(matrix)
    solutions[len(matrix) - 1] = matrix[len(matrix) - 1][len(matrix[len(matrix) - 1]) - 1]

    # Go through each row and plug in numbers for each variable
    for row in range(len(matrix) - 2, -1, -1): # <---- Moving backwards in the loop
        solution = 0
        for element in range(row + 1, len(matrix[row]) - 1):
            solution += matrix[row][element] * solutions[element]
        # move variable to other side of the equation
        solutions[row] = matrix[row][len(matrix[len(matrix) - 1]) - 1] - solution

    return solutions

def solve_matrix(matrix):
    matrix = gaussian_elimination(matrix)
    return backwards_substitution(matrix)

# test =  [
#     [3, -5, 3, 7, 4],
#     [4, 7, 2, 1, 2],
#     [1, 8, 7, -2, 7],
#     [1, 8, 7, 0, 9]]

# test = gaussian_elimination(test)

# solutions = backwards_substitution(test)
# print(solutions)
# for row in test:
    # print(row)