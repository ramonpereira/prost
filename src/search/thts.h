#ifndef THTS_H
#define THTS_H

#include "uniform_evaluation_search.h"
#include "utils/timer.h"
#include "action_selection.h"
#include "outcome_selection.h"
#include "backup_function.h"

// THTS, Trial-based Heuristic Tree Search, is the implementation of the
// abstract framework described in the ICAPS 2013 paper "Trial-based Heuristic
// Tree Search for Finite Horizon MDPs" (Keller & Helmert). The described
// ingredients are implemented in four classes (1-4) or as functions in the
// THTS class (5-6)

// 1. ActionSelection

// 2. Outcome Selection

// 3. BackupFunction

// 4. Initializer

// 5. continueTrial()

// 6. recommendAction()

struct SearchNode {
    SearchNode(double const& _prob, int const& _stepsToGo) :
        children(),
        immediateReward(0.0),
        prob(_prob),
        stepsToGo(_stepsToGo),
        futureReward(-std::numeric_limits<double>::max()),
        numberOfVisits(0),
        solved(false) {}

    ~SearchNode() {
        for (unsigned int i = 0; i < children.size(); ++i) {
            if (children[i]) {
                delete children[i];
            }
        }
    }

    void reset(double const& _prob, int const& _stepsToGo) {
        children.clear();
        immediateReward = 0.0;
        prob = _prob;
        stepsToGo = _stepsToGo;
        futureReward = -std::numeric_limits<double>::max();
        numberOfVisits = 0;
        solved = false;
    }

    double getExpectedRewardEstimate() const {
        return immediateReward + futureReward;
    }

    double getExpectedFutureRewardEstimate() const {
        return futureReward;
    }

    void print(std::ostream& out, std::string indent = "") const {
        if (solved) {
            out << indent << "SOLVED with: " << getExpectedRewardEstimate() <<
            " (in " << numberOfVisits << " real visits)" << std::endl;
        } else {
            out << indent << getExpectedRewardEstimate() << " (in " <<
            numberOfVisits << " real visits)" << std::endl;
        }
    }

    std::vector<SearchNode*> children;

    double immediateReward;
    double prob;
    int stepsToGo;
    
    double futureReward;
    int numberOfVisits;

    bool solved;
};

class THTS : public ProbabilisticSearchEngine {
public:
    enum TerminationMethod {
        TIME, //stop after timeout sec
        NUMBER_OF_TRIALS, //stop after maxNumberOfTrials trials
        TIME_AND_NUMBER_OF_TRIALS //stop after timeout sec or maxNumberOfTrials trials, whichever comes first
    };

    // Parameter setter
    virtual bool setValueFromString(std::string& param, std::string& value);

    // Disables caching because memory becomes sparse.
    void disableCaching();

    // Learns parameter values from a random training set.
    virtual void learn();

    // Start the search engine as main search engine
    bool estimateBestActions(State const& _rootState,
                             std::vector<int>& bestActions);

    // Start the search engine for Q-value estimation
    bool estimateQValues(State const& /*_rootState*/,
                         std::vector<int> const& /*actionsToExpand*/,
                         std::vector<double>& /*qValues*/) {
        assert(false);
        return false;
    }

    // Parameter setter
    virtual void setActionSelection(ActionSelection* _actionSelection) {
        actionSelection = _actionSelection;
    }

    virtual void setOutcomeSelection(OutcomeSelection* _outcomeSelection) {
        outcomeSelection = _outcomeSelection;
    }

    virtual void setBackupFunction(BackupFunction* _backupFunction) {
        backupFunction = _backupFunction;
    }
    
    virtual void setMaxSearchDepth(int _maxSearchDepth) {
        SearchEngine::setMaxSearchDepth(_maxSearchDepth);

        if (initializer) {
            initializer->setMaxSearchDepth(_maxSearchDepth);
        }
    }

    virtual void setTerminationMethod(
            THTS::TerminationMethod _terminationMethod) {
        terminationMethod = _terminationMethod;
    }

    virtual void setMaxNumberOfTrials(int _maxNumberOfTrials) {
        maxNumberOfTrials = _maxNumberOfTrials;
    }

    virtual void setInitializer(SearchEngine* _initializer) {
        if (initializer) {
            delete initializer;
        }
        initializer = _initializer;
    }

    virtual void setNumberOfInitialVisits(int _numberOfInitialVisits) {
        numberOfInitialVisits = _numberOfInitialVisits;
    }

    virtual void setNumberOfNewDecisionNodesPerTrial(
            int _numberOfNewDecisionNodesPerTrial) {
        numberOfNewDecisionNodesPerTrial = _numberOfNewDecisionNodesPerTrial;
    }

    virtual void setMaxNumberOfNodes(int _maxNumberOfNodes) {
        maxNumberOfNodes = _maxNumberOfNodes;
        // Resize the node pool and give it a "safety net" of 20000 nodes (this
        // is because the termination criterion is checked only at the root and
        // not in the middle of a trial)
        nodePool.resize(maxNumberOfNodes + 20000, nullptr);
    }

    virtual void setSelectMostVisited(bool _selectMostVisited) {
        selectMostVisited = _selectMostVisited;
    }

    virtual void setHeuristicWeight(double _heuristicWeight) {
        heuristicWeight = _heuristicWeight;
    }

    // Used only for testing
    void setCurrentRootNode(SearchNode* node) {
        currentRootNode = node;
    }

    // Getter methods for classes that implement ingredients
    SearchNode* getRootNode();
    SearchNode* getDecisionNode(double const& _prob);
    SearchNode* getChanceNode(double const& _prob);

    SearchNode const* getCurrentRootNode() const {
        return currentRootNode;
    }

    SearchNode const* getTipNodeOfTrial() const {
        return tipNodeOfTrial;
    }

    // Printer
    virtual void print(std::ostream& out);
    virtual void printStats(std::ostream& out,
                            bool const& printRoundStats,
                            std::string indent = "") const;

    THTS(std::string _name) :
        ProbabilisticSearchEngine(_name),
        actionSelection(nullptr),
        outcomeSelection(nullptr),
        backupFunction(nullptr),
        currentRootNode(nullptr),
        chosenOutcome(nullptr),
        tipNodeOfTrial(nullptr),
        states(SearchEngine::horizon + 1),
        stepsToGoInCurrentState(SearchEngine::horizon),
        stepsToGoInNextState(SearchEngine::horizon - 1),
        appliedActionIndex(-1),
        trialReward(0.0),
        currentTrial(0),
        initializer(nullptr),
        initialQValues(SearchEngine::numberOfActions, 0.0),
        initializedDecisionNodes(0),
        lastUsedNodePoolIndex(0),
        terminationMethod(THTS::TIME),
        maxNumberOfTrials(0),
        numberOfInitialVisits(1),
        numberOfNewDecisionNodesPerTrial(1),
        selectMostVisited(false),
        heuristicWeight(0.5),
        numberOfRuns(0),
        cacheHits(0),
        accumulatedNumberOfStepsToGoInFirstSolvedRootState(0),
        firstSolvedFound(false),
        accumulatedNumberOfTrialsInRootState(0),
        accumulatedNumberOfSearchNodesInRootState(0) {
        setMaxNumberOfNodes(24000000);
        setTimeout(1.0);
    }

protected:
    // Main search functions
    void visitDecisionNode(SearchNode* node);
    void visitChanceNode(SearchNode* node);
    void visitDummyChanceNode(SearchNode* node);

    // Initialization of different search phases
    void initRound();
    void initStep(State const& _rootState);
    void initTrial();
    void initTrialStep();

    // Trial length determinization
    virtual bool continueTrial(SearchNode* /*node*/) {
        return initializedDecisionNodes < numberOfNewDecisionNodesPerTrial;
    }

    // Initialization of nodes
    virtual void initializeDecisionNode(SearchNode* node);

    // Recommendation function
    virtual void recommendAction(std::vector<int>& bestActions);

    // Determines if the current state has been solved before or can be solved now
    bool currentStateIsSolved(SearchNode* node);

    // If the root state is a reward lock or has only one reasonable
    // action, noop or the only reasonable action is returned
    int getUniquePolicy();

    // Determine if the termination criterion is fullfilled or if we
    // want another trial
    bool moreTrials();

private:
    // Ingredients that are implemented externally
    ActionSelection* actionSelection;
    OutcomeSelection* outcomeSelection;
    BackupFunction* backupFunction;

    // Search nodes used in trials
    SearchNode* currentRootNode;
    SearchNode* chosenOutcome;

    // The tip node of a trial is the first node that is encountered that
    // requires initialization of a child
    SearchNode* tipNodeOfTrial;

    // States used in trials
    std::vector<PDState> states;
    int stepsToGoInCurrentState;
    int stepsToGoInNextState;

    // Currently simulated action
    int appliedActionIndex;

    // The accumulated reward that has been achieved in the current trial (the
    // rewards are accumulated in reverse order during the backup phase, such
    // that it reflects the future reward in each node when it is backed up)
    double trialReward;

    // Counter for the number of trials
    int currentTrial;

    // Max search depth for the current step
    int maxSearchDepthForThisStep;

    // The number of steps that are "cut off" if a limited search
    // depth is used
    int ignoredSteps;

    // Variable used to navigate through chance node layers
    int chanceNodeVarIndex;

    // Index of the last variable with non-deterministic outcome in the current transition
    int lastProbabilisticVarIndex;

    // Search engine that estimates Q-values for initialization of
    // decison node children
    SearchEngine* initializer;
    std::vector<double> initialQValues;

    // Counter for number of initialized decision nodes in the current
    // trial
    int initializedDecisionNodes;

    // Memory management (nodePool)
    int lastUsedNodePoolIndex;
    std::vector<SearchNode*> nodePool;

    // The timer used for timeout check
    Timer timer;

protected:    
    // Parameter
    THTS::TerminationMethod terminationMethod;
    int maxNumberOfTrials;
    int numberOfInitialVisits;
    int numberOfNewDecisionNodesPerTrial;
    int maxNumberOfNodes;
    bool selectMostVisited;
    double heuristicWeight;

    // Statistics
    int numberOfRuns;
    int cacheHits;
    int accumulatedNumberOfStepsToGoInFirstSolvedRootState;
    bool firstSolvedFound;
    int accumulatedNumberOfTrialsInRootState;
    int accumulatedNumberOfSearchNodesInRootState;

    // Tests which access private members
    friend class THTSTest;
    friend class BFSTestSearch;
    friend class MCUCTTestSearch;
    friend class UCTBaseTestSearch;
};

#endif
