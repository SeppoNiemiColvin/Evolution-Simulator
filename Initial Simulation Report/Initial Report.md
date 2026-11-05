# Initial Simulations

### Summary

Inspired by Icoso's evolution simulator, I ran my own evolution simulation with varying food densities. In my simulations, larger environments with lower food density took longer to reach their carrying capacity, and said carrying capacity was lower, even when the overall amount of food was the same. This resulted in a slower evolutionary pace overall for movement based properties (speed and acceleration) in lower density environments, something which was not as observed in Icosos' experiment. But when one compared simulations based on population size rather than time stamp, the lower density environments  still produced faster creatures. Lower density environments exerted even stronger evolutionary pressure on eyesight to the point where comparing by population size was not necessary, and in fact eyesight in high density environments developed in a chaotic fashion.

### Introduction and Context

This report is a part of a larger project inspired by a series of videos posted by Icoso on youtube about creating an evolution simulator.  In those videos Icoso created creatures named gooblets that needed to eat and drink in order to survive and if satiated could reproduce. These videos came with animations of the creatures wandering around the map between food and water sources and graphs of how properties such as eyesight and speed evolved over time. Throughout the videos various features were added including variations in the environment, and explored how changes in the environment changed the speed of evolution.

That said, while you could see the effects of different environments exerting different selective pressures, I was curious if you could see a form of niche partitioning or speciation in these simulations. For example, it would be interesting if the environment affects not just the intensity but also the direction of evolution, to the point where one gets a bimodal distribution.

However, that is a long term goal to build toward, and first requires a functioning evolution simulator. As such, we are primarily verifying Icoso's results here. My project is written in C++ and doesn't feature any animations, but hopefully it can then support more creatures in a given simulation to make the simulations more robust and less prone to genetic drift. The simulations in this report are similar to simulations done at 5:17 in Icoso's Evolution Simulator video, albeit without thirst mechanics. 

### Set Up - Environment

Each simulation had a map, denoting a region the animals were allowed to wander around, and if they wandered off the map it would pull them back in and adjust velocity accordingly. This map also held food sources for the creatures to find and interact with.

Each simulation changed the density of the food on the map, with the map_size determined the size of the world by making the bounds [-map_size, map_size] in both directions. Each time, 1414 food sources were randomly assigned across the map. This number was not chosen directly but based on the impact of exploring a food_radius variable that would determine the density and average distance between food sources.

In the first simulation the map size was 2000, in the second simulation it was 4000, and in the third simulation it was 5000. An attempt was done with 6000, but that led to population collapse.

### Set Up - Creatures

Creatures had to manage their hunger over time and mate when sufficiently satiated. They they had three evolvable traits: eyesight, acceleration, and max velocity.  These are described in more detail below. At this point, I had not implemented any thirst mechanics and there was no direct penalty for acceleration, speed or eyesight, but acceleration was capped at half the max speed.

Behavior was controlled by a state that determined what the creature was looking for ( food if hungry, a mate if full or dead and in need of clean up) and a recorded goal (a pointer to the food/mate if found and a null pointer if not). If the creature did not have a goal or the goal had become unattainable, the simulation would see if there was an option within the creature's eyesight range and if so assign the goal.

While a creature had a goal it would shift its velocity (up to its acceleration) to point towards the goal and move in that direction. Once it got within it's reach (5 units), it would accomplish its goal (eat the food or reproduce). If it did not have a goal (because there were none of its desires within sight), it would continue moving in its current direction in search of more. If its speed was sufficiently low, this would be given a nudge by picking a random direction to accelerate in. 

The creatures started with a max speed of \(10 \frac{unit}{tick}\), acceleration of \(3 \frac{unit}{tick^2}\), eyesight of 100 units, and 20 starting individuals of each gender. 

### Analysis - Population

While overall each simulation had a similar carrying capacity due to the same amount of overall food available, simulations where the food was more spaced out were harder to live in and operated at a slower pace. This is reflected in the overall population numbers over time as shown in the figure below. Not only did it take longer for the populations in Simulations 2 and 3 to establish themselves but both 2 and 3 stabilized at lower carrying capacities than simulation 1. 

![](C:\Users\mhans\source\repos\Evolution Simulator\Initial Simulation Report\PopulationOverTime.png)

Perhaps over time as the population optimized itself to its environment, these would eventually converge. One can see especially for simulation 3 that even after growth slowed down, overall population numbers were still increasing over time, especially relative to simulations 1 and 2. 

This also has an impact on our analysis of other population properties, since evolution would act slower in simulations 2 and 3 than on simulation 1 (due to the fact that reproduction took place less often and competition was less fierce). As such, in subsequent sections we will not only plot the properties over time but also over population size to help account for this fact.

Analysis - Speed and Acceleration

 I initially predicted, partly based off of the simulation that inspired this, that speed and acceleration would improve faster in environments that are more spread out. That initially did not appear to be the case when plotting each over time.

![](C:\Users\mhans\source\repos\Evolution Simulator\Initial Simulation Report\SpeedOverTime.png)

![](C:\Users\mhans\source\repos\Evolution Simulator\Initial Simulation Report\AccelerationOverTime.png)

I wondered if the fact that I had not instituted a thirst mechanic that would force movement rather than camping on a known food source was to blame. However, this trend versed itself once I plotted each against population numbers to account for the slower evolutionary pace. Creatures in Simulation 3 had the highest speed and acceleration at similar population levels. As such, the lower food density exerted a higher selective pressure for fast creatures, it just also slowed the overall pace down so that the overall improvement was slower. 

![](C:\Users\mhans\source\repos\Evolution Simulator\Initial Simulation Report\SpeedvsPopulation.png)

![](C:\Users\mhans\source\repos\Evolution Simulator\Initial Simulation Report\AccelerationvsPopulation.png)

An initial dip in speed before steady improvement appears in all simulations but most prominently in Simulation 2. This is mysterious, and my best guess is that there is a  connection between speed and eyesight, discussed more in a subsequent section.

### Analysis - Eyesight

Plotting eyesight over time, one sees that while it took Simulation 3 a little longer to get going it improved more steadily than either other population, even catching up with and overtaking Simulation 2. Meanwhile Simulation 1 lagged significantly behind, fluctuating up and down for a while before eventually steadily but slowly increasing. This narrative is only exaggerated further when Eyesight is plotted against population size, where Simulation 3 overtakes Simulation 2 much sooner.

![](C:\Users\mhans\source\repos\Evolution Simulator\Initial Simulation Report\EyesightOverTime.png)

![](C:\Users\mhans\source\repos\Evolution Simulator\Initial Simulation Report\Eyesight and Population.png)

The quick improvements in Simulation 3 were to be expected, as in order to eat one must first find food, and while speed can help traverse more distance in the search, one isn't guaranteed to be headed in the correct direction. A lack of eyesight would also inhibit population growth by slowing down how fast it takes to find a mate. 

Another interesting point found in plotting Eyesight vs Population is that while Simulation 1 did not receive much growth in Eyesight at first it was only once the Population was established that eyesight would start fluctuating downward at all. This hints at some dynamic involving intraspecific competition where eyesight was not as helpful and maybe even detrimental. One idea I had was that eyesight could tempt a creature to move towards a goal that was soon to be unattainable thus dragging the creature away from searching for other more attainable goals. 

### Exploration - Eyesight at High Food Density

One immediate question one might have is why around time 5000 does the eyesight in Simulation 1start steadily increasing, after previously fluctuating. This does not correspond to a shift in population size or when Simulation 1 got close to carrying capacity, which happened closer to time 1000. Instead I hypothesize that it required the speed increasing high enough before increased eyesight would be valuable. As mentioned in the previous section, high eyesight could be less valuable in high competition environments where someone else might grab the resource first. This is then again counteracted once speed is enough to reach the destination fast enough.

To test this hypotheses, I ran 8 more simulations where the food density was kept fixed but the overall map size changed to see if they had similar plots when comparing speed and eyesight. The map_size variable (which note denotes half the width and height of the map), took values from 1000 to 2750 with a step size of 250. The changing map size value would affect the overall carrying capacity and how fast it takes to reach said capacity from the initial 20 individuals. However, these still should be big enough that the overall selective pressures are local and still mainly depend on the density of resources. 

Unfortunately, the plots were quite chaotic as shown below. A number of them do seem to be trending upward for some time at the end, but not all and there is not a consistent point.  If there is a stochastic differential equation linking eyesight to speed, it is not simple and has a lot of noise contributing to it. Some of this noise may be coming from the fact that the randomness to model mutation was higher on eyesight than the other properties, if just to scale with the overall bigger base value. This noise did not appear nearly so strongly in Simulations 2 and 3 where the selective pressures were higher. That said, the graphs still look too smooth to my noise to be just coming from Brownian noise.

![](C:\Users\mhans\source\repos\Evolution Simulator\Initial Simulation Report\speed high food density.png)

![](C:\Users\mhans\source\repos\Evolution Simulator\Initial Simulation Report\eyesight high food density.png)

### Future Work

The biggest drawback of this current simulation set up is that it does not contain a thirst mechanic. While this might initially seem redundant with having a hunger mechanic, by having a buffer zone between food and water sources, one can make movement in general more beneficial not just something that is hard coded into the behavior. Once intraspecific competition is high, it would actually make sense for a creature to pick a food source and stay there, mating with others beside them that also decided to stay by that food source.

I would also like to eventually program in mechanics that would allow for both r-selected and k-selected species to evolve or more generally allow for different strategies to evolve adapted to different niches and environments. Right now the pace of evolution differs between the environment but the direction of evolution (once sufficiently established) is the same. One way to do this would be to have food not take creatures back to full energy but add a fixed energy amount, and have total energy reserves be a variable that fluctuates. The threshold for feeling full scale along side the total reserve amount so that way having a lower reserve would also allow you to require less food before mating. Environments with clustered resources  might prefer higher energy reserves to trek the distance between food sources, where environments with evenly distributed resources might prefer smaller energy reserves to reproduce faster.

In order to implement such a strategy though we would also need to make the behavioral controls more sophisticated. In particular, deciding when to eat may get more complicated, if eating does not fully restore energy. If you simply set a threshold for energy below which eating happens and above which it does not, that would easily put an artificial cap on the maximum energy consumed.

### Citations

This project was heavily inspired by Icoso's [evolution simulation](https://www.youtube.com/watch?v=f7vH2Li9KOw&t=764s) on youtube.

The code for this project is found in the github repository that this report is in, and is based around the commit labed 'initial report completed'