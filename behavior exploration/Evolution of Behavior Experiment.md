#  Evolution of Behavior Experiment

In this experiment, my primary goal was to find decent starting values for my new parameters that control the creature's movement and generally see what directions the parameters tend to move. However, I realized that this might depend on things like the density of food and water on the map, so I included some variation in the density to see if those trends were affected by the density.  In particular, I do not want to overfit my understanding to a particular food and water density, especially if 

### Overview of Behavior Genes

#### The Goal and Context

The main 'choice' the creature has right now is at any given moment, what direction should it move. Its body gives an upper bound on the magnitude of its acceleration and velocity at any given moment, but it does not control the direction of said movement. The creature also has an eyesight that determines the how far the simulation will look to find resources, but not how they will react to said resources.

In my first iterations of the evolution simulator, I had the creature keep track of an internal state based on which of its needs was most pressing. Then if it saw a resource its desired acceleration would record that s its goal, point directly towards said resource with an adjustment based on the creature's velocity to apply some friction (since the creature would not be able to use the resource if it was moving too fast).  It would continue using this goal until

However, I realized that I will want more subtlety in the creature's behavior as the creature and its environment got more complex. For example, as This subtlety could potentially be modeled in the current system, by creating more layers to determining priority or actions, but I wanted a uniform framework to expand upon rather than a bunch of adhoc solutions. Solutions would also likely have some arbitrary element to determine what to do when there are multiple things we could prioritize. Instead, I wanted a system that could flexibly grow, as I added on new features. Turning potentially arbitrary decisions into evolvable traits means I do not have to wed myself to one particular option, and we have the potential for emergent behaviors.

#### Overall Structure

To create a flexible system, we have to first be specific about what problem we are trying to solve, as well as identify places I know I may want to tweak in the future. The results are summarized in Figure [link]

I want a behavior set up that could, based on some evolvable genes, take in an input from the environment and calculate an acceleration in response to that input. These inputs would then be added together to get an overall desired acceleration, which the body could then modulate into movement. In order to respond to different types of inputs, survival resources would be tagged with an enumeration to distinguish their type. This became the Ecology enumeration.

The method of calculating the desired calculation should also be polymorphic, with an abstract class being able to specialize into different types of calculations that can be strung together.  Right now, I want to include both a calculation for the creature's 'desire' that pulls the creature towards or away from a resource, and an effect of 'friction' because I require creature's to be sufficiently slow to eat, drink or mate. But, I have further ideas for potential inputs to help the creature not run into obstacles or avoid predators.  We will call the abstract class that performs one of the calculations a *MovementAnalyzer*, whereas we call the class that combines these calculations to get a final result a *ForceManager*. 

A MovementAnalyzer's calculation of acceleration uses its own struct for input called an *Observation*. An Observation contains a pointer to a Survival Resource but also some additional calculations. For example, the MovmentAnalyzer should not need to know what type of SurvivalResource they are interpreting, and even the ForceManager should treat different Ecology flags in analogous ways. But, we then need a place to store information calculated using more specific information, such as the fact that a bush with multiple fruit is more appealing than a bush with only one fruit. As such, there is an intensity member variable initialized at 1.0, which can be rescaled to encode a pre-computed scaling factor. There are also some calculations, such as distance from the creature, that most MovementAnalyzers will need and thus we can include a space to record them after calculating them once. Including this as a precalculated value also means that the ForceManager does not need to keep track of the creature's position.

Having the as its own struct allows me to adjust the exact list without needing to update every single instance of this member function being called.

Before calculating any desired accelerations, movement Analyzers also need to be initialized with information regarding the creature's overall status, such as their velocity, the fraction of their reserves they have filled. The creature's velocity will be needed for applying friction, and the fraction of their reserves will help inform whether they prefer bush resources because they are hungry, ponds, because they are thirsty, or mating because they are neither hungry nor thirsty. The Status struct is defined in the Body class and taken in by 

The input to the ForceAnalyzer will then be a data structure of Observations, which will then match the data structure used to hold the MovementAnalyzers (or rather pointers to them to allow polymorphism). For each type of input that the creature's will respond to, we can have an std::vector of pointers to MovementAnalyzers that the observation will be passed along. But, we still need a data structure, that holds different  such std::vectors for different types of input. When building up the genotype, I use a dictionary that has Ecology flags as keys and std::vectors as values, to allow new options to easily be inserted and removed. But, once the creature is alive, such flexibility is no longer needed, so I move from a dictionary to another std::vectors, where the flags for each  row are encoded seperately. Thus, the ForceManager will assume that the input is itself an std::vector of std::vector of observations, ordered so that the input types match up. I have given this the type alias of View.

[put figure, here]

#### Implementing Movement Analyzers

For both the DesireAnalyzers and FrictionAnalyzers, observations closer to the creature should have a bigger impact. That way creatures looking for a resource are more drawn toward the closest relevant resource, and friction only causes creatures to slow down once they are close. That said, for the DesireAnalyzer, we do not want the asymptote to be zero, since otherwise a creature searching for a resource will still feel its pull. As such, the DesireAnalyzer and the FrictionAnalyzer make use of the following functions of the distance to the , where \(r\) is an exponent capable of evolving, and \(a\) is a number calculated based on the creature's status and genes. In the genes themselves, \(r\) is encoded as a negative value. 
$$
D(x) = \left(\frac{a}{x^r} + 0.5\right), \quad x> 5.0 \\
F(x) = \frac{a}{x^r}, \quad x>5.0
$$
To capture desire, \(D(x)\) is then multiplied by a unit vector in the direction of the observation, while \(F(x)\) is multiplied by the negative of the creature's velocity.

To prevent division by zero, accidentally large outputs, or discontinuous results, both functions were modified within 5.0 units of 0. For \(D(x)\) , we linearly interpret between the two end points, whereas for \(F(x)\) we cap out the contribution of \(\frac{1}{x^r}\) at 10. As  such, for small values of \(x\), the creature will behave like a dampened spring, but for large values they will head toward the nearest option compatible with their needs.

The coefficient \(a\) is calculated by a linear combination of the genes, named using the following pattern for the coefficients

```
[Ecology]_[analyzerType]_[Need]_coefficient
```

and for the constant term we have

```
[Ecology]_[analyzerType]_offset
```



### Gene Trends

I included the mean values for each coefficient in a table in [All Genes](#All Genes), sorted by the difference from the starting value. However, because that is a lot of genes to track, the following subsections delve into more targeted questions, including:

- Whether the exponents in particular were set at reasonable values?
- Did coefficients for not directly relevant needs still experience evolutionary pressure?

- Are there are any behavioral genes that were dramatically affected by resource density?

#### Exponent Evolution

The exponents used in the analyzers were something that I had guessed at, but I did not have as much intuition around them as I did the need coefficients and offsets. I found that for the frictionAnalyzers, the initial value of -0.80 could have been more negative, but for the pond and goobletMate options, the change was slight (within 0.02), while the bush_friction_exponent moved more dramatically, to -.911. Similarly for the desireAnalyzers, the bush exporenents changed from -0.3 to -0.24, while the the pond and goobletMate exponents only changed to respectively -0.2853 and -0.2827.

My guess is that because bushes are fixed points, localizing the effects of friction and spreading out the desire mater more than when the survival resource is an area or itself moving.

#### Coefficients that Started at Zero

Most coefficients that started at zero moved less than 1 unit, which ultimately woudln't have much effect compared to the coefficients on the scale of 30 or 100. However, the coefficients of water for bushes ended at -5.5 and 5.9 for friction and desire. That these were for the same ecology flag and the same need, seems to indicate that something real is going here, but it is less clear to me as to what, especially considering they evolved in opposite directions. Given I explained the difference in bush evolution using the fact that they are fixed points compared to large areas or moving targets, that may also play a role here.

#### Correlation with Density

The following chart shows the correlation each gene had with the density. Line charts for all of the genes discussed are also provided below. All the evolving body genes had a stronger correlation than the behavioral genes. 

 =![](C:\Users\mhans\source\repos\Evolution Simulator\behavior exploration\gene correlation with map area.png)

Of the behavioral genes,  the offsets for both bush movement analyzers showed the strongest correlation. Given previous discussion, bushes having an outsized impact shouldn't be surprising.  That said, the bush_desire_offset is decreasing in size, while the friction offset is increasing in size. For bush_desire_offset, may reflect that for lower food densities, creature's have more time to apply the acceleration toward the food and there is less chance for the food to disappear before it got there. Similarly, the bush_friction_exponent also showed high correlation, with higher food densities having a more negative exponent, localizing the effects further.

![](C:\Users\mhans\source\repos\Evolution Simulator\behavior exploration\bush desire offset versus map area.png)

![](C:\Users\mhans\source\repos\Evolution Simulator\behavior exploration\bush friction offset versus map area.png)

![](C:\Users\mhans\source\repos\Evolution Simulator\behavior exploration\bush friction exponent versus map area.png)

#### All Behavioral Genes

| Gene                                      | Starting Value | Mean value after evolution | Difference |
| ----------------------------------------- | -------------- | -------------------------- | ---------- |
| bush_friction_offset                      | 40.0           | 28.74                      | -11.25     |
| bush_friction_water_coefficient           | 0.0            | -5.52                      | -5.52      |
| goobletMate_friction_offset               | -32            | -34.74                     | -2.74      |
| goobletMate_friction_calories_coefficient | 40.0           | 37.61                      | -2.39      |
| goobletMate_friction_water_coefficient    | 40.0           | 37.73                      | -2.26      |
| pond_friction_offset                      | 40.0           | 38.33                      | -1.67      |
| pond_friction_calories_coefficient        | 0.0            | -0.37                      | -0.37      |
| pond_friction_exponent                    | -0.8           | -0.911                     | -.111      |
| bush_friction_exponent                    | -0.8           | -0.82                      | -0.021     |
| goobletMate_friction_exponent             | -0.8           | -0.81                      | -0.013     |
| pond_desire_exponent                      | -0.3           | -0.285                     | 0.015      |
| goobletMate_desire_exponent               | -0.3           | -0.283                     | 0.017      |
| bush_deire_exponent                       | -0.3           | -0.250                     | 0.050      |
| bush_desire_calories_coefficient          | -120           | -119.45                    | 0.54       |
| pond_friction_water_coefficient           | -48            | -47.23                     | 0.76       |
| goobletMate_desire_calories_coeffficient  | 100            | 100.795                    | 0.795      |
| pond_desire_calories_coefficient          | 0              | 0.835                      | 0.84       |
| goobletMate_desire_water_coefficient      | 100            | 101.46                     | 1.46       |
| pond_desire_water_coefficient             | -120           | -118.34                    | 1.66       |
| goobletMate_desire_offset                 | -80            | -78.32                     | 1.68       |
| pond_desire_offset                        | 100            | 102.75                     | 2.75       |
| bush_friction_calories_coefficient-48     | -48            | -43.36                     | 4.64       |
| bush_desire_water_coefficient             | 0.0            | 5.96                       | 5.96       |
| bush_desire_offset                        | 100            | 109.40                     | 9.4        |



### Future Work

#### Graphics

I would like to animate the simulations, so that way this work has a more visually interesting output for people to interact with. I may have to start a new repository for this to get raylib appropriately set up. I already did mock up this movement in Godot, but I would like the animation to be able to use and build upon my simulation more directly.

#### Performance

These calculations create more subtle behavior but are more computationally intensive. As such, I do plan on going through to find potential performance upgrades in my implementation. This should be done in concert with the graphics implementation so I can see the effects of my changes and the trade offs. I also would likely use this as an opportunity to learn how to do multithreading. 

#### Environment

I would like to make it so that fruit only provide a fixed amount of calories rather than fully replenishing the creature's calories. I also would like to make the environment seasonably variable so that there are more plentiful seasons such as spring and harsher seasons such as winter. This could be done with things like fluctuating temperature affecting creature's metabolism or having bushes or ponds that only appear during some seasons but not others. Having temperature cause problems if too hot and too cold would be one way to get evolution not just in one direction. Not only would this be interesting to explore on its own but it would also make exploring creature development more interesting. It is known that r-selection is more common in highly variable environments, but even for k-selected species having a season that is more plentiful means there is a time when juvenile creatures won't automatically be outcompeted.

#### Development

I would like to explore scenarios that could lead to \(r\)-selection vs \(k\)-selection, and in particular, getting environments that could get the same starting base creatures to evolve in each direction. However, one thing that will lead to pressure for there to be lots of small creatures rather than larger creatures, is if it takes a while for larger creatures to grow up and thus mate. This will require implementing a developmental cycle where creatures start out weaker and unable to mate. 