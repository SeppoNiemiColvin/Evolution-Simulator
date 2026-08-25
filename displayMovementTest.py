import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import seaborn as sns

def display_movement(creatures : pd.DataFrame,
    water : pd.DataFrame,
    food : pd.DataFrame):
    def print_water(row):
        width = row.right - row.left
        height =row.upper - row.lower
        ax.add_patch(patches.Rectangle((row.left, row.lower), width, height))
    def print_food(row):
        dot = plt.Circle((row.x, row.y), radius = 10, color = 'green')
        ax.add_artist(dot)

    #plot locations over time
    plt.axis("equal")
    fig, ax = plt.subplots()
    sns.scatterplot(creatures, x= 'Position_x', y = 'Position_y', hue = 'Time',
        ax = ax
    )

    water.apply(print_water, axis = 1)
    food.apply(print_food, axis = 1)

    return fig, ax

if __name__ == "__main__":
    prefix = "wander"
    creatures = pd.read_csv(prefix + "_position.csv")
    food = pd.read_csv(prefix + "_food.csv")
    water = pd.read_csv(prefix + "_water.csv")

    print(creatures.tail())
    print(food.head())
    print(water.head())
    display_movement(creatures, water, food)
    display_movement(creatures.iloc[:500], water, food)
    display_movement(creatures.iloc[-500:], water, food)
    plt.show()
