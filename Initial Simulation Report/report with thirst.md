# Update on Initial Simulation with Thirst

### Summary

As an update on the initial report, I show that adding a thirst mechanic does indeed increase the pace of evolution for speed and eyesight. For eyesight in particular, the thirst mechanic made the direction of selection more consistent over time. However, the thirst mechanic was enough to counteract the slower pace of evolution in low food density environments, and it did not significantly impact overall population numbers. In the set up section, I also note that one should be careful how one initializes new creatures as a part of population control.

### Set Up

The simulations followed a similar set up as in the initial report, so we will focus specifically on the differences that arise from adding thirst and water.

First rectangular ponds covered 1/15 of the map, with each having an average area of 1000 square units, and a minimal width/height of 20 units. The ponds are stored in a similar data structure to the food sources to easily be found near the creatures. At this point, ponds do not currently change size or deplete when creatures drink from them.  The ponds also will push out creatures that end inside them, though it does not proactively prevent creatures from attempting to move inside them. This is a part of the reason to have a minimal width and height is to prevent creatures from skipping over them entirely, at least at the start of the simulation.

Food sources were placed next with the number based on the desired density/food diameter and the map area after the pond area have been removed. They also were not placed within 70 units of a pond to ensure that some travel will need to happen to stay alive. This was the key feature I wanted to test after the initial report.

The creatures now track both how many calories they currently have and how much water they have. Both have a threshold of 60% where less than that in either category means the creature cannot mate. To track the creature's current state, the creature first checks if the creature is already dead, applies metabolism, which lowers the reserves of both calories and water. It then checks if passed the threshold for water then calories, so if a creature is below the threshold on both water and food it will focus on water.

When initializing a new creature, I needed to make sure at least one of food or water was below the threshold to prevent immediate mating and force selective pressure to work right away. I also had a preference to have the other reserve start full so that way they have time to fulfil the first need and the second need before dying. I initially used water as the controlling need, having creatures start with 60% water and 100% calories.

However, I discovered that it was very important to have food be the initial controlling need because food was also the resource that would provide a population cap. With this set up creatures drinking water doesn't prevent another creature from drinking water, but a creature eating food can prevent another creature from surviving. By starting with calories at 100%, creatures could create calories by reproducing, and populations ballooned significantly, reaching up to 20,000 instead of the 6,000 from earlier runs.

Note that simulations 1 and 2 correspond to the same map size and food density as with the initial report.

### Population with Thirst Mechanic

While one would suspect that the addition of the thirst mechanic which creates new ways to creatures to die, we also started these simulations with a larger base population (since experiencing an early population crash does not provide much useful information), so we cannot make conclusive statements about how the addition of thirst affected population numbers. As seen in the graph below, the low food density simulation with a thirst mechanic initially lagged behind the low density simulation without a thirst mechanic, but even then the overall population ended up higher.

It may be that the higher selection pressure with the thirst mechanic increased the overall pace of evolution for speed, acceleration, and eyesight, which then helped the creatures more efficiently make use of the food resources. Additionally, the fact that food currently replenishes creature's to full calorie reserves, creatures now gain more benefit from eating at low calorie reserves. Thus, being forced to attend to other tasks, potentially lets the creatures make better use of the food resources available.

![](C:\Users\mhans\source\repos\Evolution Simulator\Initial Simulation Report\population over time with thirst.png)

We could probably make more decisive statements by running more simulations for comparison, but there are other questions we would prefer to focus on for now.

### Speed with Thirst Mechanic

Unsurprisingly speed evolved faster in simulations that included a thirst dynamic than those that did not.  This was not enough to overcome the slower overall pace of evolution in low density environments, but once one accounted for population size, low food density with thirst significantly outperformed the rest.

I discovered upon attempting to make the graphs that I had adjusted the initial acceleration value, and while i could rerun the simulations with this corrected, I do not expect a significantly different story there, and my main focus is on the eyesight property anyways.

![](C:\Users\mhans\source\repos\Evolution Simulator\Initial Simulation Report\speed over time wtih thirst.png)

![](C:\Users\mhans\source\repos\Evolution Simulator\Initial Simulation Report\Speed vs Population with Thirst.png)

### Eyesight with Thirst Mechanic

Adding thirst created more pressure for eyesight in both high and low food density simulations but high density with thirst still lags behind low density without thirst. Still, for high food density simulations, without thirst it took a while for any trend to emerge in the average eyesight and even then the trend was slight, whereas the trend immediately arose with the thirst mechanic. This showed that the thirst mechanic did force creatures to move around and be on the look  out for resources even in an abundant environment.

![](C:\Users\mhans\source\repos\Evolution Simulator\Initial Simulation Report\eyesight over time with thirst.png)

### 