import numpy as np
from itertools import permutations

class Stats:
    def __init__(self) -> None:
        self.fitness_history = []
        self.network = None
        self.fitness = None

class Params():
    def __init__(self, data, T0, p, alpha, mutation_prob, max_iterations, max_tests) -> None:
        self.data = data
        self.T0 = T0
        self.p = p
        self.alpha = alpha
        self.mutation_prob = mutation_prob
        self.max_iterations = max_iterations
        self.max_tests = max_tests

def comparator(x, y):
    if x > y:
        return y, x
    else:
        return x, y
    
def num_of_comparators(network):
    return sum(map(lambda t: t[-1], network))

def init_network(n):
    network = []
    for i in range(n):
        for j in range(i+1, n):
            network.append((i, j, True))
    return network

def apply_network(network, data):
    sorted_data = data.copy()
    for i, j, used in network:
        if used:
            sorted_data[i], sorted_data[j] = comparator(sorted_data[i], sorted_data[j])
    return sorted_data

def count_errors(network, input):
    ground_truth = np.sort(input)
    sorted_input = apply_network(network, input)
    return sum(1 for i in range(len(input)) if sorted_input[i] != ground_truth[i])

def test_max_n_cases(network, data, n):
    if np.math.factorial(len(data)) > n:
        return np.array([count_errors(network, input=np.random.permutation(data)) for _ in range(int(n))])
    else:
        return np.array([count_errors(network, np.array(permutation)) for permutation in list(permutations(data))])

def fitness(network, data, max_tests):
    errors = sum(test_max_n_cases(network, data, max_tests))
    fitness = num_of_comparators(network) + 10*errors**2
    return fitness

def mutate(network, prob):
    return list(map(lambda t: (t[0], t[1], not t[2]) if np.random.random() < prob else t, network))

def linear_temperature(T, alpha):
    """Generator for exponential temperature function"""
    while True:
        yield T
        T = T - alpha if T > alpha else 0

def power_temperature(T0, alpha, p):
    """Generator for power temperature function"""
    t = 0
    while True:
        yield T0 / (1 + (1-alpha) * t ** p)
        t += 1

def geometric_temperature(T, alpha):
    while True:
        yield T
        T *= alpha

def simulated_annealing(params:Params):
    stats = Stats()
    current_network = init_network(len(params.data))
    current_fitness = fitness(current_network, params.data, params.max_tests)
    best_network = current_network
    best_fitness = current_fitness
    T = params.T0
    eps = 1e-6
    schedule = power_temperature(T, params.alpha, params.p)
    for i in range(params.max_iterations):
        new_network = list(current_network)
        i, j = np.random.choice(range(len(new_network)), 2, replace=False)
        new_network[i], new_network[j] = new_network[j], new_network[i]
        new_network = mutate(new_network, params.mutation_prob)
        new_fitness = fitness(new_network, params.data, params.max_tests)
        delta_fitness = new_fitness - current_fitness
        if delta_fitness < 0 or np.random.random() < np.exp(-delta_fitness / (T + eps)):
            current_network = new_network
            current_fitness = new_fitness
        if current_fitness < best_fitness:
            best_network = current_network
            best_fitness = current_fitness
        T = next(schedule)
        stats.fitness_history.append(best_fitness)
    stats.network = best_network
    stats.fitness = best_fitness
    return stats
