#pragma once

#include <QList>
#include <random>

namespace LocalSearch {
    static std::mt19937 randGen = std::mt19937{std::random_device{}()};

    struct State {
        QList<QPoint> queens;
        int heuristics;

        bool operator<(const State &s) const {
            return heuristics < s.heuristics;
        }
    };

    // Calculate heuristics of queen positions (number of pairs of queens attacking each other)
    int calcHeuristics(QList<QPoint> queens);
    // Checks if queens threaten each other
    bool threatens(QPoint queen1, QPoint queen2);

    QList<QList<QPoint>> getAllowedStates(int boardSize, QList<QPoint> queens, int index);
    bool onBoard(int boardSize, QPoint queen);

    // Algorithms
    State hillClimb(int boardSize, QList<QPoint> &queens, int equivalentMoves);
    State hillClimbStep(int boardSize, QList<QPoint> &queens);

    State simulatedAnnealing(int boardSize, QList<QPoint> &queens, int &tempStart, int tempChange);
    State simulatedAnnealingStep(int boardSize, QList<QPoint> &queens, int &temp, int tempChange);

    State localBeam(int boardSize, QList<QPoint> &queens, int nStates, int maxIters);
    QList<State> localBeamInit(int boardSize, QList<QPoint> &queens, int nStates);
    QList<State> localBeamStep(int boardSize, QList<State> &states, int nStates);

    /*State genetic(int boardSize, QList<QPoint> &queens, int populationSize, int elitePerc, int crossProb, int mutationProb, int generations);*/
};
