#include "localsearch.h"

#include <algorithm>

int LocalSearch::calcHeuristics(QList<QPoint> queens) {
    int h = 0;
    for (int i = 0; i < queens.size(); i++) {
        for (int j = i + 1; j < queens.size(); j++) {
            if (threatens(queens[i], queens[j])) {
                h++;
            }
        }
    }

    return h;
}

bool LocalSearch::threatens(QPoint queen1, QPoint queen2) {
    return queen1.x() == queen2.x() // Same column
        || queen1.y() == queen2.y() // Same row
        || abs(queen1.x() - queen2.x()) == abs(queen1.y() - queen2.y()); // Diagonal
}

QList<QList<QPoint>> LocalSearch::getAllowedStates(int boardSize, QList<QPoint> queens, int index) {
    QPoint movingQueen = queens.at(index);
    queens.removeAt(index);

    QList<QList<QPoint>> newQueensStates;

    // Create all possible moves
    QList<QPoint> possibleMoves;

    // Left
    for (int x = movingQueen.x() - 1; x >= 0; x--) {
        possibleMoves.push_back({x, movingQueen.y()});
    }
    // Right
    for (int x = movingQueen.x() + 1; x < boardSize; x++) {
        possibleMoves.push_back({x, movingQueen.y()});
    }
    // Up
    for (int y = movingQueen.y() - 1; y >= 0; y--) {
        possibleMoves.push_back({movingQueen.x(), y});
    }
    // Down
    for (int y = movingQueen.y() + 1; y < boardSize; y++) {
        possibleMoves.push_back({movingQueen.x(), y});
    }
    // Left-Up
    for (int x = movingQueen.x() - 1, y = movingQueen.y() - 1; x >= 0 && y >= 0; x--, y--) {
        possibleMoves.push_back({x, y});
    }
    // Left-Down
    for (int x = movingQueen.x() - 1, y = movingQueen.y() + 1; x >= 0 && y < boardSize; x--, y++) {
        possibleMoves.push_back({x, y});
    }
    // Right-Up
    for (int x = movingQueen.x() + 1, y = movingQueen.y() - 1; x < boardSize && y >= 0; x++, y--) {
        possibleMoves.push_back({x, y});
    }
    // Right-Down
    for (int x = movingQueen.x() + 1, y = movingQueen.y() + 1; x < boardSize && y < boardSize; x++, y++) {
        possibleMoves.push_back({x, y});
    }

    // Filter by already contained
    for (auto &move : possibleMoves) {
        if (!queens.contains(move)) {
            newQueensStates.push_back(queens);
            newQueensStates.last().push_back(move);
        }
    }

    return newQueensStates;
}

LocalSearch::State LocalSearch::hillClimb(int boardSize, QList<QPoint> &queens, int equivalentMoves) {
    State prevState = {queens, std::numeric_limits<int>::max()};
    State state;

    int steps = 0;
    int equalHeuristicsCount = 0;

    // Try to find global optimum (heuristics = 0)
    while (prevState.heuristics != 0) {
        steps++;
        state = hillClimbStep(boardSize, prevState.queens);

        // Exit if heuristics higher than previous heuristics (found local optimum)
        if (state.heuristics > prevState.heuristics) {
            break;
        }

        // Mark side moves (with equal heuristics) to soften getting stuck on plateau
        if (state.heuristics == prevState.heuristics) {
            equalHeuristicsCount++;
        } else {
            equalHeuristicsCount = 0;
        }

        prevState = state;

        // Stop search if stuck on plateau
        if (equalHeuristicsCount >= equivalentMoves) {
            break;
        }
    }

    state.steps = steps;
    return state;
}

LocalSearch::State LocalSearch::hillClimbStep(int boardSize, QList<QPoint> &queens) {
    QList<State> moveStates;

    // Perform all possible moves on all queens and calculate their heuristics
    for (int i = 0; i < queens.size(); i++) {
        for (auto &newQueens : getAllowedStates(boardSize, queens, i)) {
            moveStates.push_back({newQueens, calcHeuristics(newQueens)});
        }
    }

    // Find minimal heuristics state (all if multiple of same minimal value)
    State minState = *std::min_element(moveStates.begin(), moveStates.end());
    QList<State> minStates;
    for (auto &moveState : moveStates) {
        if (moveState.heuristics == minState.heuristics) {
            minStates.push_back(moveState);
        }
    }

    // Select random minimal heuristics state
    QList<State> states;
    std::sample(minStates.begin(), minStates.end(), std::back_inserter(states), 1, randGen);

    return states[0];
}

LocalSearch::State LocalSearch::simulatedAnnealing(int boardSize, QList<QPoint> &queens, int &tempStart, int tempChange) {
    State state = {queens, std::numeric_limits<int>::max()};
    int steps = 0;

    // Try to find global optimum (heuristics = 0)
    while (state.heuristics != 0 && tempStart > 0) {
        steps++;
        state = simulatedAnnealingStep(boardSize, state.queens, tempStart, tempChange);
    }

    state.steps = steps;
    return state;
}

LocalSearch::State LocalSearch::simulatedAnnealingStep(int boardSize, QList<QPoint> &queens, int &temp, int tempChange) {
    State state = {queens, calcHeuristics(queens)};

    // Exit if temperature at minimum
    if  (temp <= 0) {
        return state;
    }

    // Perform all possible moves on all queens
    QList<QList<QPoint>> moveStates;
    for (int i = 0; i < queens.size(); i++) {
        moveStates.append(getAllowedStates(boardSize, queens, i));
    }

    // Select a random move
    QList<QList<QPoint>> randStates;
    std::sample(moveStates.begin(), moveStates.end(), std::back_inserter(randStates), 1, randGen);
    QList<QPoint> randState = randStates.first();

    // Select random state if lower heuristics or by probability
    int randStateHeuristics = calcHeuristics(randState);
    int deltaHeuristics = randStateHeuristics - state.heuristics;
    if (deltaHeuristics < 0) {
        state = {randState, randStateHeuristics};
    } else {
        double probability = exp(-deltaHeuristics / static_cast<double>(temp));
        if (probability > distProbability(randGen)) {
            state = {randState, randStateHeuristics};
        }
    }

    temp -= tempChange;

    return state;
}

LocalSearch::State LocalSearch::localBeam(int boardSize, QList<QPoint> &queens, int nStates, int maxIters) {
    QList<State> states = localBeamInit(boardSize, queens, nStates);
    int steps = 0;

    // Try to find global optimum (heuristics = 0)
    while (states.size() > 1 && states[0].heuristics != 0 && maxIters > steps) {
        steps++;
        states = localBeamStep(boardSize, states, nStates);
    }

    states.first().steps = steps;
    return states.first();
}

QList<LocalSearch::State> LocalSearch::localBeamInit(int boardSize, QList<QPoint> &queens, int nStates) {
    // Perform all possible moves on all queens and calculate their heuristics
    QList<State> moveStates;
    for (int i = 0; i < queens.size(); i++) {
        for (auto &newQueens : getAllowedStates(boardSize, queens, i)) {
            moveStates.push_back({newQueens, calcHeuristics(newQueens)});
        }
    }

    // Select random states
    QList<State> states;
    std::sample(moveStates.begin(), moveStates.end(), std::back_inserter(states), nStates, randGen);
    std::shuffle(states.begin(), states.end(), randGen); // Randomize (sample keeps relative order)

    return states;
}

QList<LocalSearch::State> LocalSearch::localBeamStep(int boardSize, QList<State> &states, int nStates) {
    // Sort by heuristics
    std::sort(states.begin(), states.end());

    // Exit if found result
    if (states.first().heuristics == 0) {
        return {states.first()};
    }

    // Select nStates best states
    states = QList<State>(states.mid(0, nStates));

    // Perform all possible moves on all queens and calculate their heuristics
    QList<State> newStates;
    for (auto &state : states) {
        for (int i = 0; i < state.queens.size(); i++) {
            for (auto &newQueens : getAllowedStates(boardSize, state.queens, i)) {
                newStates.push_back({newQueens, calcHeuristics(newQueens)});
            }
        }
    }

    return newStates;
}

LocalSearch::State LocalSearch::genetic(int boardSize, QList<QPoint> &queens, int nStates, int elitePerc, double crossProb, double mutationProb, int generations) {
    QList<State> states = geneticInit(boardSize, queens, nStates);
    int steps = 0;

    // Try to find global optimum (heuristics = 0)
    while (states.size() > 1 && states.first().heuristics != 0 && generations > steps) {
        steps++;
        states = geneticStep(boardSize, states, nStates, elitePerc, crossProb, mutationProb);
    }

    states.first().steps = steps;
    return states.first();
}

QList<LocalSearch::State> LocalSearch::geneticInit(int boardSize, QList<QPoint> &queens, int nStates) {
    // Perform all possible moves on all queens and calculate their heuristics
    QList<State> moveStates;
    for (int i = 0; i < queens.size(); i++) {
        for (auto &newQueens : getAllowedStates(boardSize, queens, i)) {
            moveStates.push_back({newQueens, calcHeuristics(newQueens)});
        }
    }

    // Select random states
    QList<State> states;
    std::sample(moveStates.begin(), moveStates.end(), std::back_inserter(states), nStates, randGen);
    std::shuffle(states.begin(), states.end(), randGen); // Randomize (sample keeps relative order)

    return states;
}

QList<LocalSearch::State> LocalSearch::geneticStep(int boardSize, QList<State> &states, int nStates, int elitePerc, double crossProb, double mutationProb) {
    // Sort by heuristics
    std::sort(states.begin(), states.end());

    // Exit if found result
    if (states.first().heuristics == 0) {
        return {states.first()};
    }

    QList<State> newStates;

    // Copy elites if population high enough to satisfy required elite percentage
    int elites = states.size() / elitePerc;
    for (int i = 0; i < elites; i++) {
        newStates.push_back(states[i]);
    }

    for (int i = 0; i < nStates / 2 && i < states.size(); i++) {
        QList<State> selected = {};

        // Choose 2 parents with random selection
        std::sample(states.begin(), states.end(), std::back_inserter(selected), 2, randGen);
        std::shuffle(states.begin(), states.end(), randGen); // Randomize (sample keeps relative order)

        // Uniform crossover by probablity
        if (crossProb > distProbability(randGen)) {
            // Exchange each queen couple by probability
            State &state1 = selected[0];
            State &state2 = selected[1];

            for (int i = 0; i < state1.queens.size(); i++) {
                for (int j = i + 1; j < state2.queens.size(); j++) {
                    QPoint queen1 = state1.queens[i];
                    QPoint queen2 = state2.queens[j];

                    if (crossProb > distProbability(randGen)) {
                        if (!state1.queens.contains(queen2) && !state2.queens.contains(queen1)) {
                            state1.queens.replace(i, queen2);
                            state2.queens.replace(j, queen1);
                            state1.heuristics = calcHeuristics(state1.queens);
                            state2.heuristics = calcHeuristics(state2.queens);
                        }
                    }
                }
            }
        }

        std::uniform_int_distribution<> distQueen(0, boardSize - 1);
        for (auto &sel : selected) {
            // Mutate by probability
            if (mutationProb > distProbability(randGen)) {
                QList<QList<QPoint>> allowedStates = getAllowedStates(boardSize, sel.queens, distQueen(randGen));
                QList<QList<QPoint>> randStates;
                std::sample(allowedStates.begin(), allowedStates.end(), std::back_inserter(randStates), 1, randGen);
                QList<QPoint> randState = randStates.first();

                sel = {randState, calcHeuristics(randState)};
            }
        }

        newStates.append({selected[0], selected[1]});
    }

    return newStates;
}
