#include "localsearch.h"
#include <algorithm>
#include <QDebug>

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

    QList<QPoint> possibleMoves = {
        {movingQueen.x() - 1, movingQueen.y()},     // Left
        {movingQueen.x() + 1, movingQueen.y()},     // Right
        {movingQueen.x(),     movingQueen.y() - 1}, // Up
        {movingQueen.x(),     movingQueen.y() + 1}, // Down
        {movingQueen.x() - 1, movingQueen.y() - 1}, // Left-Up
        {movingQueen.x() - 1, movingQueen.y() + 1}, // Left-Down
        {movingQueen.x() + 1, movingQueen.y() - 1}, // Right-Up
        {movingQueen.x() + 1, movingQueen.y() + 1}  // Right-Down
    };

    for (auto &move : possibleMoves) {
        if (onBoard(boardSize, move) && !queens.contains(move)) {
            newQueensStates.push_back(queens);
            newQueensStates.last().push_back(move);
        }
    }

    return newQueensStates;
}

bool LocalSearch::onBoard(int boardSize, QPoint queen) {
    return queen.x() >= 0 && queen.x() < boardSize // Column
        && queen.y() >= 0 && queen.y() < boardSize; // Row
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

    qDebug() << "steps:" << steps; // TODO Status bar
    return state;
}

LocalSearch::State LocalSearch::hillClimbStep(int boardSize, QList<QPoint> &queens) {
    QList<State> moveStates;

    // Perform all possible moves on all queens and calculate each move's heuristics
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

    qDebug() << "min:" << states[0].heuristics;
    return states[0];
}

/*void LocalSearch::simulatedAnnealing(int boardSize, QList<QPoint> *queens, int tempStart, int tempChange) {

}

void LocalSearch::localBeam(int boardSize, QList<QPoint> *queens, int states) {

}

void LocalSearch::genetic(int boardSize, QList<QPoint> *queens, int populationSize, int elitePerc, int crossProb, int mutationProb, int generations) {

}*/
