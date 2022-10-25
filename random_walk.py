"""
    States:
        0 <-> A <-> B <-> C <-> D <-> E <-> 1
        0, 1 are terminal state
    Transition:
        Every transition returns reward of 0 except for E to 1
"""
import numpy as np
import matplotlib.pyplot as plt
np.random.seed(0)

"""
    Simulate a random walk
    Args:
        current_state: Current state Value
    Returns:
        tuple(int, float): next state and the reward, if the next state is terminal then it return -1
"""
def random_walk(current_state:int):
    current_state += 1 if np.random.uniform() >= 0.5 else -1
    if current_state == 5:
        return (-1, 1.0) 
    elif current_state == -1:
        return (-1, 0.0)
    return (current_state, 0.0)

def get_total_reward(rewards:list, gamma):
    size = len(rewards)
    total_reward = 0.0
    for i in range(size):
        total_reward += (gamma ** i) * rewards[i] 
    return total_reward

def get_average_reward():
    value_function = [0.5] * 5
    total_reward = [0.0] * 5
    count = [0] * 5
    for _ in range(10000):
        initial_state = np.random.randint(0, 5)
        current_state = initial_state
        count[current_state] += 1
        while True:
            if current_state ==  -1:
                break

            # take a step
            next_state, reward = random_walk(current_state)
            total_reward[initial_state] += reward
            current_state = next_state 
            
    for i in range(5):
        value_function[i] = total_reward[i] / count[i]

    return value_function

def simulate(iteration = 1000, alpha = 0.1):
    value_function = [0.5] * 5
    for _ in range(iteration):
        current_state = 2
        while True:
            if current_state ==  -1:
                break

            # take a step
            next_state, reward = random_walk(current_state)
            next_state_value = 0 if next_state == -1 else value_function[next_state]
            value_function[current_state] += alpha * (reward + next_state_value - value_function[current_state])
            current_state = next_state 
            

    return value_function

if __name__ == "__main__":
    # simulate(100)
    iterations = [0, 1, 10, 100, 1000]
    for iteration in iterations:
        value_function = simulate(iteration)
        plt.plot(["A", "B", "C", "D" , "E"], value_function, label=f"{iteration}")

    average_reward = get_average_reward()
    plt.plot(["A", "B", "C", "D" , "E"], average_reward, label = "Average reward")
    plt.legend()
    plt.show()