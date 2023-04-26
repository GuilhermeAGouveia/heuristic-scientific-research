import random

class AntColony:
    def __init__(self, num_ants, num_iterations, num_cities, distances, alpha=1, beta=3, evaporation=0.5):
        self.num_ants = num_ants
        self.num_iterations = num_iterations
        self.num_cities = num_cities
        self.distances = distances
        self.alpha = alpha
        self.beta = beta
        self.evaporation = evaporation
        self.pheromone = [[1 / (num_cities * num_cities) for j in range(num_cities)] for i in range(num_cities)]

    def run(self):
        best_distance = float('inf')
        best_solution = []
        for i in range(self.num_iterations):
            solutions = self.generate_solutions()
            self.update_pheromone(solutions)
            distance, solution = self.get_best_solution(solutions)
            if distance < best_distance:
                best_distance = distance
                best_solution = solution
        return best_solution, best_distance

    def generate_solutions(self):
        solutions = []
        for ant in range(self.num_ants):
            solution = self.generate_solution()
            solutions.append(solution)
        return solutions

    def generate_solution(self):
        visited_cities = set()
        current_city = random.randint(0, self.num_cities - 1)
        visited_cities.add(current_city)
        solution = [current_city]
        while len(visited_cities) < self.num_cities:
            next_city = self.choose_next_city(current_city, visited_cities)
            solution.append(next_city)
            visited_cities.add(next_city)
            current_city = next_city
        return solution

    def choose_next_city(self, current_city, visited_cities):
        unvisited_cities = [city for city in range(self.num_cities) if city not in visited_cities]
        pheromone_values = [self.pheromone[current_city][city] ** self.alpha * (1 / self.distances[current_city][city]) ** self.beta for city in unvisited_cities]
        total_pheromone = sum(pheromone_values)
        probabilities = [pheromone / total_pheromone for pheromone in pheromone_values]
        next_city_index = self.choose_index_by_probability(probabilities)
        return unvisited_cities[next_city_index]

    def choose_index_by_probability(self, probabilities):
        random_value = random.uniform(0, 1)
        probability_sum = 0
        for i in range(len(probabilities)):
            probability_sum += probabilities[i]
            if random_value <= probability_sum:
                return i

    def update_pheromone(self, solutions):
        for i in range(self.num_cities):
            for j in range(self.num_cities):
                if i != j:
                    pheromone_delta = sum([(1 / self.distances[i][j]) for solution in solutions if (i, j) in zip(solution, solution[1:])])
                    self.pheromone[i][j] = self.evaporation * self.pheromone[i][j] + pheromone_delta

    def get_best_solution(self, solutions):
        best_distance = float('inf')
        best_solution = None
        for solution in solutions:
            distance = self.get_distance(solution)
            if distance < best_distance:
                best_distance = distance
                best_solution = solution
        return best_distance, best_solution

    def get_distance(self, solution):
        distance = 0
        for i in range(len(solution) - 1):
            distance += self.distances[solution[i]][solution[i + 1]]
            distance += self.distances[solution[-1]][solution[0]]
        return distance
distances = [
    [0, 10, 15, 20],
    [10, 0, 35, 25],
    [15, 35, 0, 30],
    [20, 25, 30, 0]
]
colony = AntColony(num_ants=10, num_iterations=50, num_cities=4, distances=distances)
best_solution, best_distance = colony.run()
print(f"Melhor solução: {best_solution}")
print(f"Melhor distância: {best_distance}")
