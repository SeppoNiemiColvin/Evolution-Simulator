import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import math

#loads the data for the basic simulation
def load_simulation():
    def pull_data(sim : int, run :int, with_thirst :bool):
        file = "simulation" + str(sim) + "_run"+ str(run)
        if with_thirst:
            file += "_withThirst.csv"
        else:
            file += "_withoutThirst.csv"
        data = pd.read_csv(file).reset_index()
        data['Simulation'] = sim
        data['Run'] = run
        data['With Thirst'] = with_thirst
        return data
    dataframes = []

    for sim in range(7):
        for run in range(4):
            data = pull_data(sim, run, True)
            if data.shape[0] >= 1e4:
                dataframes.append(data)
            dataframes.append(pull_data(sim, run, False))
    return pd.concat(dataframes).rename(columns = {'index' : 'Time'})

def line_plot(data, property: str):
    data = data.iloc[::100]
    facet = sns.relplot(data, x= 'Time', y = property, kind =  'line',
            hue = 'Simulation', size = 'Simulation', col = 'With Thirst')
    return facet

def scat_plot(data, y, x = 'Population'):
    data = data.loc[data['Time'] % 200 == 0]
    facet = sns.relplot(data, x= x, y = y, kind =  'scatter',
            hue = 'Simulation', size = 'Simulation', col = 'With Thirst')
    return facet

if __name__== "__main__":
    data = load_simulation()
    data['Speed to Acc Ratio'] = data['Speed']/ data['Acceleration']
    data = data.loc[-data['With Thirst']]
    data = data.loc[data['Simulation'] <= 2]
    sns.relplot(data, x= 'Time', y = 'Eyesight', hue = 'Run', row = 'Simulation', kind = 'line')
    plt.show()
