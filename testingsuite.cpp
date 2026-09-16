#include "testingsuite.h"
#include "Simulation.h"
#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <algorithm>
#include <Eigen/Dense>
#include <cmath>
#include "Rectangle.h"
#include "ResourceFinder.h"
#include "WaterSource.h"
#include "Body.h"
#include "DesireAnalyzer.h"
#include "FrictionAnalyzer.h"

#include "EcologyFlags.h"
#include "ForceManager.h"
#include <iomanip>
#include <string_view>

using Eigen::Vector2d;

/*
* Sees how long a single creature can survive on its own up to some bound.
* I.e. if the creature survives until bound units, we'll stop and just return
* bound, but if the creature dies earlier we return the time when that happened.
*/
int test_survivability(int bound, double food_distance, double water_density) {
    Creature::Genotype genes{};
    const double speed{ genes.body_genes.max_speed };
    const double map_area{ speed * speed * 2500.0 };
    const double water_area{map_area * water_density };
    const int pond_number{ static_cast<int>(water_area / (7 * speed * speed))};
    const int food_number{ static_cast<int>((map_area - water_area) * 4.0 /
        (food_distance * food_distance * 3.14159)) };
    


	Simulation simulation{ 0.0, speed * 50.0, 0.0, speed* 50.0, genes, 1,food_number , water_area, pond_number, false};
	for (int time{}; time < bound; ++time) {
		simulation.update();

		if (simulation.population_size() == 0) {
			return time;
		}
	}

	return bound;
}

/*
* Runs a single creature wandering around an environment for 5 * bound timesteps 
* (it adjust speed and acceleration so that these are each 1/5 of a normal time  step)
* Note the function handles file extensions so no need to include a .csv in flie_name
* if verbose is false (which is the default) therew ill still be some printing but it will
* only be at the end and every 500 timesteps instead of every timestep
*/
void wander(const std::string& file_name, int bound, bool verbose) {
    constexpr int time_stretch{ 5 };
    Body::Genotype body_genes{ {100.0 * time_stretch, 100.0 * time_stretch},
        100.0/time_stretch, 50.0/(time_stretch * time_stretch)};
    Creature::Genotype genes{ DefaultBrainGenes::gooblet(), body_genes, 300.0 };

    std::string full_file_name{ "MovementRecords/" + file_name };
    std::ofstream position_file{ full_file_name + "_position.csv"};
    std::ofstream food_file{ full_file_name + "_food.csv" };
    std::ofstream water_file{ full_file_name + "_water.csv" };
    if (!position_file || !food_file || !water_file) {
        std::cerr << "Unable to open output file(s)\n";
        return;
    }

    constexpr int pond_number{ 3 };

    double water_area{ fmax(body_genes.max_speed * body_genes.max_speed * pond_number * 6, 5e4 )};
	Simulation simulation{ 0.0, 1e3, 0.0, 1e3, 
        genes, 1, 25,  water_area, pond_number,false };
    simulation.record_food(food_file);
    simulation.record_water(water_file);

    position_file << "Position_x,Position_y,Velocity_x, Velocity_y,Time\n";
	for (int time{}; time < bound * time_stretch; ++time) { 
        if (verbose || time % 500 == 0) {
            std::cout << std::endl << "Time: " << time << '\n';
        }
        if (verbose) {
            simulation.print_population();
            simulation.print_water();
        }
        simulation.record_population_positions(position_file, time);

        simulation.update();
        if (simulation.population_size() == 0) {
            break;
        }
	}
    simulation.print_food();
    simulation.print_water();
}

void small_pop(const std::string& file_name, int bound) {
    Creature::Genotype genes{DefaultBrainGenes::gooblet()};
    const double starting_speed{ genes.body_genes.max_speed };

    Simulation simulation{ 0.0, starting_speed * 10, 0.0, starting_speed * 10, genes, 2, 6, 15 * pow(starting_speed,2), 3};
    simulation.print_water();
    simulation.print_population();


    std::string full_file_name{ "MovementRecords/" + file_name };
    std::ofstream position_file{ full_file_name + "_position.csv" };
    std::ofstream food_file{ full_file_name + "_food.csv" };
    std::ofstream water_file{ full_file_name + "_water.csv" };
    simulation.record_food(food_file);
    simulation.record_water(water_file);

    for (int time{}; time < bound; ++time) {
        simulation.update();
        std::cout << "Time: " << time << '\n';
        std::cout << "Population: " << simulation.population_size() << '\n';
        simulation.record_population_positions(position_file, time);

        //print to console
        simulation.print_population();
        simulation.print_water();
        simulation.print_food();
        std::cout << '\n' << '\n';
        if (simulation.population_size() == 0) {
            return;
        }
    }
}

//note that sim1 was map_size 2e3 and food_diameter 30.0
void test_density(double map_size, double food_diameter, const std::string& file_path) {
    const double starting_speed{ 100.0 };
    const double water_area{ map_size * map_size / 20 };
    const int pond_number{ static_cast<int>(water_area / (5 * starting_speed * starting_speed) )};

	const double food_area{ food_diameter * food_diameter * 3.141592 };
	const int food_number{ static_cast<int>((map_size * map_size - water_area)/ food_area) };
    std::cout << "Available Food: " << food_number << '\n';
    std::cout << "Available Water: " << water_area << '\n';

    Creature::Genotype genes{ DefaultBrainGenes::gooblet()};
    genes.body_genes.max_speed = starting_speed;

    Simulation simulation{ -map_size, map_size, -map_size, map_size,
        genes, 100, food_number, water_area, pond_number };

    simulation.run_and_record(file_path, 15000);
}

bool test_regions() {
    constexpr int size{ 7 };

    const std::array<Rectangle, size> test_rects{
        Rectangle{0.0, 100.0, 50.0, 200.0},
        Rectangle{50.0, 300.0, 150.0, 250.0},
        Rectangle{60.0, 110.0, 0.0, 100.0 },
        Rectangle{-120.0, 20.0, 120.0, 230.0},
        Rectangle{-140.0, 10.0, 20.0, 75.0},
        Rectangle{-20.0, 400.0, 0.0, 300.0},
        Rectangle{300.0, 500.0, 240.0, 600.0}
    };
    constexpr std::array<double, size> areas{
        100.0 * 150.0,
        250.0 * 100.0,
        50.0 * 100.0,
        140.0 * 110.0,
        150.0 * 55.0,
        420.0 * 300.0,
        200.0 * 360.0
    };

    constexpr std::array<bool, size> intersections{
        true, true, true, true, true, true, false
    };

    const Vector2d start(-20.0, -10.0);
    const Vector2d end(110.0, 110.0);

    constexpr std::array<bool, size> line_intersect{
        true, false, true, false,false, true, false
    };
    for (int i{}; i < size; ++i) {
        if (test_rects[i].get_area() != areas[i]) {
            std::cout << "Area failed at " << i << '\n';
            return false;
        }
        if (test_rects[0].intersects(test_rects[i]) != intersections[i]) {
            std::cout << "Intersection failed at " << i << '\n';
            return false;
        }
        
        if (test_rects[i].line_intersects(start, end) != line_intersect[i]) {
            std::cout << "Line Intersection failed at " << i << '\n';
            return false;
        }
    }


    return true;
}

bool report_dousing_attempt(SurvivalResource* pond, Vector2d position, Vector2d* correct) {
    static int attempt{};
    attempt += 1;
    if (!pond && !correct) {
        return true;
    }

    if (!pond && correct) {

        std::cout << "Failed to find resource on attempt " << attempt << '\n';
        return false;
    }

    if (pond && !correct) {
        std::cout << "Found resource incorrectly on attempt " << attempt << '\n';
        return false;
    }
    const Vector2d nearest_point{ pond->get_nearest(position) };
    if ((nearest_point - *correct).norm() > 1e-7) {
        std::cout << "Found incorrect resource on attempt " << attempt << '\n';
        return false;
    }
    return true;
}

bool test_resource_finder() {
    std::vector<WaterSource> water{ WaterSource{10.0, 1000.0, 50.0, 1500.0},
        WaterSource{-1000, -50, -500, -20},
        WaterSource{30, 150, -300, 200},
        WaterSource{-300,-150, 40, 100}
    };
    bool answer{ true };
    ResourceFinder<WaterSource> water_manager{water, 130.0};

    Vector2d position{};
    WaterSource* pond{ water_manager.find_closest_to(Vector2d{}, 100.0)};
    Vector2d* correct = new Vector2d(10.0, 50.0);
    answer = answer || report_dousing_attempt(pond, position, correct);

    position = Vector2d(1010, 1510);
    pond = water_manager.find_closest_to(position, 100.0);
    delete correct;
    correct = new Vector2d(1000, 1500);
    answer = answer && report_dousing_attempt(pond, position, correct);

    pond = water_manager.find_closest_to(Vector2d{}, 20.0);
    delete correct;
    correct = nullptr;
    answer = answer && report_dousing_attempt(pond, position, correct);


    position = Vector2d(-40, -200);
    pond = water_manager.find_closest_to(position, 100.0);
    delete correct;
    correct = new Vector2d(-50, -200);
    answer = answer && report_dousing_attempt(pond, position, correct);
    
    delete correct;
    correct = nullptr;

    position = Vector2d{};
    std::vector<WaterSource*> ponds = water_manager.within_eyesight(position, 100.0);

    auto distanceToPoint{
        [position](const WaterSource* a, const WaterSource* b) {
            return a->get_distance(position) <= b->get_distance(position);
        }
    };

    std::sort(ponds.begin(), ponds.end(), distanceToPoint);

    constexpr int answer_size{ 3 };
    if (ponds.size() != answer_size) {
        answer = false;
        std::cout << "Found incorrect number of ponds\n";
    }
    else {
        std::array<int, answer_size> correct_ponds{ 2, 0, 1 };
        for (int i{}; i < answer_size; ++i) {
            if (correct_ponds[i] != ponds[i]->get_id()) {
                answer = false;
                std::cout << "Found incorrect pond at position " << i << '\n';
            }
        }
    }
    return answer;
}

bool test_creatures() {
    Body::Genotype body_genes{ {200.0, 200.0} };
    Creature::Genotype genes{ {},body_genes };
    bool answer{ true };
    Creature gooblet{ Vector2d{}, genes };

    //do some checks that the gooblet initialized correctly
    if (gooblet.is_available()) {
        std::cout << "Creatures should start unavailable\n";
        answer = false;
    }

    gooblet.eat();
    gooblet.drink();
    

    if (gooblet.time_till_unavailable() != 81) {
        std::cout << "Wrong time till unavailable\n";
        answer = false;
    }

    if (gooblet.time_till_dead() != 200) {
        std::cout << "Wrong time till death\n";
        answer = false;
    }
    gooblet.update();
    gooblet.update();
    if (gooblet.time_till_unavailable() != 79) {
        std::cout << "Wrong time till unavailable\n";
        answer = false;
    }

    //makes sure the creature lets go of a mate that is no longer available
    Creature mate{ Vector2d{10.0, 15.0}, genes };
    mate.eat();
    mate.drink();
    gooblet.set_goal(&mate, MetaEcology::mate);

    const int run_time{ mate.time_till_unavailable() };
    for (int time{}; time < run_time; ++time) {
        mate.update();
        gooblet.goal_in_reach();
    }

    if (mate.is_available()) {
        std::cout << "Mate should no longer be available\n";
        answer = false;
    }

    if (gooblet.goal_is_active()) {
        std::cout << "Gooblet should have released goal\n";
        answer = false;
    }
    return answer;
}

/*
* Designed to test the capabilitie of resource set and make sure it isn't
* going too wonky
*/
bool test_resource_set() {

    bool answer{ true };
    constexpr int size{ 5 };
    std::array<Vector2d, size> starting_positions{
        Vector2d(-5,-2), Vector2d(10.0, 10.0), Vector2d{100.0, 100.0},
        Vector2d( -200, 100), Vector2d( -300,-400)
    };

    //set up the genes to our predefined values
    Creature::Genotype genes{};
    genes.eyesight = 100.0;
    genes.body_genes.max_speed = 10.0;
    genes.body_genes.max_acceleration = 3.0;

    ResourceSet<Creature> test_population{};
    for (int i{}; i < size; ++i) {
        test_population.insert(new Creature{ starting_positions[i], genes});
    }

    //to make all the creatures available
    for (auto creature : test_population) {
        creature->drink();
        creature->eat();
    }
    test_population.update(100.0);

    //pull out how long we expect these creatures to be available and live
    int time_till_available{};
    int time_till_dead{};
    for (auto creature : test_population) {
        time_till_available = creature->time_till_unavailable();
        time_till_dead = creature->time_till_dead();
        break;
    }


    //make sure the population finds the correct closest creature in a few cases
    Creature* mate = test_population.find_closest_to(Vector2d{}, 100.0);
    Vector2d* correct = new Vector2d(-5, -2);

    answer = answer && report_dousing_attempt(mate, Vector2d{}, correct);

    mate = test_population.find_closest_to(Vector2d(150, 150), 100.0);
    delete correct;
    correct = new Vector2d(100, 100);
    answer = answer && report_dousing_attempt(mate, Vector2d(150, 150), correct);


    mate = test_population.find_closest_to(Vector2d(150, 150), 10.0);
    delete correct;
    correct = nullptr;
    answer = answer && report_dousing_attempt(mate, Vector2d(150, 150), correct);

    //makesure within_eyesight works correctly
    Vector2d position{};
    std::vector<Creature*> mates{ test_population.within_eyesight(position, 100.0) };

    constexpr int answer_size = 2;
    if (mates.size() != answer_size) {
        answer = false;
        std::cout << "Found wrong number of mates\n";
    }
    else {
        auto distanceToPoint{
            [position](const Creature* a, const Creature* b) {
                return a->get_distance(position) <= b->get_distance(position);
            }
        };
        std::sort(mates.begin(), mates.end(), distanceToPoint);
        std::array<Vector2d, answer_size> mate_positions{ Vector2d(-5,-2), Vector2d(10,10) };
        for (int i{}; i < answer_size; ++i) {
            if (mates[i]->get_position() != mate_positions[i]) {
                answer = false;
                std::cout << "Found wrong mate\n";
            }
        }
    }

    //make sure moving doesn't cause problems
    for (int i{}; i < time_till_available -5; ++i) {
        for (auto& creature : test_population) {
            creature->move();
        }
        test_population.update(100.0);
    }
    
    
    if (test_population.available_size() != 5) {
        answer = false;
        std::cout << "Incorrect number of available creatures\n";
    }

    //make sure creatures get removed from availability
    for (int i{}; i < 6; ++i) {
        test_population.update(100.0);
    }

    if (test_population.available_size() != 0) {
        answer = false;
        std::cout << "There should be no available creatures\n";
    }

    //make sure dying works
    //to do that we will make sure te first creature is satiated and add a new born
    //then update until the others die
    for (auto& creature : test_population) {
        creature->eat();
        creature->drink();
        break;
    }
    Creature* new_born = new Creature{ Vector2d(1.0, 30), genes};
    new_born->drink();
    new_born->eat();
    test_population.insert(new_born);
    new_born = nullptr;

    for (int i{}; i < time_till_dead -time_till_available +1; ++i) {
        test_population.update(100.0);
    }
    if (test_population.size() != 2) {
        answer = false;
        std::cout << "Incorrect Number of Survivors\n";
    }
    if (test_population.available_size() != 0) {
        answer = false;
        std::cout << "Incorrect number available";
    }

    delete correct;
    return answer;
}

/*
* Here is where a bunch of tests of the analyzers start
*/

struct AnalyzerTest {
    std::string_view resource_name{};
    std::string_view test_name{};
    MovementAnalyzer& analyzer;
    SurvivalResource* resource;
    bool verbose{ true };
    double normalized_force{};
    bool increasing_stage1{true};
    double sign_x{ 1.0 };
    double sign_y{ 1.0 };
};

bool test_data_line(const std::array<double, 4>& data, const AnalyzerTest& expectations, double previous, bool increasing) {
    bool answer{ true };
    const bool incorrect_direction{ increasing ? data[1] <= previous : data[1] > previous };
    if (incorrect_direction) {
        answer = false;
    }

    const double EPSILON{ 1e-10 };
    if (abs( data[2]) > EPSILON ) {
        answer = answer && (data[2] * expectations.sign_x > 0);
    }
    if (abs(data[3]) > EPSILON) {
        answer = answer && (data[3] * expectations.sign_y > 0);
    }
    return answer;
}
//runs a bunch of test based off this pair and prints the results in nice fashion
//used in the functions testing the analyzers
bool analyzer_test(const AnalyzerTest& expectations) {
    constexpr int first_test{ 6 };
    constexpr int second_test{ 7 };
    constexpr int test_num{ first_test + second_test +1 };
    constexpr int data_types{ 4 };

    std::array<std::array<double, data_types>, test_num> data{};

    Vector2d initial_position{ (expectations.resource->get_position())};
    initial_position.normalize();
    initial_position *= 5.0;

    MovementAnalyzer::Observation observation{ expectations.resource, initial_position };
    double scale{};
    double previous{ -1.0 };
    //collect a bunch of data at different scales
    for (int i{}; i < first_test; ++i) {
        data[i][0] = scale;
        observation.update_displacement(initial_position * scale);
        Vector2d acceleration{ expectations.analyzer.calculate_acceleration(observation) };
        data[i][1] = acceleration.norm();
        data[i][2] = acceleration(0);
        data[i][3] = acceleration(1);

        if (i > 0 && !test_data_line(data[i], expectations, previous, expectations.increasing_stage1)) {
            std::cout << "Data Expectations in first stage wrong for " << expectations.resource_name << " , "
                << expectations.test_name << '\n';
            return false;
        }
        scale += 0.2;
        previous = data[i][1]; 
    }

    if (abs(data[first_test - 1][1] - expectations.normalized_force) > 1e-6) {
        std::cout << "Wrong Force Value for" << expectations.resource_name << " , " << expectations.test_name << '\n';
        return false;
    }

    for (int j{}; j < second_test; ++j) {
        const int i{ j + first_test };
        data[i][0] = scale;
        observation.update_displacement(initial_position * scale);
        Vector2d acceleration{ expectations.analyzer.calculate_acceleration(observation) };
        data[i][1] = acceleration.norm();
        data[i][2] = acceleration(0);
        data[i][3] = acceleration(1);

        if (!test_data_line(data[i], expectations, previous, false)) {
            std::cout << "Data Expectations in second stage wrong for " << expectations.resource_name << " , "
                << expectations.test_name << '\n';
            return false;
        }
        scale += 1.0;
    }
    scale = 4e2;
    constexpr int j{ test_num - 1 };
    data[j][0] = scale;
    observation.displacement = initial_position * scale;
    Vector2d acceleration{ expectations.analyzer.calculate_acceleration(observation) };
    data[j][1] = acceleration.norm();
    data[j][2] = acceleration(0);
    data[j][3] = acceleration(1);

    //now we print the data
    if (expectations.verbose) {
        std::cout << expectations.resource_name << ',' << expectations.test_name << '\n';
        std::cout << std::setprecision(4);
        for (int i{}; i < test_num; ++i) {
            for (int j{}; j < data_types; ++j) {
                std::cout << data[i][j] << "\t\t";
            }
            std::cout << '\n';
        }
    }
    return true;
}

bool test_desire_analyzer(bool verbose) {
    bool answer{ true };
    Vector2d velocity{ 10.0, 30.0 };
    MovementAnalyzer::Status status{
        {1.0, 0.5}, velocity
    };

    //the desire analyzer
    MovementAnalyzer::LinearGenotype genes{
        MovementAnalyzer::AnalyzerType::desire,
        MovementAnalyzer::NeedVector{ 0.0, -100.0 },
        80.0,
        -0.3
    };
    DesireAnalyzer pull{&genes };
    pull.consolidate(status);

    std::vector<FoodSource> bushes{ {Vector2d{40.0, 60.0}, Vector2d{20.0, -40.0}} };

    double force_size{ 45.0 };
    std::string_view test_name{ "Starting Hunger" };
    AnalyzerTest first_bush_test{ "First Bush", test_name,
        pull, &bushes[0], verbose, force_size
    };

    AnalyzerTest second_bush_test{ "Second Bush", test_name,
        pull, &bushes[1], verbose, force_size
    };
    second_bush_test.sign_y = -1.0;
    answer &= analyzer_test(first_bush_test);
    answer &= analyzer_test(second_bush_test);

    status.felt_needs[1] = 0.1;
    pull.consolidate(status);
    test_name = "Intense Hunger";
    first_bush_test.test_name = test_name;
    second_bush_test.test_name = test_name;
    double force{ 70 * 1.5 };
    first_bush_test.normalized_force = force;
    second_bush_test.normalized_force = force;
    answer &= analyzer_test(first_bush_test);
    answer &= analyzer_test(second_bush_test);

    status.felt_needs[0] = 0.3;
    pull.consolidate(status);

    status.felt_needs[1] = 1.0;
    pull.consolidate(status);
    first_bush_test.test_name = "Full";
    first_bush_test.sign_x = -1.0;
    first_bush_test.sign_y = -1.0;
    first_bush_test.normalized_force = 20 * 1.5;
    answer &= analyzer_test(first_bush_test);
    return answer;
}

bool test_friction_analyzer(bool verbose) {
    //set up creatures and resources to interact
    Vector2d velocity{ 6.0, 8.0 };
    MovementAnalyzer::Status status{
        {1.0, 0.5}, velocity
    };

    FoodSource bush{ Vector2d{30.0, 50.0} };

    //make the analyzer
    MovementAnalyzer::LinearGenotype genes{
    MovementAnalyzer::AnalyzerType::friction,
    MovementAnalyzer::NeedVector{ 0.0, -100.0 },
    80.0,
    -0.7
    };
    FrictionAnalyzer slow{ &genes };
    slow.consolidate(status);

    AnalyzerTest friction_test{ "Bush","Friction Test First", slow, &bush,verbose , false};
    friction_test.normalized_force = 30.0 * velocity.norm();
    friction_test.increasing_stage1 = false;
    friction_test.sign_x = -1;
    friction_test.sign_y = -1;

    bool answer{ analyzer_test(friction_test) };
    velocity(0) *= -1;
    slow.consolidate(status);
    friction_test.sign_x = 1;
    friction_test.test_name = "Friction Test Second";
    answer &= analyzer_test(friction_test);

    velocity(0) *= -1;
    status.felt_needs(1) = 1.0;
    friction_test.sign_y = 1.0;
    friction_test.test_name = "Friction Test Third";
    friction_test.normalized_force = 20 * velocity.norm();
    slow.consolidate(status);
    answer &= analyzer_test(friction_test);
    return answer;
}

bool test_force_manager(bool verbose) {
    ForceManager::Genotype genes{};

    genes.build_resource_flag(Ecology::bush, MetaEcology::food);
    genes.build_resource_flag(Ecology::pond, MetaEcology::water);
    genes.build_resource_flag(Ecology::gooblet_mate, MetaEcology::mate);
    ForceManager manager{ genes };

    Vector2d velocity{0, 100.0 };
    MovementAnalyzer::Status status{
        {0.8, 0.4}, velocity
    };

    std::vector<FoodSource> bushes{ { Vector2d{50.0, 0.0} }, {Vector2d{-50.0, 0.0 }} };
    MovementAnalyzer::Observation bush_sight{
        &(bushes[0]),
        bushes[0].get_position(),
    };

    ForceManager::View surroundings{ {bush_sight}, {}, {} };
    Vector2d response{};
    response = manager.response(status, surroundings);
    if (verbose) {
        std::cout << response << "\n\n";
    }

    //test a different distance away
    surroundings[0][0].from_perspective(Vector2d{ -50.0,0.0 });
    response = manager.response(status, surroundings);
    if (verbose) {
        std::cout << response << "\n\n";;
    }

    surroundings[0][0].from_perspective(Vector2d{ -100.0,0.0 });
    response = manager.response(status, surroundings);
    if (verbose) {
        std::cout << response << "\n\n";;
    }
    
    surroundings[0][0].from_perspective(Vector2d{});
    surroundings[0].push_back(MovementAnalyzer::Observation{ &bushes[1], bushes[1].get_position() });
    response = manager.response(status, surroundings);
    if (verbose) {
        std::cout << response << "\n\n";;
    }

    std::vector<WaterSource>ponds{WaterSource{ 50,100,-50,50 }};
    surroundings[1].push_back(MovementAnalyzer::Observation{&ponds[0] , Vector2d{50,0}});


    return true;
}