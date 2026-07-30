import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

#loads the data for the basic simulation
def load_basic_sim():
    sim1 = pd.read_csv('BasicOnlyFoodSim1.csv')
    sim2 = pd.read_csv('BasicOnlyFoodSim2.csv')
    sim3 = pd.read_csv('BasicOnlyFoodSim3.csv')
    return pd.concat([sim1,sim2,sim3], axis =1,
        keys = [1,2,3], names = ['Simulation', 'Property'])

def load_fixed_density():
    prefix = "BasicOnlyFoodSim"
    keys = [1] + list(range(4,12))
    return pd.concat([pd.read_csv(prefix + str(key) + ".csv") for key in keys],
        axis = 1, keys = keys, names = ['Simulation', 'Property'])

#loads data for the simulations with water/thirst
def load_water_sim():
    sim1 = pd.read_csv('withWater1.csv')
    sim2 = pd.read_csv('withWater2.csv')
    return pd.concat([sim1, sim2], axis =1,
        keys = [1,2], names = ['Simulation', 'Property'])

#creates a figure that displays the property as a line chart with hue
#representing which simulation the properyt came from
def compare_sims_line(sim_data : pd.DataFrame, property : str):
    property_data = sim_data.xs(property, level = "Property", axis = 1).stack()
    property_data = property_data.reset_index()
    property_data = property_data.rename(
        columns = {'level_0': 'Time', 0: property})
    return sns.lineplot(property_data, x= 'Time', y = property,
        hue = 'Simulation', palette = 'deep')

#Plots the relation between to two different properties across th simulations
#the property on the x axis defaults to population. It plots around 500 points
#from each simulation spaced evenly apart in time to reduce overcrouding
def compare_sims_scatter(sim_data : pd.DataFrame, property_y :str,
    property_x :str = 'Population'):
    sim_data = sim_data.swaplevel(axis =1)[[property_x, property_y]]
    step_size = sim_data.shape[0] // 200
    sim_data = sim_data.iloc[::step_size].stack().reset_index(level = 1)
    return sns.scatterplot(sim_data,
        x = property_x, y = property_y, hue = 'Simulation', palette = 'deep')

if __name__== "__main__":
    sims_basic = load_basic_sim()
    sims_water = load_water_sim()
    combined = sims_basic[[1,2]].join(sims_water,
        lsuffix= " w/o water", rsuffix = " with water")
    print(combined.tail())
    compare_sims_scatter(combined, "Speed")
    plt.show()
