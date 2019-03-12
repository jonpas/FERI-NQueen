#pragma once

#include <random>
#include <QList>

namespace LocalSearch {
    static std::mt19937 randGen = std::mt19937{std::random_device{}()};
    static std::uniform_real_distribution<> distProbability(0.0, 1.0);

    struct State {
        QList<QPoint> queens;
        int heuristics;
        int steps = 0;

        bool operator<(const State &s) const {
            return heuristics < s.heuristics;
        }
    };

    // Calculate heuristics of queen positions (number of pairs of queens attacking each other)
    int calcHeuristics(QList<QPoint> queens);
    // Checks if queens threaten each other
    bool threatens(QPoint queen1, QPoint queen2);

    QList<QList<QPoint>> getAllowedStates(int boardSize, QList<QPoint> queens, int index);

    // Algorithms
    State hillClimb(int boardSize, QList<QPoint> &queens, int equivalentMoves);
    State hillClimbStep(int boardSize, QList<QPoint> &queens);

    State simulatedAnnealing(int boardSize, QList<QPoint> &queens, int &tempStart, int tempChange);
    State simulatedAnnealingStep(int boardSize, QList<QPoint> &queens, int &temp, int tempChange);

    State localBeam(int boardSize, QList<QPoint> &queens, int nStates, int maxIters);
    QList<State> localBeamInit(int boardSize, QList<QPoint> &queens, int nStates);
    QList<State> localBeamStep(int boardSize, QList<State> &states, int nStates);

    // State = Chromosome, Queen = Gene, nState = Population
    State genetic(int boardSize, QList<QPoint> &queens, int nStates, int elitePerc, double crossProb, double mutationProb, int generations);
    QList<State> geneticInit(int boardSize, QList<QPoint> &queens, int nStates);
    QList<State> geneticStep(int boardSize, QList<State> &states, int nStates, int elitePerc, double crossProb, double mutationProb);
};
