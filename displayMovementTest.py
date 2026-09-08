import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.animation import ArtistAnimation
import seaborn as sns

"""
Creates a plot of the creature's movement over time where the hue reflects time.
Known water and food sources are plotted as well to make sure the plot is interperable
"""
def display_movement(creatures : pd.DataFrame,
    water : pd.DataFrame,
    food : pd.DataFrame):
    #these helper functions put the water/food on the figure/axes
    def print_water(row):
        width = row.right - row.left
        height =row.upper - row.lower
        ax.add_patch(patches.Rectangle((row.left, row.lower), width, height))
    def print_food(row):
        dot = plt.Circle((row.x, row.y), radius = 10, color = 'green')
        ax.add_artist(dot)
    #should the circles for food actual circles
    plt.axis("equal")
    fig, ax = plt.subplots()
    sns.scatterplot(creatures, x= 'Position_x', y = 'Position_y', hue = 'Time',
        ax = ax
    )

    water.apply(print_water, axis = 1)
    food.apply(print_food, axis = 1)

    return fig, ax

"""
Works similarly to display_movement but instead of a scatter plot with varying hue
to illustrate time, uses an animation
"""
def movement_animation(creatures : pd.DataFrame,
    water : pd.DataFrame,
    food : pd.DataFrame):
    #these build up artists that record info about the environment
    #these funcs are designed to be passed to data frames with apply
    def make_pond(row):
        width = row.right - row.left
        height =row.upper - row.lower
        rect = ax.add_patch(patches.Rectangle((row.left, row.lower), width, height,
            color = "blue", figure = fig))
        ponds.append(rect)
    def make_food(row):
        circ = ax.add_patch(patches.Circle((row.x, row.y), radius = 10, color = 'green',
            figure = fig
        ))
        bushes.append(circ)
    def make_frame(i):
        def fill_positions(row):
            circ = ax.add_patch(patches.Circle((row['Position_x'], row['Position_y']),
            radius = 10))
            answer.append(circ)
        current_positions = creatures.loc[creatures['Time']==i]
        answer = [ax]
        current_positions.apply(fill_positions, axis = 1)
        answer.extend(ponds)
        answer.extend(bushes)
        return answer
    plt.axis("equal")
    fig, ax = plt.subplots()
    ponds = []
    bushes = []
    water.apply(make_pond, axis =1)
    food.apply(make_food, axis =1)

    ax.set_xlim(creatures['Position_x'].min()-10,
        creatures['Position_x'].max() + 10)
    ax.set_ylim( creatures['Position_y'].min() - 10,
        creatures['Position_y'].max() + 10)

    start = creatures['Time'].min()
    end = creatures['Time'].max()
    artist_list = [make_frame(i) for i in range(start,end)]
    return ArtistAnimation(fig, artist_list, interval = 100)

def collect_run_data(prefix : str):
    plt.close()
    #retrieve the data
    true_prefix = "MovementRecords/" + prefix
    creatures = pd.read_csv(true_prefix + "_position.csv")
    food = pd.read_csv(true_prefix + "_food.csv")
    water = pd.read_csv(true_prefix + "_water.csv")

    #display all of it
    fig, ax = display_movement(creatures, water, food)
    fig.savefig(true_prefix + "scatter_full.png")
    ani = movement_animation(creatures, water, food)
    ani.save(true_prefix + "animation_full.gif")
    plt.close()

    fig, ax = display_movement(creatures.iloc[:300], water, food)
    fig.savefig(true_prefix + "scatter_start.png")
    ani = movement_animation(creatures.iloc[:300], water, food)
    ani.save(true_prefix + "animation_start.gif")
    plt.close()
    fig, ax = display_movement(creatures.iloc[-300:], water, food)
    fig.savefig(true_prefix + "scatter_end.png")
    ani = movement_animation(creatures.iloc[-300:], water, food)
    ani.save(true_prefix + "animation_end.gif")
    plt.close()



if __name__ == "__main__":
    prefix = "ForceBasedWandering"
    collect_run_data(prefix + str(0))
    collect_run_data(prefix + str(1))
