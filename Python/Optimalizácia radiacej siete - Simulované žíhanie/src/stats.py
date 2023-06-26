import matplotlib.pyplot as plt
import numpy as np
import os
from typing import List
import annealing as an

def best_error_fitness_history(runs, path, data, max_final_tests):
    best = min(runs, key=lambda x:x.fitness)
    print("Best sorting network:", best.network)
    print("Fitness:", best.fitness)
    print("Num of comparators:", an.num_of_comparators(best.network))

    results = an.test_max_n_cases(best.network, data, max_final_tests)
    all = len(results)
    errors = results[results != 0]
    invalid = len(errors)
    valid = len(results) - invalid
    accuracy = valid/all
    error = 1 - accuracy
    mean_error = np.mean(errors) if len(errors) > 0 else 0

    fig, ax = plt.subplots(figsize=(12.8, 7.2), dpi=100)
    bars, counts = np.unique(errors, return_counts=True)
    print(bars, counts)
    plt.bar(bars, counts, alpha=0.6)
    plt.xticks(bars, bars)
    textstr = '\n'.join((
        f"presnosť = {round(accuracy,3)}",
        f"chyba = {round(error,3)}",
        f'nesprávnych = {invalid}',
        f"priemerná \nveľkosť chyby = {round(mean_error, 3)}"))

    props = dict(boxstyle='round', facecolor='white', alpha=0.3)
    plt.text(0.832, 0.98, textstr, verticalalignment='top', horizontalalignment='left', transform=ax.transAxes, bbox=props)
    plt.xlabel("chyby")
    plt.ylabel("permutácie")
    fig.savefig(os.path.join(path, "all_errors.png"))

    fig = plt.figure(figsize=(12.8, 7.2), dpi=100)
    plt.plot(best.fitness_history)
    fig.savefig(os.path.join(path, "best_fitness_history.png"))

def fitness_history_graph(runs, path, max_iterations):
    fitness_history_runs = np.array([run.fitness_history for run in runs])

    mins = np.min(fitness_history_runs, axis=0)
    meds = np.median(fitness_history_runs, axis=0)
    maxs = np.max(fitness_history_runs, axis=0)
    var = np.var(fitness_history_runs, axis=0)

    fig = plt.figure(figsize=(12.8, 7.2), dpi=100)
    x = np.arange(1,max_iterations+1)
    plt.xscale('log')
    plt.plot(x,meds, label="median")
    plt.plot(x,mins, label="minimum")
    plt.fill_between(x,mins,maxs,alpha=0.4)
    plt.fill_between(x,meds-var,meds+var,alpha=0.2)
    plt.legend()
    plt.xlabel("počet iterácií")
    plt.ylabel("fitness")
    fig.savefig(os.path.join(path, "fitness_history.png"))

def plot_graphs(params, runs, path, max_final_tests):
    best_error_fitness_history(runs, path, params.data, max_final_tests)
    fitness_history_graph(runs, path, params.max_iterations)

def boxplot_graph(runs_to_compare, path):
    plt.figure(figsize=(12.8, 7.2), dpi=100)
    plt.boxplot([np.array([run.fitness for run in runs]) for runs in runs_to_compare], notch=True)
    plt.savefig(os.path.join(path, "boxplot.png"))

def run_mul_runs(params, num_of_runs, path, name, max_final_tests) -> List[an.Stats]:
    path = os.path.join(path, name)
    if not os.path.exists(path):
        os.mkdir(path)
    runs = []
    for i in range(num_of_runs):
        runs.append(an.simulated_annealing(params))
        np.save(os.path.join(path, "runs.npy"), np.array(runs))
        print(i+1, end=". ", flush=True)
    plot_graphs(params, runs, path, max_final_tests)
    print()
    return runs
        

params = an.Params(
    data = np.arange(1,16+1),
    T0 = 100.0,
    p = 2,
    alpha = 0.80,
    mutation_prob = 0.005,
    max_iterations = 25000,
    max_tests = 200
)
max_final_tests = 1e6

path="/stats"
if not os.path.exists(path):
    os.mkdir(path)

run_mul_runs(params, 3, path, "pow", max_final_tests)

# params.alpha = 0.9
# print(params.__dict__)
# run1 = run_mul_runs(params, 30, path, "run1", max_final_tests)
# params.alpha = 0.95
# print(params.__dict__)
# run2 = run_mul_runs(params, 30, path, "run2", max_final_tests)
# params.alpha = 0.99
# print(params.__dict__)
# run3 = run_mul_runs(params, 30, path, "run3", max_final_tests)

# boxplot_graph([run1, run2, run3], path)
