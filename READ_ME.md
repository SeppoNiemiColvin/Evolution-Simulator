This document highlights how to use these files and the different roles the classes play and structural patterns across them. Rather than focus on describing every property and method of the classes, this document is focused on providing a high level overview so that way you better idea of where to look for the details. Documentation for specific properties/methods are usually contained in the corresponding headers.

[TOC]

# Solving Particular Problems

## Setting up a Simulation

To initialize a [Simulation](#Simulation) you need to know:

1. A starting [genotype](#Genotype-1) including a [ForceManager genotype](#Genotype-3) by building one up or using a [DefaultBrainGene](#DefaultBrainGenes) function.

   A starting Genotype should be set with reasonable values so that the creatures can survive long enough to evolve. There may be some parameters that are not currently set to evolve, but can be adjusted in the genotype such as the size of the creature's reserve caps that may make it easier or harder for the creature population to establish themselves. Note when messing with reserve caps and starting amounts that you need to make sure that the creatures have calories starting below 60% to so that they cannot immediately mate and population does not spiral out of control. 

2. the size of the region you plan to work with, which is easy enough to do by choosing a side length, and making a square. 

3. Next, you need to know how many bushes you are planning to use, which you can decide based on either your desired carrying capacity or your preferred density.

4. The total amount of water you want. You can decide this based on a fraction of the area you want covered in ponds. 

5. how many ponds to split that water between. Note to make sure creatures don't clip right over the ponds, ponds are required to have a width and height of greater than 2 times the creatures speed, so the simulation enforces a lower bound of \(4.5 * speed^2 \)  Bushes are also placed at a distance of 70% the creature's eyesight from any pond to make movement necessary for long term survival. As such, more ponds results in less area for bushes even if the water area remains constant.

6. A starting population size. Should be robust enough that the creatures can establish themselves and find a mate without completely collapsing. I usually go with 100 males and 100 females. The simulation initializes this simply using the value of 100, but note that it produces 200 individuals, though it is possible to create a simulation with only one gender to prevent reproduction.



The following code implements the heuristics above

```c++
#include "Body.h"
#include "Creature.h"
#include "ForceManager.h"
#include "Simulation.h"

Creature::Genotype genes{DefaultBrainGenes::gooblet()};
//this code reflects that you can shift the starting genes if you would like but is not necessary
genes.body_genes.reserve_caps[Body::Need::calories] = 200.0;

const int starting_population_size{ 100 }; // note that you will this amount in both male and female creature.

const double side_length{ 1e4 }; //your side length here
const double area{ side_length * side_length};

const double area_per_bush{2.2e5}
const int bush_number{static_cast<int>(area/area_per_bush)}; //if you have a carrying capacity in mind consider changing the bush number directlly

const double water_area_denominator{20.0}
const double water_area{area / water_area_denominator};

const double pond_size_constraint{ 5.0 }; //must be at least 4.5, the larger this is the larger the ponds will be. Too large though and you won't have any ponds
const double speed{genes.body_genes.max_speed};
const double average_pond_size{pond_size_constraint * speed * speed};
const int pond_number{max(1,static_cast<int>(water_area/average_pond_size))};

Simulation sim{0.0, side_length, 0.0, side_length,
               genes, starting_population_size,
              bush_number,
              water_area, pond_number};
```



## Running and Recording a Simulation

Once you have a [simulation](#Simulation), you will likely want to let it run and see what happens over time. To make this easier, the Simulation class comes with a method run_and_record, which will take in a *file_name* (without extension), and make a csv file recording the population size and average genome every *sample_frequency* ticks for *bound* ticks. Every 10 times the *sample_frequency* it will also print a short summary of the current genome.

If you would like to record extra information or otherwise want more finetuned control, writing your own c code to run and record a simulation is not too difficult. The [PopulationGenome](#PopulationGenome) class in particular is designed around collecting data about the average Genotype present and printing that information to a file. If you have the Genotype used to initialize the simulation on hand you can use that to make sure the PopulationGenome has the right structure. Population Genome also comes with a member function to make sure your csv file has the correct header.

Additionally, any member function of Creature that takes no inputs and returns a double can be averaged over the population using member functions of the Simulation class, as well as information about the total amount of calories and water across the population. 



```c++
void record_simulation(Simulation& sim, Creature::Genotype& genes){
    //file set up
    std::string FILE_NAME{ "test.csv" };//put your file name here
    std::ofstream output_file{ FILE_NAME };
    if(!output_file){
        std::cout << "Unable to open output file\n";
        return
    }
    
    //make a header for our file
    output_file << sim.get_genome_header() << ','; 
    output_file << ",Time" <<",Biomass" 
        << ",Speed" << '\n'; //additional columns to track, adjust as needed
   
    //run our simulation
    const int BOUND{ 5000 }; 
    for(int time{}; time < BOUND; ++time){
        sim.update();
        if(sim.population_size() == 0){
            //if the population has crashed no need to continue
            //and no need to print to the file
            break;
        }
        //printing! Note this is in the same order as the header
        output_file << sim.get_genome() << ','<< time << ',' <<
            sim.total_biomass() << ',' << 
            sim.average_property(&Creature::get_current_speed()) <<'\n';
    }
}
```



## Testing Movement

In the process of adding features to the Creature class one will likely want to test what happens with only a single creature trying to survive and to visualize its journey to see if it makes sense. Eventually I would like to graphically represent the simulation using raylib, c++ graphics library. However, at this point, one can record the information from a simulation to a handful of csv files, that are then processed in python. Examples of this work are found in [Movement Records](#Movement Records).

To aid in these sorts of tests the [Simulation](#Simulation) class comes with the following member functions:

- record_population_positions
- record_food
- record_water

Each takes in a desired output file and the first also takes in an integer to represent the current time tick.

Note that record_food and record_water only need to be run once at the start of a simulation, while record_population_positions should be run at every time step.

This functionality has been bundled together in the function wander under the header [testingsuite.h](testingsuite.h). This function takes in a string giving the prefix for the files to be produced, a bound for how long to run this simulation and a boolean verbose, which determines how much is printed to the console. It then creates a 1000 by 1000 simulation with a single creature in it, 25 bushes and 3 ponds. It then records where the food and water were and, it then runs for bound time_steps recording the position of the creature at teach step. 

The file [displayMovement.py](displayMovement.py) then has some useful functions to help display this information visually. This is written in python to make use of the matplotlib library. To produce a scatterplot of the positions with hue representing time, use the function display_movement, whereas movement animation creates a gif to show movement over time. These functions take in pandas dataframes to extract the information from the csv. Both visualizations display the water and food to make 

```python
import pandas as pd
import displayMovementTest as display

prefix = "MovementRecords/test" #this is the same as the string we used in wander
creatures : pd.DataFrame = pd.read_csv(prefix + "_position.csv")
food : pd.DataFrame = pd.read_csv(prefix + "_food.csv")
water : pd.DataFrame = pd.read_csv(prefix + "_water.csv")
fig, ax = display.display_movement(creatures, water, food) 
fig.savefig(prefix + "scatter_full.png")
fig, ax = display.movement_animation(creatures, water, food)
fig.savefig(prefix + "animation_full.gif")
```



# Report Folders

Most folders that were not automatically generated by the build hold the files for reports. In these reports I perform an experiment using the simulation and provide a brief write up of the results. These folders contain a markdown document describing the experiment and the results, image files for any figures used in said reports, and any csv files used in said reports.

## Initial Simulation Report

Reports on the first simulations I ran when I was only tracking calories and not water. Compared the population, average speed, average acceleration, and eyesight over time for three different densities of food. At this point creatures did not need to slow down in order to eat.

## Movement Records

These provided visual representations of the movement of an individual creature that I used while redoing the behavioral architecture. This moved from creatures beelining to a goal to having forces act on the creature based on the creature's desires.  Displays the movement using animated gifs as well as scatterplots of the creature's position where time is depicted via hue. 

## Behavior Exploration

Explores what the genome looks like after evolving for 7000 ticks after moving from goal based to movement to forces illustrating desires. 

# Classes and Files

## TestingSuite

A file that contains some functions to test the functionality of various components as well as functions to run different types of experiments I might ran during troubleshooting.  These are often the functions I was calling in the main file, but were separated out to reduce clutter.

## Flags

This file some enumerations that the various classes can use to communicate about the various parts of the environment a creature may want to see or interact with.  

### Ecology 

Ecology flags represent specific components of the ecology (a bush for a food source, a pond for a water source,  a creature of a specifric species, or specially a mate of the same species, ect.) Depending on the complexity of the simulation these may roughly correspond to classes or specific configurations of classes.

### MetaEcology

MetaEcology flags signal what relationship the creature has to that specific component. For example, food is a MetaEcology flag, which for a herbivore might be associated with a bush, but for a predator might be associated to a gooblet. They particularly help the creature know how the object should relate to its internal needs. For example, if a creature has a lot of calories already, it will not select or keep a goal around things it considers food. 

## Simulation

A class that manages a single simulation of an environment. Provides methods that allow one to move the simulation forward, query information about its current state, and record information to files. It is designed to so that it can handle the random generation of starting positions for the various components of the environment. In moving the simulation forward by a tick, the simulation handles querying food and water resources for the creatures, and ordering the operations.

## Creature

This class describes a creature that we want to evolve. Various aspects have been condensed into different components to managed different aspects of the  creature such as the body that manages physical needs and movement and the force manager that operates as the creature's brain, transforming observations into a desired direction of movement. These are discussed further below.

Some discussion here is relevant not only for the main creature class but are philosophies/names carried across the various component classes as well.  These are marked with an asterisk. These will not be repeated for each component class.

The creature class has static member [NeedVectors](#Need) that give (as percentages of the body's total reserves) that creature's must surpass in order to mate and another set of thresholds they will use in deciding whether to pursue a [goal](#Goal).

Creatures also have the capability to track how long they have been alive and how long since they last mated. These do not currently satisfy a mechanical function, but have been useful during debugging to tell if creatures are dying/doing other things early in life or later in life.

### Internal Classes

#### Goal

Holds an observation of a survival resource (see below) that a creature may want to ineract with along with a corresponding MetaEcology flag. While movement is no longer designed around beelining toward goals, tracking the nearest object of interest allows us to distinguish between the cases where a creature has no desirable objects nearby, is already close to a useful survival resource, or is torn between multiple different survival resources. 

#### Genotype*

One design philosophy carried across these components is that each class making up the creature (including the creature class itself) has an internal struct labeled Genotype. These are usually named the same thing to make their role clear, and are distinguished by which class they are inside. The one exception is that for MovementAnalyzers, which are polymorphic, we call overarching abstract class a MetaGenotype, and give the concrete classes an additional descriptor (such as LinearGenotype).

Genotypes hold various values that in the creature's life will be constant, but which may  potentially (but not necessarily) mutate and  evolve over generations. Genotypes have a roughly parallel structure to their components as the classes they are inside. Creatures are initialized using a genotype and a starting position. They often come with default initial values (or have related functions to provide different reasonable defaults). As such, if one wants to test the effects of different genes one can create a Genotype, change the specific values one needs without having to specify the rest every single time. If one wants to change which values evolve, the corresponding component will have a reproduce function that can be modified.

They can be added together or divided by a value (in order to more easily allow for averaging between parents or across a population). Genotypes are designed with the flexibility to model different types of creatures, and arithmetic done between different types may raise an exception due to one having genes the other does not or those genes not being arranged in the same way.

Genotypes come with a static function to produce a genotype that functions as the additive identity (with inputs to establish the overall structure and avoid exceptions) and a method to reset the genotype to the additive identity for the current structure.

Since we will often be tracking these genotypes over time, genotypes also provide methods to record the genotype to a csv file or the console, including overiding the insertion operator for ostreams and ofstreams. They also provide a function that produces a header describing what each entry of the csv file is.

Some genotypes also provide a method to test the validity of the genotype if certain values would not make sense (for example, speed, acceleration, and eyesight should be positive).

### Enumerations

#### Gene*

These values are used to mark genes that have some requirement that could be initialized incorrectly, such as values that should be positive. This allows for clearer communication if an exception is raised.

### Component Classes with Genotypes

The following classes describe components of creatures and follow the philosophy described above.

- [Body](#Body): handles physical needs and movement (together because moving may use up needs)
- [ForceManager](#ForceManager): acts as the brain, taking in all the objects in the environment and produces an overall desired accelration.
  - [MovementAnalyzer](#MovementAnalyzer): a unit of movement analysis. Takes in a single object in the environment and computes a corresponding. This is an abstract class that will encompass different types of calculations one could do based on objects in the environment.

## Body

Handles the creature's physical needs and physical capabilities (such as movement). These are included together so that it is easy to have a physical action incur a cost on the physical needs both directly (taking the action directly consumes said resources) or indirectly (by affecting the overall metabolism).

### Internal Classes

#### Status

A struct that bundles together information about how the body is physically doing right now, including the felt needs (current reserves over reserve caps) and a constant reference to the velocity. The information is then sent to the [ForceManager](#ForceManager) to affect the creature's priorities.

#### Genotype

See [Creature Genotype](#Genotype-1).

Note: Not all of these genes are currently set to evolve. For example, ignored_needs is designed to allow one to turn off tracking for a need at the start of a simulation. Meanwhile reserve_caps and starting_amounts may eventually evolve (to model k vs r selection), but currently doing so would not be meaningful and keeping those values fixed helps provide more evolutionary pressure on the other genes.

### Enumerations

#### Need

The bodily needs the creature needs to mange in order to stay alive. Currently only contains calories and water.  I may eventually add things like alertness or structural integrity. A bunch of information will be tracked as NeedVectors (mathematical vectors with as many components as needs) and including this enum makes accessing the information from NeedVectors more descriptive and means that the size of NeedVectors will automatically adjust if new needs are added.

Because these are so fundamental, I may eventually move these to the Flags file or their own file to make them easier to reference without needing to reference the entire Body class.

#### Gene 

See [Creature Gene](#Gene-1).

This has more genes that need to be strictly positive. Genes that are stored as NeedVectors only have their overall name rather than the specific need (which )

## ForceManager

 This acts as the brain, taking in all the objects in the environment and produces an overall desired acceleration. For each Ecology flag that the creature can respond to, the ForceManager has a std::vector of pointers MovementAnalyzers that will process inputs of that type. The ForceManager also stores what type of relationship (a [MetaEcology](#MetaEcology) flag) the creature has to each potential input ([Ecology](#Ecology) flag).

### Internal Classes

#### Genotype

See [Creature Genotype](#Genotype-1)

Note: different data structures are used between the Genotype and the ForceManager itself. In particular, the genotype uses a dictionary (std::unordered_map) with keys Ecology flags to provide flexibility while building up a genome, while the brain itself is an std::vector with the corresponding flags stored as a separate std::vector because this structure will not change while the creature is alive.

The Genotype also comes with a member functions to build up the genotype in stages, as otherwise the complexity, number of genes could get overwelming. One member function adds on a specific MovementAnalyzer Genotype with a specific flag, while another that takes an Ecology flag and MetaEcology flag and adds some reasonable genes to the genotype for that relationship.

### Related Namespaces

#### DefaultBrainGenes

This name space provides some functions that return default values for ForceManager Genotypes by building up the corresponding relationships.

## MovementAnalyzer

A specific unit of analysis for the [ForceManager](#ForceManager). It is itself an abstract class to encapsulate different types of  calculations one could do.

Each MovementAnalyzer has a member function consolidate that takes in the creatures Status (see Body Status) and then condenses that down to a values it will apply to future applications. This helps weight the creature's desire for different objects in their environment by the needs that they need fulfilled, so that they are more drown to food when hungry, ect. Most of a MovementAnalyzer's genes will describe parameters in this process.

Movement analyzers then have a separate member function calculate_acceleration that takes in information about a particular object in the environment and calculate the force that that object would induce.

### Enumerations

#### AnalyzerType

Tracks the polymorphism between subclasses of MovementAnalyzer. 

#### GeneStructure

Tracks polymorphism in how genes are structured. Currently only has a linear option but  could include more complicated responses to their felt needs or include more information needed to calculate a reaction to their environment.

### Internal Classes

#### MetaGenotype

An abstract class for different genotypes that can be used to make a MovementAnalyzer

#### LinearGenotype

A MovementAnalyzer genotype that includes enough information to condense down the creature's felt needs using a linear function (so slopes for each need and an offset) and an exponent to apply on the distance to the object. This exponent will often be negative in order to prioritize objects that are closer.

#### Observation

This is the input to calculate_acceleration, which is a pointer to a [SurvivalResource](#SurvivalResource), an intensity that could be pre_calculated to prioritize one resource over another (for example food sources with more food), and precalculated displacement from creature and distance.

### Subclasses

#### DesireAnalyzer

Calculates a force pulling the creature towards resources that the creature wants (or pushing away from resources the creature does not want)

#### FrictionAnalyzer

Applies friction slowing the creature down as it approaches things it wants.

## PopulationGenome

Helps consolidate creature genotypes across a whole population for averaging. Also tracks population size and also provides a place to count genes that can't be so easily averaged using a mean (for example genes that are categorical instead of numerical can instead be tallied). To build up a population summary, you can increment a PopulationGenome by another PopulationGenome, a [Creature](#Creature), or a [Creature Genotype](#Genotype-1). Incrimenting by a creature is equivalent to incrementing by said creature's genotype and incrimenting by a PopulationGenome is equivalent to incrimenting by all the creatures used to produce said genome.

## ReproHelpers

A file containing some helper functions used in reproduction. There is a function for essentially flipping a coin, a function for averaging two values then applying a mutation that scales with those values (does assume the inputs are positive), and a function for averaging two values then applying a mutation that does not scale with the inputs. If the inputs are known to be positive prefer the first function over the second.

## SurvivalResource

An abstract class for any sort of object that a creature may want to interact with in their environment. In particular,  a lot of manipulation and movement will not require knowing the particular type of object in question, and thus can be done together. In particular survival resources, must provide a way to locate them via get_position(), and Survival Resource then provides member functions (which can be overridden) for finding the nearest point to another value and getting the distance to that resource. Overriding these other functions can be useful if the SurvivalResource in question is modeled on a [Region](#Region) rather than a point.

SurvivalResources must also provide a way to tell if they are available for use and a member function to use them. SurvivalResources also have an update member function to simulate the effects of the passage of time.

### Subclasses

#### FoodSource

A food source represents a bush with a handful of fruit (or other source of calories). Currently using a food source consumes one fruit and replenishes the creature's calorie reserves completely, though future iterations might assign fruits a nutritional value. Fruits grow back after a fixed number of ticks via the update function.

#### WaterSource

Water sources are currently modeled on a rectangle that can supply water. Using the water source completely replenishes the creature's water reserves, and does not affect the pond. As such, it is essentially a wrapper for a  [Rectangle](#Rectangle), but it could in the future be adjusted to shrink with use or accomodate other shapes.

#### [Creature](#Creature)

When interpreted as a SurvivalResource, availability and use are framed around mating. The update function runs the creature's metabolism.

## Region

A class to dictate a region  of space and how to interact with it. Should be able to query what point is nearest to a given other point, pull or push regions in and out of the region, generate a random point inside the region, produce its area, and test intersections.

### subclasses

#### Rectangle

A basic rectangle parallel to the axes.

#### HalfPlane

The area on one side of a line in 2D space. Not currently used in the simulations.

## Containers for SurvivalResources

In order to make available resources easier to find and leverage the fact that creatures have a limited eyesight, we provide 2 different custom containers to manage said resources. In particular, the containers each have a main subcontainer that owns the resources. The second part of the container splits up the map into a discrete grid with blocks as big as the maximum eyesight of a given creature. There is then an unordered_map that has as keys integers corresponding to blocks in that gird and values with a container for pointers to the available resources within that block. As such, finding the available resources within eyesight does not require looking through all of the resources each time.

The exact containers used for ownership and for the containers at each block depend on the nature of the resource.

### ResourceFinder

Designed to find resources that remain throughout the simulation at a fixed location. The owning container is a std::vector while the block containers are sets

### ResourceSet

Similar to ResourceFinder, but geared more towards creatures, particularly because they can be added or removed mid-simulation and because them moving around means that the data structure allowing look up by location needs to be regularly wiped anyways and thus one does not get benefit from using a set over a std::vector there. As such, the owning container is a std::unorderd_set of pointers (as new creatures are allocated to the heap), while the container at each block is a std::vector.

### Related Namespace: hash2d

A namespace containing functions used to make Vector2d objects hashable. It first makes them discrete by dividing the 2d plane into blocks. The header also includes a type alias CoordPrehash for the intermediate type.