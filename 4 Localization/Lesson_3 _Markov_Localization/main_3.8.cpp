#include <iostream>
#include <vector>

// initialize priors assuming vehicle at landmark +/- 1.0 meters position stdev
std::vector<float> initialize_priors(
    int map_size,
    std::vector<float> landmark_positions,
    float position_stdev);

int main() {
    // set standard deviation of position
    float position_stdev = 1.0f;

    // set map horizon distance in meters
    int map_size = 25;

    // initialize landmarks
    std::vector<float> landmark_positions {5, 10, 20};

    // initialize priors
    std::vector<float> priors = initialize_priors(map_size, landmark_positions,
                                             position_stdev);

    // print values to stdout
    for (int p = 0; p < priors.size(); ++p) {
        std::cout << priors[p] << std::endl;
    }

    return 0;
}

std::vector<float> initialize_priors(
    int map_size,
    std::vector<float> landmark_positions,
    float position_stdev) {

    // initialize priors assuming vehicle at landmark +/- 1.0 meters position stdev

    // set all priors to 0.0
    std::vector<float> priors(map_size, 0.0);
    float prop = 1. / landmark_positions.size() / (2 * position_stdev + 1);
    for (int i = 0; i < map_size; ++i) {
        for (float landmark : landmark_positions) {
            if (std::abs(landmark - i) <= position_stdev) {
                priors.at(i) += prop;
            }
        }
    }
    return priors;
}